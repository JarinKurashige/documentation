/************************************************************************************//**
* \file         main.c
* \brief        FwCombine program source file.
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
#include <stdlib.h>                         /* for standard library                    */
#include <stdio.h>                          /* Standard I/O functions.                 */
#include <string.h>                         /* for string library                      */
#include "openblt.h"                        /* OpenBLT host library.                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Program return code indicating that the program executed successfully. */
#define RESULT_OK                           (0)

/** \brief Program return code indicating that an error was detected when processing the
 *         command line parameters/
 */
#define RESULT_ERROR_COMMANDLINE            (1)

/** \brief Program return code indicating that an error was detected while loading
 *         firmware data from a firmware file.
 */
#define RESULT_ERROR_FIRMWARE_LOAD          (2)

/** \brief Program return code indicating that an error was detected while saving
 *         firmware data to a firmware file.
 */
#define RESULT_ERROR_FIRMWARE_SAVE          (3)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void DisplayProgramInfo(void);
static void DisplayProgramUsage(void);
static bool ParseCommandLine(int argc, char const * const argv[]);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The filename of the first firmware file as specified on the command line. */
static char const * input1FileStr;

/** \brief The filename of the second firmware file as specified on the command line. */
static char const * input2FileStr;

/** \brief The filename of the output firmware file as specified on the command line. */
static char const * outputFileStr;


/************************************************************************************//**
** \brief     This is the program entry point.
** \param     argc Number of program arguments.
** \param     argv Array with program arguments.
** \return    Program return code. 0 for success, error code otherwise.
**
****************************************************************************************/
int main(int argc, char const * const argv[])
{
  int result = RESULT_OK;

  /* -------------------- Initialization --------------------------------------------- */
  /* Initialize the firmware data module using the S-record parser. */
  BltFirmwareInit(BLT_FIRMWARE_PARSER_SRECORD);

  /* -------------------- Display info ----------------------------------------------- */
  /* Check that at least enough command line arguments were specified. The first one is
   * always the name of the executable. Additionally, two firmware file must be 
   * specified, followed by the output file.
   */
  if (!ParseCommandLine(argc, argv))
  {
    /* Display program info */
    DisplayProgramInfo();
    /* Display program usage. */
    DisplayProgramUsage();
    /* Display error. */
    printf("[ERROR] Incorrect command line parameters specified.\n");
    /* Set error code. */
    result = RESULT_ERROR_COMMANDLINE;
  }

  /* -------------------- Firmware loading ------------------------------------------- */
  /* Load the first input file. */
  if (result == RESULT_OK)
  {
    if (BltFirmwareLoadFromFile(input1FileStr, 0) != BLT_RESULT_OK)
    {
      /* Display error. */
      printf("[ERROR] Could not load the first input file.\n");
      /* Set error code. */
      result = RESULT_ERROR_FIRMWARE_LOAD;
    }
  }
  /* Load the second input file. */
  if (result == RESULT_OK)
  {
    if (BltFirmwareLoadFromFile(input2FileStr, 0) != BLT_RESULT_OK)
    {
      /* Display error. */
      printf("[ERROR] Could not load the second input file.\n");
      /* Set error code. */
      result = RESULT_ERROR_FIRMWARE_LOAD;
    }
  }

  /* -------------------- Firmware saving -------------------------------------------- */
  /* Save the currently loaded firmware data of both input files into a new output
   * file.
   */
  if (result == RESULT_OK)
  {
    if (BltFirmwareSaveToFile(outputFileStr) != BLT_RESULT_OK)
    {
      /* Display error. */
      printf("[ERROR] Could not save output file.\n");
      /* Set error code. */
      result = RESULT_ERROR_FIRMWARE_SAVE;
    }
  }
  /* -------------------- Clean-up --------------------------------------------------- */
  /* Terminate the firmware data module. */
  BltFirmwareTerminate();
  
  /* Give result back. */
  return result;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Outputs information to the user about this program.
**
****************************************************************************************/
static void DisplayProgramInfo(void)
{
  printf("--------------------------------------------------------------------------\n");
  printf("FwCombine version 1.4.2 Combines two S-record files into one.\n");
  printf("Copyright (c) 2017 by Feaser  http://www.feaser.com\n");
  printf("-------------------------------------------------------------------------\n");
} /*** end of DisplayProgramInfo ***/


/************************************************************************************//**
** \brief     Outputs information to the user about how to use this program.
**
****************************************************************************************/
static void DisplayProgramUsage(void)
{
  printf("Usage:    FwCombine [input file 1] [input file 2] [output file]\n");
  printf("\n");
  printf("Example:  FwCombine bootloader.srec userprog.srec combined.srec\n");
  printf("\n");
  printf("-------------------------------------------------------------------------\n");
} /*** end of DisplayProgramUsage ***/


/************************************************************************************//**
** \brief     Parses the command line to validate and extract the command line
**            parameters.
** \param     argc Number of program arguments.
** \param     argv Array with program parameter strings.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool ParseCommandLine(int argc, char const * const argv[])
{
  bool result = false;

  /* Check parameters. */
  assert(argv != NULL);

  /* Reset the command line parameter storage variables. */
  input1FileStr = NULL;
  input2FileStr = NULL;
  outputFileStr = NULL;

  /* Only continue if parameters are valid. Note that 4 command line parameters are
   * expected. The first one if the name of this program's executable. After this the
   * two input files are specified, followed by the output filename.
   */
  if ( (argv != NULL) && (argc == 4) ) /*lint !e774 */
  {
    /* Extract the first input file. It should not start with '-', as this is typically
     * the start of a command line option. Additionally it must have a plausible
     * length.
     */
    if ( (argv[1][0] != '-') && (strlen(argv[1]) > 2) )
    {
      /* Store the pointer to the filename. */
      input1FileStr = argv[1];
    }
    /* Extract the second input file. It should not start with '-', as this is typically
     * the start of a command line option. Additionally it must have a plausible
     * length.
     */
    if ( (argv[2][0] != '-') && (strlen(argv[2]) > 2) )
    {
      /* Store the pointer to the filename. */
      input2FileStr = argv[2];
    }
    /* Extract the output file. It should not start with '-', as this is typically
     * the start of a command line option. Additionally it must have a plausible
     * length.
     */
    if ( (argv[3][0] != '-') && (strlen(argv[3]) > 2) )
    {
      /* Store the pointer to the filename. */
      outputFileStr = argv[3];
    }
    /* Validate the result. */
    if ( (input1FileStr != NULL) && (input2FileStr != NULL) && (outputFileStr != NULL) )
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of ParseCommandLine ***/


/*********************************** end of main.c *************************************/


