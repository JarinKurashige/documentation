/************************************************************************************//**
* \file         netaccess.h
* \brief        TCP/IP network access header file.
* \ingroup      NetAccess
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
* \defgroup   NetAccess TCP/IP Network Access
* \brief      This module implements a generic TCP/IP network access client driver.
* \ingroup    Session
****************************************************************************************/
#ifndef NETACCESS_H
#define NETACCESS_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Function prototypes
****************************************************************************************/
void NetAccessInit(void);
void NetAccessTerminate(void);
bool NetAccessConnect(char const * address, uint16_t port);
void NetAccessDisconnect(void);
bool NetAccessSend(uint8_t const * data, uint32_t length);
bool NetAccessReceive(uint8_t * data, uint32_t * length, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* NETACCESS_H */
/********************************* end of netaccess.h **********************************/

