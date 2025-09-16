/************************************************************************************//**
* \file         seednkey.h
* \brief        XCP Seed and Key shared library header file.
* \ingroup      SeedNKey
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
* \defgroup   SeedNKey XCP Seed/Key
* \brief      XCP Seed and Key shared library.
* \details
* This shared library implements an example XCP Seed and Key protection algorithm. If the
* OpenBLT bootloader on the microcontroller is configured to support this protection,
* this shared library file must be configured in the firmware update tool on the host
* (for example MicroBoot or BootCommander). The OpenBLT bootloader will reject new
* firmware update requests, if an incorrect XCP Seed and Key protection algorithm is
* specified. 
*
* You are free and even encouraged to change the protection algorithm in this shared
* library to however you see fit to protect your target from unwanted firmware updates.
****************************************************************************************/
#ifndef SEEDNKEY_H
#define SEEDNKEY_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdint.h>                         /* for standard integer types              */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* CMake automatically defines macro seednkey_shared_EXPORTS when building the shared
 * version of the library. When building under windows, this is used to set the export
 * macro, which is needed to confige the library functions.
 */
#if defined(_WIN32) || defined(_WIN64)
#if defined(seednkey_shared_EXPORTS)
#define  LIBOPENBLT_EXPORT __declspec(dllexport)
#else
#define  LIBOPENBLT_EXPORT 
#endif /* seednkey_shared_EXPORTS */
#else /* defined(_WIN32) || defined(_WIN64) */
#define LIBOPENBLT_EXPORT
#endif

/* Result values. */
/** \brief Result value in case of success. */
#define XCP_RESULT_OK                (0u)
/** \brief Result value in case of error. */
#define XCP_RESULT_ERROR             (1u)


/* XCP supported resources. */
/** \brief XCP ProGraMing resource. */
#define XCP_RESOURCE_PGM             (0x10u)
/** \brief XCP data STIMulation resource. */
#define XCP_RESOURCE_STIM            (0x08u)
/** \brief XCP Data AcQuisition resource. */
#define XCP_RESOURCE_DAQ             (0x04u)
/** \brief XCP CALibration and PAGing resource. */
#define XCP_RESOURCE_CALPAG          (0x01u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
LIBOPENBLT_EXPORT uint32_t XCP_ComputeKeyFromSeed(uint8_t resource, uint8_t seedLen, 
                                                  uint8_t const * seedPtr, 
                                                  uint8_t * keyLenPtr,
                                                  uint8_t * keyPtr);
LIBOPENBLT_EXPORT uint32_t XCP_GetAvailablePrivileges(uint8_t * resourcePtr);


#ifdef __cplusplus
}
#endif

#endif /* SEEDNKEY_H */
/********************************* end of seednkey.h ***********************************/

 
