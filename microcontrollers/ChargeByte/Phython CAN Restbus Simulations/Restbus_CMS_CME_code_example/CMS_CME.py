import sys
import threading

import can.logger
import cantools
import time

from CMS.CMS import cms_charging
from CME.CME import cme_charging

'''-------------------------------------------------------------------------------------------------
MAIN
-------------------------------------------------------------------------------------------------'''
keepRunning = True


def isRunning():
    global keepRunning
    return keepRunning


if __name__ == "__main__":
    mode = "GPIO"
    if len(sys.argv) > 1:
        if sys.argv[1] == "CAN":
            mode = "CAN"
            print("Simulation started in CAN mode!! You have to control the C-State manually!")

    cms_dbc = cantools.db.load_file("CMS/ISC_CMS_Automotive.dbc")
    cme_dbc = cantools.db.load_file("CME/ISC_CME.dbc")
    canBus = can.interface.Bus(bustype="pcan", channel="PCAN_USBBUS1", bitrate="500000")
    tester_cms = cantools.tester.Tester('CMS', cms_dbc, canBus, 'ISC_CMS_Automotive')
    tester_cme = cantools.tester.Tester('CME', cme_dbc, canBus, 'ISC_CME')

    th_cms = threading.Thread(target=cms_charging, args=(tester_cms, isRunning, mode))
    th_cme = threading.Thread(target=cme_charging, args=(tester_cme, isRunning, tester_cms if mode == "CAN" else None))

    th_cms.start()
    th_cme.start()

    input("press enter to exit\n")

    keepRunning = False
    print("Main stopped")
