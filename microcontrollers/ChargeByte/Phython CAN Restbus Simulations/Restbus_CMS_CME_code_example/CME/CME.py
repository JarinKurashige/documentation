import can.logger
import cantools
import sys
import traceback
import time

EVSEMaxCurrent = 500
EVSEMaxVoltage = 1000
EVSEMaxPower = 500000
EVSEEnergyToBeDelivered = 500000

EVSEMinCurrent = 0
EVSEMinVoltage = 10
EVSEPeakCurrentRipple = 5
EVSECurrentRegulationTolerance = 3

AUTHENTICATION_RESPONSE_DELAY = 1  # seconds
PARAMETER_RESPONSE_DELAY = 1  # seconds
ISOLATION_RESPONSE_DELAY = 1  # seconds


def cmePrint(*args, **kwargs):
    print("CME:"+" ".join(map(str, args))+"", **kwargs)


def cme_charge_loop(can_tester, stopPtr, cms_tester):
    can_tester.messages['EVSEDCStatus']['EVSEPresentCurrent'] = 'SNA'
    can_tester.messages['EVSEDCStatus']['EVSEPresentVoltage'] = 'SNA'
    can_tester.messages['EVSEDCStatus']['EVSEIsolationStatus'] = 'Invalid'
    can_tester.messages['EVSEDCStatus']['EVSEFreeService'] = 1
    can_tester.messages['EVSEDCStatus']['EVSEVoltageLimitAchieved'] = 'False'
    can_tester.messages['EVSEDCStatus']['EVSEStatusCode'] = 'EVSE_Ready'
    can_tester.messages['EVSEDCStatus']['EVSECurrentLimitAchieved'] = 'False'
    can_tester.messages['EVSEDCStatus']['EVSEPowerLimitAchieved'] = 'False'
    can_tester.messages['EVSEDCStatus']['EVSEProcessingCA'] = 'SNA'
    can_tester.messages['EVSEDCStatus']['EVSEProcessingCPD'] = 'SNA'
    can_tester.messages['EVSEDCStatus']['EVSEProcessingCC'] = 'SNA'

    can_tester.messages['EVSEID']['EVSEID'] = 0xDEADBEEF

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', timeout=10) is not None, "CME dead?"
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'Default'},
                             timeout=10) is not None, "CME not unplugged?"

    cmePrint('Please plug in...')

    can_tester.flush_input()
    notB = None
    while stopPtr() and notB is None:
        notB = can_tester.expect('ChargeInfo', {'ControlPilotState': 'B'}, timeout=1)
    if stopPtr() is False:
        return
    cmePrint('Pilot State B detected')
    if cms_tester:
        cms_tester.messages['EVPlugStatus']['EVControlPilotDutyCycle'] = 5
        cms_tester.messages['EVPlugStatus']['EVControlPilotState'] = 'B'
        cms_tester.messages['EVPlugStatus']['EVProximityPinState'] = 'Type2_Connected13A'
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'Init'}, timeout=20), "No HLC?"
    cmePrint('Charge Init')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'Authentication'}, timeout=20)
    cmePrint('Authentication')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    can_tester.messages['EVSEDCMaxLimits']['EVSEMaxCurrent'] = EVSEMaxCurrent
    can_tester.messages['EVSEDCMaxLimits']['EVSEMaxVoltage'] = EVSEMaxVoltage
    can_tester.messages['EVSEDCMaxLimits']['EVSEMaxPower'] = EVSEMaxPower
    can_tester.messages['EVSEDCMaxLimits']['EVSEEnergyToBeDelivered'] = EVSEEnergyToBeDelivered

    can_tester.messages['EVSEDCRegulationLimits']['EVSEMinCurrent'] = EVSEMinCurrent
    can_tester.messages['EVSEDCRegulationLimits']['EVSEMinVoltage'] = EVSEMinVoltage
    can_tester.messages['EVSEDCRegulationLimits']['EVSEPeakCurrentRipple'] = EVSEPeakCurrentRipple
    can_tester.messages['EVSEDCRegulationLimits']['EVSECurrentRegulationTolerance'] = EVSECurrentRegulationTolerance

    can_tester.messages['EVSEDCStatus']['EVSEProcessingCA'] = 'Ongoing'

    time.sleep(AUTHENTICATION_RESPONSE_DELAY)

    can_tester.messages['EVSEDCStatus']['EVSEProcessingCA'] = 'Finished'

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'Parameter'}, timeout=3)
    cmePrint('Parameter')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    can_tester.messages['EVSEDCStatus']['EVSEProcessingCPD'] = 'Ongoing'

    time.sleep(PARAMETER_RESPONSE_DELAY)

    EVDCMaxLimits = can_tester.expect('EVDCMaxLimits')
    EVMaxVoltage = EVDCMaxLimits['EVMaxVoltage']
    EVMaxPower = EVDCMaxLimits['EVMaxPower']
    EVMaxCurrent = EVDCMaxLimits['EVMaxCurrent']

    can_tester.messages['EVSEDCStatus']['EVSEProcessingCPD'] = 'Finished'

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'Isolation'})
    cmePrint('Isolation')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    can_tester.messages['EVSEDCStatus']['EVSEPresentVoltage'] = EVMaxVoltage
    can_tester.messages['EVSEDCStatus']['EVSEProcessingCC'] = 'Ongoing'
    can_tester.messages['EVSEDCStatus']['EVSEIsolationStatus'] = 'Invalid'

    if cms_tester:
        assert can_tester.expect('ChargeInfo', {'ControlPilotState': 'C'}, timeout=30)
        cmePrint("Got State C, continue")
        cms_tester.messages['EVPlugStatus']['EVControlPilotState'] = 'C'

    time.sleep(ISOLATION_RESPONSE_DELAY)

    can_tester.messages['EVSEDCStatus']['EVSEIsolationStatus'] = 'Valid'
    can_tester.messages['EVSEDCStatus']['EVSEPresentVoltage'] = 0
    can_tester.messages['EVSEDCStatus']['EVSEProcessingCC'] = 'Finished'

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'PreCharge'}, timeout=10)
    cmePrint('PreCharge')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    pre_charge_voltage = 'SNA'
    while (pre_charge_voltage == 'SNA'):
        charge_targets = can_tester.expect('EVDCChargeTargets')
        pre_charge_voltage = str(charge_targets['EVPreChargeVoltage'])

    cmePrint("Got precharge volttage: %s   " % pre_charge_voltage)

    can_tester.messages['EVSEDCStatus']['EVSEPresentCurrent'] = 0.1

    delays = [0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2]
    for index, delay in enumerate(delays):
        charge_targets = can_tester.expect('EVDCChargeTargets')
        pre_charge_voltage = charge_targets['EVPreChargeVoltage']
        voltage = (index + 1) / len(delays) * pre_charge_voltage
        time.sleep(delay)

        cmePrint(" present Voltage: %.1fV target %.1fV" % (voltage, pre_charge_voltage))

        can_tester.messages['EVSEDCStatus']['EVSEPresentVoltage'] = voltage

    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'StateMachineState': 'Charge'}, timeout=3)
    cmePrint('Charge')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    target_current = 'SNA'
    target_voltage = 'SNA'

    while target_voltage == 'SNA' or target_current == 'SNA':
        charge_targets = can_tester.expect('EVDCChargeTargets')
        target_current = charge_targets['EVTargetCurrent']
        target_voltage = charge_targets['EVTargetVoltage']

    cnt = 0
    charging = True
    while charging:
        charge_targets = can_tester.expect('EVDCChargeTargets')
        target_current = charge_targets['EVTargetCurrent']
        target_voltage = charge_targets['EVTargetVoltage']
        cmePrint(" %10d sec Voltage: %f, Current: %f" % (cnt, target_voltage, target_current))
        time.sleep(1)
        cnt += 1
        can_tester.flush_input()
        charge_info = can_tester.expect('ChargeInfo')
        if 'StateMachineState' in charge_info and charge_info['StateMachineState'] != 'Charge':
            charging = False

    can_tester.messages['EVSEDCStatus']['EVSEPresentCurrent'] = target_current * 0.9
    can_tester.messages['EVSEDCStatus']['EVSEPresentVoltage'] = target_voltage * 0.9

    cmePrint("Charging ended")

    can_tester.flush_input()
    stopped = False
    while not stopped:
        charge_info = can_tester.expect('ChargeInfo')
        if ('StateMachineState' in charge_info) and (charge_info['StateMachineState'] in ['ShutOff', 'Welding',
                                                                                          'Error', 'StopCharge']):
            stopped = True
    if cms_tester:
        assert can_tester.expect('ChargeInfo', {'ControlPilotState': 'B'}, timeout=40)
        cmePrint("Set CMS to State-B 100%")
        cms_tester.messages['EVPlugStatus']['EVControlPilotDutyCycle'] = 100
        cms_tester.messages['EVPlugStatus']['EVControlPilotState'] = 'B'
        cms_tester.messages['EVPlugStatus']['EVProximityPinState'] = 'Type2_Connected13A'

    # assert can_tester.expect('ChargeInfo', {'StateMachineState': 'ShutOff'})
    cmePrint('ShutOff')
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    cmePrint('Waiting for Unplug')
    can_tester.flush_input()
    assert can_tester.expect('ChargeInfo', {'ControlPilotState': 'A'})
    if cms_tester:
        cmePrint("Set CMS to State-E 0%")
        cms_tester.messages['EVPlugStatus']['EVControlPilotDutyCycle'] = 0
        cms_tester.messages['EVPlugStatus']['EVControlPilotState'] = 'E'
        cms_tester.messages['EVPlugStatus']['EVProximityPinState'] = 'Not_Connected'
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


def cme_charging(can_tester, stopPtr, cms_tester=None):
    # Endless loop for continuous testing
    while stopPtr() is True:
        can_tester.start()

        try:
            cme_charge_loop(can_tester, stopPtr, cms_tester)
        except AssertionError:
            _, _, tb = sys.exc_info()
            tb_info = traceback.extract_tb(tb)
            filename, line, func, text = tb_info[-1]

            cmePrint('An error occurred on line {} in statement {}'.format(line, text))
            cmePrint('Restart charge cycle at the beginning.\nWaiting for Unplug...')
            can_tester.expect('ChargeInfo', {'ControlPilotState': 'A'})
            if cms_tester:
                cmePrint("Set CMS to State-E 0%")
                cms_tester.messages['EVPlugStatus']['EVControlPilotDutyCycle'] = 0
                cms_tester.messages['EVPlugStatus']['EVControlPilotState'] = 'E'
                cms_tester.messages['EVPlugStatus']['EVProximityPinState'] = 'Not_Connected'

        can_tester.stop()

    cmePrint("stopped")


'''-------------------------------------------------------------------------------------------------
MAIN
-------------------------------------------------------------------------------------------------'''
if __name__ == "__main__":
    database_dbc = cantools.db.load_file("ISC_CME.dbc")
    canBus = can.interface.Bus(bustype="pcan", channel="PCAN_USBBUS1", bitrate="500000")
    tester = cantools.tester.Tester('CME', database_dbc, canBus, 'ISC_CME')

    cme_charging(tester, lambda: True)
