/************************************************************************************//**
* \file         xldriver.h
* \brief        Vector XL driver interface header file.
* \ingroup      XLDRIVER
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
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
* \defgroup   XLDRIVER Vector XL Driver USB to CAN interface
* \brief      This module implements the CAN interface for the Vector XL Driver.
* \ingroup    CanDriver
****************************************************************************************/
#ifndef XLDRIVER_H
#define XLDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************
* Function prototypes
****************************************************************************************/
tCanInterface const * VectorXlGetInterface(void);

#ifdef __cplusplus
}
#endif

#endif /* XLDRIVER_H */
/*********************************** end of xldriver.h *********************************/
