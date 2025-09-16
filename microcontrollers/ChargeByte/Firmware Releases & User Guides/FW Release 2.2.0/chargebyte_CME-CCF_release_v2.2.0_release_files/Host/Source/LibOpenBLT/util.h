/************************************************************************************//**
* \file         util.h
* \brief        Utility module header file.
* \ingroup      Utility
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
* \defgroup   Utility Generic Utilities
* \brief      Generic utility functions and definitions.
* \ingroup    Library
* \details
* The Utility module contains generic functions and definitions that can be handy for
* use internally in the library and also externally by another application that makes
* use of the library.
****************************************************************************************/
#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Function prototypes
****************************************************************************************/
uint16_t UtilChecksumCrc16Calculate(uint8_t const * data, uint32_t len);
uint32_t UtilChecksumCrc32Calculate(uint8_t const * data, uint32_t len);
bool UtilFileExtractFilename(char const * fullFilename, char * filenameBuffer);
uint32_t UtilTimeGetSystemTimeMs(void);
void UtilTimeDelayMs(uint16_t delay);
void UtilCriticalSectionInit(void);
void UtilCriticalSectionTerminate(void);
void UtilCriticalSectionEnter(void);
void UtilCriticalSectionExit(void);
bool UtilCryptoAes256Encrypt(uint8_t * data, uint32_t len, uint8_t const * key);
bool UtilCryptoAes256Decrypt(uint8_t * data, uint32_t len, uint8_t const * key);


#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */
/********************************* end of util.h ***************************************/

 
 
