"""
Package **openblt** is a python wrapper for the OpenBLT Host Library (`LibOpenBLT <https://www.feaser.com/openblt/doku.php?id=manual:libopenblt>`_).  

Have a look at the function headers inside openblt.lib for details on how to call the functions, including examples.
"""
__docformat__ = 'reStructuredText'
# ***************************************************************************************
#  File Name: __init__.py
#
# ---------------------------------------------------------------------------------------
#                           C O P Y R I G H T
# ---------------------------------------------------------------------------------------
#    Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
#
# ---------------------------------------------------------------------------------------
#                             L I C E N S E
# ---------------------------------------------------------------------------------------
# This file is part of OpenBLT. It is released under a commercial license. Refer to
# the license.pdf file for the exact licensing terms. It should be located in
# '.\Doc\license.pdf'. Contact Feaser if you did not receive the license.pdf file.
# 
# In a nutshell, you are allowed to modify and use this software in your closed source
# proprietary application, provided that the following conditions are met:
# 1. The software is not released or distributed in source (human-readable) form.
# 2. These licensing terms and the copyright notice above are not changed.
# 
# This software has been carefully tested, but is not guaranteed for any particular
# purpose. Feaser does not offer any warranties and does not guarantee the accuracy,
# adequacy, or completeness of the software and is not responsible for any errors or
# omissions or the results obtained from use of the software.
#
# ***************************************************************************************


# ***************************************************************************************
#  Imports
# ***************************************************************************************
from openblt.lib import BLT_RESULT_OK
from openblt.lib import BLT_RESULT_ERROR_GENERIC


# ***************************************************************************************
#              V E R S I O N   I N F O R M A T I O N
# ***************************************************************************************
from openblt.lib import version_get_number
from openblt.lib import version_get_string


# ***************************************************************************************
#              S E S S I O N   /   T R A N S P O R T   L A Y E R S
# ***************************************************************************************
from openblt.lib import BLT_SESSION_XCP_V10
from openblt.lib import BLT_TRANSPORT_XCP_V10_RS232
from openblt.lib import BLT_TRANSPORT_XCP_V10_CAN
from openblt.lib import BLT_TRANSPORT_XCP_V10_USB
from openblt.lib import BLT_TRANSPORT_XCP_V10_NET
from openblt.lib import BltSessionSettingsXcpV10
from openblt.lib import BltTransportSettingsXcpV10Rs232
from openblt.lib import BltTransportSettingsXcpV10Can
from openblt.lib import BltTransportSettingsXcpV10Net
from openblt.lib import session_init
from openblt.lib import session_terminate
from openblt.lib import session_start
from openblt.lib import session_stop
from openblt.lib import session_clear_memory
from openblt.lib import session_write_data
from openblt.lib import session_read_data


# ***************************************************************************************
#              F I R M W A R E   D A T A
# ***************************************************************************************
from openblt.lib import BLT_FIRMWARE_PARSER_SRECORD
from openblt.lib import firmware_terminate
from openblt.lib import firmware_init
from openblt.lib import firmware_load_from_file
from openblt.lib import firmware_save_to_file
from openblt.lib import firmware_get_segment_count
from openblt.lib import firmware_get_segment
from openblt.lib import firmware_add_data
from openblt.lib import firmware_remove_data
from openblt.lib import firmware_clear_data


# ***************************************************************************************
#              G E N E R I C   U T I L I T I E S
# ***************************************************************************************
from openblt.lib import util_crc16_calculate
from openblt.lib import util_crc32_calculate
from openblt.lib import util_time_get_system_time
from openblt.lib import util_time_delay_ms
from openblt.lib import util_crypto_aes256_encrypt
from openblt.lib import util_crypto_aes256_decrypt


# ********************************* end of __init__.py **********************************
