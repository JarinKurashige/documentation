/************************************************************************************//**
* \file         main.c
* \brief        CsAddTable program source file.
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

/** \brief Program return code indicating that an error was detected during a memory
 *         allocation operation.
 */
#define RESULT_ERROR_MEMORY_ALLOC           (4)

/** \brief Program return code indicating that an error was detected during the creation
 *         of the checksum info table.
 */
#define RESULT_ERROR_CHECKSUM_TABLE         (5)

/** \brief Program return code indicating that an error was detected while
 *         adding firmware data to the firmware module.
 */
#define RESULT_ERROR_FIRMWARE_ADD           (6)

/** \brief Program return code indicating that an error was detected while excluding
 *         firmware data from the checksum info table.
 */
#define RESULT_ERROR_DATA_EXCLUSION         (7)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void DisplayProgramInfo(void);
static void DisplayProgramUsage(void);
static bool ParseCommandLine(int argc, char const * const argv[]);
static uint32_t GenerateChecksumInfoTable(uint8_t * const tableData);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Base address of where the checksum info table will be added to the firmware
 *         data.
 */
static uint32_t tableBaseAddr;

/** \brief Flag to control what is stored in the output file. False means that the
 *         firmware data of the input file together with the generated checksum info
 *         table is written. True means that only the generated checksum info table is
 *         written.
 */
static bool outputTableOnly;

/** \brief Flag to indicate if a specific region should be excluded when generating the
 *         checksum info table. The start address of the region to exclude is stored in
 *         excludeRegionBaseAddr and the size of the region is stored in
 *         excludeRegionLen.
 */
static bool excludeRegion;

/** \brief The start address of the region to exclude. */
static uint32_t excludeRegionBaseAddr;

/** \brief The size in bytes of the region to exclude. */
static uint32_t excludeRegionLen;

/** \brief The filename of the input firmware file as specified on the command line. */
static char const * inputFileStr;

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
  /* Load the firmware file. */
  if (result == RESULT_OK)
  {
    if (BltFirmwareLoadFromFile(inputFileStr, 0) != BLT_RESULT_OK)
    {
      /* Display error. */
      printf("[ERROR] Could not load the input file.\n");
      /* Set error code. */
      result = RESULT_ERROR_FIRMWARE_LOAD;
    }
  }

  /* -------------------- Remove excluded region ------------------------------------- */
  if (result == RESULT_OK)
  {
    /* Should a data region be excluded from the checksum info table? */
    if (excludeRegion)
    {
      /* Remove the region from the firmware data. If will be added again before the
       * checksum info table gets patched in later on.
       */
      if (BltFirmwareRemoveData(excludeRegionBaseAddr, excludeRegionLen) != BLT_RESULT_OK)
      {
        /* Display error. */
        printf("[ERROR] Could not remove data that is to be excluded.\n");
        /* Set error code. */
        result = RESULT_ERROR_DATA_EXCLUSION;
      }
    }
  }

  /* -------------------- Checksum info table creation ------------------------------- */
  if (result == RESULT_OK)
  {
    /* Determine the length of the checksum info table in bytes. The table has a 10 byte
     * table header and then an additional 10 bytes for each segment of firmware data.
     */
    uint32_t tableSize;
    /* Made static to resolve possible custodial pointer issues on auto (stack)
     * variables.
     */
    static uint8_t * tableDataPtr = NULL;

    /* Determine the length of the checksum info table in bytes. The table has a 10 byte
     * table header and then an additional 10 bytes for each segment of firmware data.
     */
    tableSize = 10u + (BltFirmwareGetSegmentCount() * 10u);
    /* Allocate memory for the checksum info table. */
    tableDataPtr = malloc(tableSize);

    /* Verify the memory allocation of the table data. */
    assert(tableDataPtr != NULL);

    /* Only continue if the memory allocation of the table data was successful. */
    if (tableDataPtr == NULL) /*lint !e774 */
    {
      /* Display error. */
      printf("[ERROR] Could not allocate table data.\n");
      /* Set error code. */
      result = RESULT_ERROR_MEMORY_ALLOC;
    }
    /* Memory allocation was successful. */
    else
    {
      /* Perform checksum info table generation and check that the generated size is
       * as expected.
       */
      if (GenerateChecksumInfoTable(tableDataPtr) != tableSize)
      {
        /* Display error. */
        printf("[ERROR] Could not generate checksum info table.\n");
        /* Set error code. */
        result = RESULT_ERROR_CHECKSUM_TABLE;
      }
      /* Checksum info table successfully generated. */
      else
      {
        /* Should the output file only contain the checksum info table? */
        if (outputTableOnly)
        {
          /* Clear the firmware data that is currently stored in the firmware module. */
          BltFirmwareClearData();
        }
        else
        {
          /* Was data possibly excluded and therefore removed from the firmware data? */
          if (excludeRegion)
          {
            /* Reload the original firmware data to restore it. */
            BltFirmwareClearData();
            if (BltFirmwareLoadFromFile(inputFileStr, 0) != BLT_RESULT_OK)
            {
              /* Display error. */
              printf("[ERROR] Could not load the input file.\n");
              /* Set error code. */
              result = RESULT_ERROR_FIRMWARE_LOAD;
            }
          }
        }
        /* Add the checksum info table to the firmware data. */
        if (result == RESULT_OK)
        {
          if (BltFirmwareAddData(tableBaseAddr, tableSize, tableDataPtr) != BLT_RESULT_OK)
          {
            /* Display error. */
            printf("[ERROR] Could not add the checksum info table data.\n");
            /* Set error code. */
            result = RESULT_ERROR_FIRMWARE_ADD;
          }
        }
      }
      /* Release the allocated memory. */
      free(tableDataPtr);
    }
  }

  /* -------------------- Firmware saving -------------------------------------------- */
  /* Save the currently loaded firmware data. Depending on the specified '-t' command
   * line parameter, this is either just the checksum info table or both the checksum
   * info table and the original firmware data from the input file.
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
  printf("CsAddTable version 1.4.2. Adds a checksum info table to an S-record file.\n");
  printf("Copyright (c) 2017 by Feaser  http://www.feaser.com\n");
  printf("-------------------------------------------------------------------------\n");
} /*** end of DisplayProgramInfo ***/


/************************************************************************************//**
** \brief     Outputs information to the user about how to use this program.
**
****************************************************************************************/
static void DisplayProgramUsage(void)
{
  printf("Usage:    CsAddTable [options] [input file] [output file]\n");
  printf("\n");
  printf("Example:  CsAddTable -a0800C000 demoprog.srec demoprog_checksum.srec\n");
  printf("\n");
  printf("Available options:\n");
  printf("  -a[value]         Specifies the base address of where the checksum info\n");
  printf("                    table will be added. Specify the value as a 32-bit\n");
  printf("                    hexadecimal value.\n");
  printf("  -t                Only output the checksum info table into the output\n");
  printf("                    file.\n");
  printf("  -x[value]:[value] Specifies an address range that is to be excluded.\n");
  printf("                    The first value is the start address and the second\n");
  printf("                    value is the end address. Specify both addresses as\n");
  printf("                    32-bit hexadecimal values e.g. -x08000000:08000FFF.\n");
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
  uint8_t paramIdx;

  /* Check parameters. */
  assert(argv != NULL);

  /* Reset the command line parameter storage variables. */
  tableBaseAddr = 0xffffffff;
  outputTableOnly = false;
  excludeRegion = false;
  excludeRegionBaseAddr = 0x00000000;
  excludeRegionLen = 0;
  inputFileStr = NULL;
  outputFileStr = NULL;

  /* Only continue if parameters are valid. Note that 4 -5 command line parameters are
   * expected. The first one if the name of this program's executable. After this the
   * address offset is expected, followed by the  input and output firmware files. The
   * '-t' option for outputting only the checksum info table is optional
   */
  if ( (argv != NULL) && ( (argc >= 4) && (argc <= 6) ) ) /*lint !e774 */
  {
    /* Extract the input file. It is the second to last command line parameter. It should
     * not start with '-', as this is typically the start of a command line option.
     * Additionally it must have a plausible length.
     */
    if ( (argv[argc-2][0] != '-') && (strlen(argv[argc-2]) > 2) )
    {
      /* Store the pointer to the filename. */
      inputFileStr = argv[argc-2];
    }
    /* Extract the output file. It is the last command line parameter. It should not
     * start with '-', as this is typically the start of a command line option.
     * Additionally it must have a plausible length.
     */
    if ( (argv[argc-1][0] != '-') && (strlen(argv[argc-1]) > 2) )
    {
      /* Store the pointer to the filename. */
      outputFileStr = argv[argc-1];
    }

    /* Loop through the remaining command line parameters to extract the options starting
     * with a '-'. The first parameter can be skipped as this is the program name. The
     * last two parameters can also be skipped as these are the input and output
     * filename, which were already extracted previously.
     */
    for (paramIdx = 1; paramIdx < (argc - 2); paramIdx++)
    {
      /* Is this the -t parameter? */
      if (strcmp(argv[paramIdx], "-t") == 0)
      {
        /* Set the flag. */
        outputTableOnly = true;
        /* Continue with next loop iteration. */
        continue;
      }
      /* Is this the -a[value] parameter? */
      if ( (strstr(argv[paramIdx], "-a") != NULL) &&
           (strlen(argv[paramIdx]) > 2) )
      {
        /* Extract the hexadecimal base address for the checksum info table. */
        sscanf(&argv[paramIdx][2], "%x", &tableBaseAddr);
        /* Continue with next loop iteration. */
        continue;
      }
      /* Is this the -x[value]:[value] parameter? */
      if ( (strstr(argv[paramIdx], "-x") != NULL) &&
           (strlen(argv[paramIdx]) >= 5) )
      {
        uint32_t startAddr = 0;
        uint32_t endAddr = 0;
        /* Extract the hexadecimal start and end addresses for the region to exclude. */
        sscanf(&argv[paramIdx][2], "%x:%x", &startAddr, &endAddr);
        /* Validate the addresses. */
        if (startAddr < endAddr)
        {
          /* Store the information about the region to exclude. */
          excludeRegion = true;
          excludeRegionBaseAddr = startAddr;
          excludeRegionLen = (endAddr - startAddr) + 1;
        }
        /* Continue with next loop iteration. */
        continue;
      }
    }
    /* Validate the result. */
    if ( (tableBaseAddr != 0xffffffff) && (inputFileStr != NULL) &&
         (outputFileStr != NULL) )
    {
      result = true;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of ParseCommandLine ***/


/************************************************************************************//**
** \brief     Generates the checksum info table contents based on the currently loaded
**            firmware data in the firmware module. The resulting table is stored in the
**            specified tableData buffer.
** \param     tableData Byte buffer where the resulting checksum info table is stored. It
**            must be large enough to hold the entire checksum info table.
** \return    Number of bytes of the checksum info table that were stored in tableData,
**            if successful, 0 otherwise.
**
****************************************************************************************/
static uint32_t GenerateChecksumInfoTable(uint8_t * const tableData)
{
  uint32_t result = 0;
  uint32_t segmentIdx;
  uint8_t * dataPtr;
  uint16_t crc16Value;
  uint32_t segmentAddress;
  uint32_t segmentSize;
  uint8_t const * segmentData;
  /* Constant checksum table identifier. This value is stored in the first four bytes
   * of the checksum info table.
   */
  uint32_t const checksumTableId = 0x23AA5537ul;

  /* Check parameters. */
  assert(tableData != NULL);

  /* Only continue with valid parameters. */
  if (tableData != NULL) /*lint !e774 */
  {
    /* Initialize the data pointer to right after the header row. This row will be added
     * after adding the rows for each segment of firmware data.
     */
    dataPtr = &tableData[0] + 10u;
    /* Loop over all segments with firmware data. */
    for (segmentIdx = 0; segmentIdx < BltFirmwareGetSegmentCount(); segmentIdx++)
    {
      /* Obtain information about the segment and access to its firmware data. */
      segmentData = BltFirmwareGetSegment(segmentIdx, &segmentAddress, &segmentSize);
      /* Validate the obtained segment information. */
      if ( (segmentData == NULL) || (segmentSize == 0) )
      {
        /* This should not happen so flag it as an error. */
        result = 0;
        break;
      }
      /* Segment information is assumed valid. */
      else
      {
        /* Calculate the checksum over the segment's firmware data. */
        crc16Value = BltUtilCrc16Calculate(segmentData, segmentSize);
        /* Add the segment's base address (LSB first). */
        *dataPtr = (uint8_t)segmentAddress;
        dataPtr++;
        *dataPtr = (uint8_t)(segmentAddress >> 8);
        dataPtr++;
        *dataPtr = (uint8_t)(segmentAddress >> 16);
        dataPtr++;
        *dataPtr = (uint8_t)(segmentAddress >> 24);
        dataPtr++;
        /* Add the segment's length (LSB first). */
        *dataPtr = (uint8_t)segmentSize;
        dataPtr++;
        *dataPtr = (uint8_t)(segmentSize >> 8);
        dataPtr++;
        *dataPtr = (uint8_t)(segmentSize >> 16);
        dataPtr++;
        *dataPtr = (uint8_t)(segmentSize >> 24);
        dataPtr++;
        /* Add the segment's checksum value (LSB first). */
        *dataPtr = (uint8_t)crc16Value;
        dataPtr++;
        *dataPtr = (uint8_t)(crc16Value >> 8);
        dataPtr++;
        /* Update the result value, which holds the number of bytes that were added. */
        result += 10;
      }
    }
    /* Check if the number of added bytes to the checksum info table matches the
     * expected. Expected is that a row consisting of 10 bytes was added for each
     * segment with firmware data.
     */
    if (result != (BltFirmwareGetSegmentCount() * 10u))
    {
      /* Flag error. */
      result = 0;
    }
    /* All good so far. Continue by adding the header row to the table. */
    else
    {
      /* Initialize the data pointer to right after the header row. It now points to the
       * first row, which is needed to perform the checksum calculation over the rows
       * itself.
       */
      dataPtr = &tableData[0] + 10u;
      /* Perform checksum calculation over the rows. */
      crc16Value = BltUtilCrc16Calculate(dataPtr, result);
      /* Initialize the data pointer to the start, which is where the header row should
       * now be added.
       */
      dataPtr = &tableData[0];
      /* Add the constant table identifier (LSB first). */
      *dataPtr = (uint8_t)checksumTableId;
      dataPtr++;
      *dataPtr = (uint8_t)(checksumTableId >> 8);
      dataPtr++;
      *dataPtr = (uint8_t)(checksumTableId >> 16);
      dataPtr++;
      *dataPtr = (uint8_t)(checksumTableId >> 24);
      dataPtr++;
      /* Next add the number of segments as a byte and add the three reserved bytes. */
      *dataPtr = (uint8_t)BltFirmwareGetSegmentCount();
      dataPtr++;
      *dataPtr = 0u;
      dataPtr++;
      *dataPtr = 0u;
      dataPtr++;
      *dataPtr = 0u;
      dataPtr++;
      /* Add the table's checksum value (LSB first). */
      *dataPtr = (uint8_t)crc16Value;
      dataPtr++;
      *dataPtr = (uint8_t)(crc16Value >> 8);
      /* Update the result value, which holds the number of bytes that were added. */
      result += 10;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of GenerateChecksumInfoTable ***/


/*********************************** end of main.c *************************************/


