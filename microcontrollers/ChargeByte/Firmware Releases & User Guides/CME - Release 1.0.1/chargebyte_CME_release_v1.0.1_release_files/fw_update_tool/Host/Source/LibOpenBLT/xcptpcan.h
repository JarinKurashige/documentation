/************************************************************************************//**
* \file         xcptpcan.h
* \brief        XCP CAN transport layer header file.
* \ingroup      XcpTpCan
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
* \defgroup   XcpTpCan XCP CAN transport layer
* \brief      This module implements the XCP transport layer for CAN.
* \ingroup    XcpLoader
****************************************************************************************/
#ifndef XCPTPCAN_H
#define XCPTPCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of structure with settings specific to the XCP transport layer module
 *         for CAN.
 */
typedef struct t_xcp_tp_can_settings
{
  char const * device;           /**< Device name such as can0, peak_pcanusb, etc.     */
  uint32_t channel;              /**< Channel on the device to use.                    */
  uint32_t baudrate;             /**< Communication speed in bits/sec.                 */
  uint32_t transmitId;           /**< Transmit CAN identifier.                         */
  uint32_t receiveId;            /**< Receive CAN identifier.                          */
  bool useExtended;              /**< Boolean to configure 29-bit CAN identifiers.     */
} tXcpTpCanSettings;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
tXcpTransport const * XcpTpCanGetTransport(void);

#ifdef __cplusplus
}
#endif

#endif /* XCPTPCAN_H */
/*********************************** end of xcptpcan.h *********************************/
