/************************************************************************************//**
* \file         port/windows/critutil.c
* \brief        Critical section utility source file.
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

/****************************************************************************************
* Include files
****************************************************************************************/
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stdbool.h>                        /* for boolean type                        */
#include <windows.h>                        /* for windows library                     */
#include "util.h"                           /* Utility module                          */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Flag to determine if the critical section object was already initialized. */
static volatile bool criticalSectionInitialized = false;

/** \brief Critical section object. */
static CRITICAL_SECTION criticalSection;


/************************************************************************************//**
** \brief     Initializes the critical section module. Should be called before the
**            Enter/Exit functions are used. It is okay to call this initialization
**            multiple times from different modules.
**
****************************************************************************************/
void UtilCriticalSectionInit(void)
{
  /* Only initialize if not yet done so previously. */
  if (!criticalSectionInitialized)
  {
    /* Initialize the critical section object. */
    InitializeCriticalSection((CRITICAL_SECTION *)&criticalSection);
    /* Set initialized flag. */
    criticalSectionInitialized = true;
  }
} /*** end of UtilCriticalSectionInit ***/


/************************************************************************************//**
** \brief     Terminates the critical section module. Should be called once critical
**            sections are no longer needed. Typically called from another module's
**            termination function that also initialized it. It is okay to call this 
**            termination multiple times from different modules.
**
****************************************************************************************/
void UtilCriticalSectionTerminate(void)
{
  /* Only terminate if it was initialized. */
  if (criticalSectionInitialized)
  {
    /* Reset the initialized flag. */
    criticalSectionInitialized = false;
    /* Delete the critical section object. */
    DeleteCriticalSection((CRITICAL_SECTION *)&criticalSection);
  }
} /*** end of UtilCriticalSectionTerminate ***/



/************************************************************************************//**
** \brief     Enters a critical section. The functions UtilCriticalSectionEnter and 
**            UtilCriticalSectionExit should always be used in a pair.
**
****************************************************************************************/
void UtilCriticalSectionEnter(void)
{
  /* Check initialization. */
  assert(criticalSectionInitialized);

  /* Only continue if actually initialized. */
  if (criticalSectionInitialized)
  {
    EnterCriticalSection((CRITICAL_SECTION *)&criticalSection);
  }
} /*** end of UtilCriticalSectionEnter ***/


/************************************************************************************//**
** \brief     Leaves a critical section. The functions UtilCriticalSectionEnter and 
**            UtilCriticalSectionExit should always be used in a pair.
**
****************************************************************************************/
void UtilCriticalSectionExit(void)
{
  /* Check initialization. */
  assert(criticalSectionInitialized);

  /* Only continue if actually initialized. */
  if (criticalSectionInitialized)
  {
    LeaveCriticalSection((CRITICAL_SECTION *)&criticalSection);
  }
} /*** end of UtilCriticalSectionExit ***/


/*********************************** end of critutil.c *********************************/

