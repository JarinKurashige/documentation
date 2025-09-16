/************************************************************************************//**
* \file         canusb.h
* \brief        Lawicel CANUSB interface header file.
* \ingroup      Lawicel_CanUsb
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. It is released under a commercial license. Refer to
* the license.pdf file for the exact licensing terms. It should be located in
* '.\Doc\license.pdf'. Contact Feaser if you did not receive the license.pdf file.
* 
* In a nutshell, you are allowed to modify and use this software in your closed source
* proprietary application, provided that the following conditions are met:
* 1. The software is not released or distributed in source (human-readable) form.
* 2. These licensing terms and the copyright notice above are not changed.
* 
* This software has been carefully tested, but is not guaranteed for any particular
* purpose. Feaser does not offer any warranties and does not guarantee the accuracy,
* adequacy, or completeness of the software and is not responsible for any errors or
* omissions or the results obtained from use of the software.
*
* \endinternal
****************************************************************************************/
/************************************************************************************//**
* \defgroup   Lawicel_CanUsb Lawicel CANUSB interface
* \brief      This module implements the CAN interface for the Lawicel CANUSB.
* \details    When using the Lawicel CANUSB interface, the 32-bit driver for the CANUSB 
*             DLL API should be installed: 
*             http://www.can232.com/download/canusb_setup_win32_v_2_2.zip
* \ingroup    CanDriver
****************************************************************************************/
#ifndef CANUSB_H
#define CANUSB_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************
* Function prototypes
****************************************************************************************/
tCanInterface const * CanUsbGetInterface(void);

#ifdef __cplusplus
}
#endif

#endif /* CANUSB_H */
/*********************************** end of canusb.h ***********************************/
