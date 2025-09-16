/************************************************************************************//**
* \file         xcpprotect.h
* \brief        XCP Protection module header file.
* \ingroup      XcpLoader
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
#ifndef XCPPROTECT_H
#define XCPPROTECT_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* XCP supported resources. */
#define XCPPROTECT_RESOURCE_PGM        (0x10u)   /**< ProGraMing resource.             */
#define XCPPROTECT_RESOURCE_STIM       (0x08u)   /**< data STIMulation resource.       */
#define XCPPROTECT_RESOURCE_DAQ        (0x04u)   /**< Data AcQuisition resource.       */
#define XCPPROTECT_RESOURCE_CALPAG     (0x01u)   /**< CALibration and PAGing resource. */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void XcpProtectInit(char const * seedKeyFile);
void XcpProtectTerminate(void);
bool XCPProtectComputeKeyFromSeed(uint8_t resource, uint8_t seedLen,
                                  uint8_t const * seedPtr, uint8_t * keyLenPtr,
                                  uint8_t * keyPtr);
bool XcpProtectGetPrivileges(uint8_t * resourcePtr);


#ifdef __cplusplus
}
#endif

#endif /* XCPPROTECT_H */
/*********************************** end of xcpprotect.h *******************************/
