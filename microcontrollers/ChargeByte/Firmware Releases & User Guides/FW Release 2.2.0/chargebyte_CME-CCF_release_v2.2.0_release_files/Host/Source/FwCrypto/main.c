/************************************************************************************//**
* \file         main.c
* \brief        FwCrypto program source file.
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

/** \brief Program return code indicating that an error was detected while extracting the
 *         cryptography key from the key file.
 */
#define RESULT_ERROR_INVALID_KEY            (3)

/** \brief Program return code indicating that an error was detected while aligning the
 *         firmware data.
 */
#define RESULT_ERROR_DATA_ALIGNMENT         (4)

/** \brief Program return code indicating that an error was detected while performing
 *         a cryptographic operation on the firmware data.
 */
#define RESULT_ERROR_CRYPTO_OPERATION       (5)

/** \brief Program return code indicating that an error was detected while saving
 *         firmware data to a firmware file.
 */
#define RESULT_ERROR_FIRMWARE_SAVE          (6)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void DisplayProgramInfo(void);
static void DisplayProgramUsage(void);
static bool ParseCommandLine(int argc, char const * const argv[]);
static bool ExtractCryptoKeyFromFile(char const * cryptoKeyFile, uint8_t * cryptoKey);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The filename of the text file with the cryptography key. */
static char const * cryptoKeyFileStr;

/** \brief Boolean flag to control the cryptography operation to perform on the program
 *         data. True for encryption, false for decryption.
 */
static bool encryptOperation;

/** \brief Flag to indicate if a specific region should be excluded when performing the
 *         crypto operation. The start address of the region to exclude is stored in
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
  static uint8_t cryptoKey[32] = { 0 };

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
    if (BltFirmwareLoadFromFile(inputFileStr, 0) != BLT_RESULT_OK)
    {
      /* Display error. */
      printf("[ERROR] Could not load the input file.\n");
      /* Set error code. */
      result = RESULT_ERROR_FIRMWARE_LOAD;
    }
    /* Check to make sure that data was actually present, in which case at least one
     * firmware data segment should be there.
     */
    else
    {
      if (BltFirmwareGetSegmentCount() == 0)
      {
        /* Set error code. */
        result = RESULT_ERROR_FIRMWARE_LOAD;
      }
    }
  }

  /* -------------------- Load cryptography key -------------------------------------- */
  /* Extract key from the specified file. */
  if (result == RESULT_OK)
  {
    /* Attempt to extract the key from the file. */
    if (!ExtractCryptoKeyFromFile(cryptoKeyFileStr, cryptoKey))
    {
      /* Set error code. */
      result = RESULT_ERROR_INVALID_KEY;
    }
  }

  /* -------------------- Perform firmware data alignment ---------------------------- */
  /* Decryption is performed in the OpenBLT bootloader's flash driver. The flash driver
   * programs blocks in chunks of 256 bytes (aligned) or a multiple thereof. AES ECB
   * cryptography works on blocks of 16 bytes. It is therefore important that the
   * firmware data segment's base addresses are 16 byte aligned and that the segment
   * size is a multiple of 16 bytes. Start by checking the base address alignment and
   * add filler data where necessary.
   */
  if (result == RESULT_OK)
  {
    uint32_t segmentCount = BltFirmwareGetSegmentCount();
    for (uint32_t segmentIdx = 0; segmentIdx < segmentCount; )
    {
      /* Obtain segment info. */
      uint32_t segmentAddress;
      uint32_t segmentLen;
      uint32_t currentSegment;
      if (BltFirmwareGetSegment(segmentIdx, &segmentAddress, &segmentLen) == NULL)
      {
        /* Set error code. */
        result = RESULT_ERROR_DATA_ALIGNMENT;
        /* Stop looping because an error was detected. */
        break;
      }
      /* Store current segment so it can still be used after the indexer got
       * incremented.
       */
      currentSegment = segmentIdx;
      /* Increment the segment indexer to continue with the next segment at the next
       * loop iteration.
       */
      segmentIdx++;
      /* Only continue if is no error was detected. */
      if (result == RESULT_OK)
      {
        /* Determine aligned base address. */
        uint32_t alignedBaseAddress = segmentAddress & ~(0x0000000Fu);
        /* Determine number of filler bytes to add. */
        uint32_t numFillerBytes = segmentAddress - alignedBaseAddress;
        /* Check if there is a segment before this one. */
        if (currentSegment >= 1)
        {
          /* Obtain information about the previous segment. */
          uint32_t prevSegmentAddress;
          uint32_t prevSegmentLen;
          if (BltFirmwareGetSegment(currentSegment - 1, &prevSegmentAddress,
              &prevSegmentLen) != NULL)
          {
            /* Sanity check. */
            assert((prevSegmentAddress + prevSegmentLen) < segmentAddress);
            /* Calculate how many bytes are in the gap between the two segments. */
            uint32_t deltaBytes = segmentAddress - (prevSegmentAddress + prevSegmentLen);
            /* Make sure the number of filler bytes is not too big such that it would
             * overwrite data in the previous segment.
             */
            if (numFillerBytes > deltaBytes)
            {
              numFillerBytes = deltaBytes;
            }
          }
        }
        /* Sanity check. */
        assert(numFillerBytes < 16u);
        /* Prepend filler bytes at the start of the segment. */
        for (uint8_t idx = 1; idx <= numFillerBytes; idx++)
        {
          uint8_t fillerByteValue = 0xffu;
          if (BltFirmwareAddData(segmentAddress - idx, 1, &fillerByteValue) !=
                BLT_RESULT_OK)
          {
            /* Set error code. */
            result = RESULT_ERROR_DATA_ALIGNMENT;
            /* Stop looping because an error was detected. */
            break;
          }
        }
        /* Check if the length of the segment got changed. */
        if (numFillerBytes > 0)
        {
          /* Filler bytes were added to a segment, possibly now being adjacent to another
           * segment and be merged with it. This leads to the number of segments being
           * changed. Best to reinitialize the loop control variabled and restart the 
           * loop.
           */
          segmentIdx = 0;
          segmentCount = BltFirmwareGetSegmentCount();
          continue;
        }
      }
      /* Stop looping if an error was detected. */
      if (result != RESULT_OK)
      {
        break;
      }
    }
  }
  /* The segment data is now aligned to 16 byte addresses. Now make sure all segments
   * have a data length that is a multiple of 16 bytes. Add filler bytes where needed.
   */
  if (result == RESULT_OK)
  {
    uint32_t segmentCount = BltFirmwareGetSegmentCount();
    for (uint32_t segmentIdx = 0; segmentIdx < segmentCount; )
    {
      /* Obtain segment info. */
      uint32_t segmentAddress;
      uint32_t segmentLen;
      if (BltFirmwareGetSegment(segmentIdx, &segmentAddress, &segmentLen) == NULL)
      {
        /* Set error code. */
        result = RESULT_ERROR_DATA_ALIGNMENT;
        /* Stop looping because an error was detected. */
        break;
      }
      /* Increment the segment indexer to continue with the next segment at the next
       * loop iteration.
       */
      segmentIdx++;
      /* Only continue if is no error was detected. */
      if (result == RESULT_OK)
      {
        /* Determine number of filler bytes to add. */
        uint32_t numFillerBytes = 0;
        if ((segmentLen % 16u) != 0)
        {
          numFillerBytes = 16u - (segmentLen % 16u);
        }
        /* Note that in a previous step the base address of each segment was already
         * 16 byte aligned. This means that there is no need to check if the to be
         * added filler bytes overwrite data in the begin of the next segment. It this
         * would have been the case, then both segments got already merged into one when
         * the filler bytes were added during the base address alignment.
         */
        /* Sanity check. */
        assert(numFillerBytes < 16u);
        /* Append filler bytes at the end of the segment. */
        for (uint8_t idx = 0; idx < numFillerBytes; idx++)
        {
          uint8_t fillerByteValue = 0xffu;
          if (BltFirmwareAddData(segmentAddress + segmentLen + idx, 1, &fillerByteValue)
                != BLT_RESULT_OK)
          {
            /* Set error code. */
            result = RESULT_ERROR_DATA_ALIGNMENT;
            /* Stop looping because an error was detected. */
            break;
          }
        }
        /* Check if the length of the segment got changed. */
        if (numFillerBytes > 0)
        {
          /* Filler bytes were added to a segment, possibly now being adjacent to another
           * segment and be merged with it. This leads to the number of segments being
           * changed. Best to reinitialize the loop control variabled and restart the 
           * loop.
           */
          segmentIdx = 0;
          segmentCount = BltFirmwareGetSegmentCount();
          continue;
        }
      }
      /* Stop looping if an error was detected. */
      if (result != RESULT_OK)
      {
        break;
      }
    }
  }

  /* -------------------- Perform cryptography --------------------------------------- */
  if (result == RESULT_OK)
  {
    for (uint32_t segmentIdx = 0; segmentIdx < BltFirmwareGetSegmentCount(); segmentIdx++)
    {
      /* Obtain segment info. */
      uint32_t segmentAddress;
      uint32_t segmentLen;
      uint8_t * segmentData;
      segmentData = BltFirmwareGetSegment(segmentIdx, &segmentAddress, &segmentLen);
      if (segmentData == NULL)
      {
        /* Set error code. */
        result = RESULT_ERROR_CRYPTO_OPERATION;
        /* Stop looping because an error was detected. */
        break;
      }
      /* Only continue if is no error was detected. */
      if (result == RESULT_OK)
      {
        /* Perform cryptography operation in blocks of 16 bytes. Not really needed from
         * a LibOpenBLT API perspective, but this makes it easier to exclude a region.
         */
        uint32_t blockBaseAddress = segmentAddress;
        for (uint32_t i = 0; i < (segmentLen / 16u); i++)
        {
          bool blockExcluded = false;
          /* Was an exclusion region specified? */
          if (excludeRegion)
          {
            /* Is this block excluded? */
            if ( (blockBaseAddress >= excludeRegionBaseAddr) &&
                 ((blockBaseAddress + 16u) <= (excludeRegionBaseAddr + excludeRegionLen)) )
            {
              /* This block should be excluded. */
              blockExcluded = true;
            }
          }
          /* Only perform the cryptography operation if the block is not part of the
           * region that was requested to be excluded.
           */
          if (!blockExcluded)
          {
            /* Should the firmware data be encrypted? */
            if (encryptOperation)
            {
              if (BltUtilCryptoAes256Encrypt(&segmentData[i * 16u], 16, cryptoKey) !=
                    BLT_RESULT_OK)
              {
                /* Set error code. */
                result = RESULT_ERROR_CRYPTO_OPERATION;
                /* Stop looping because an error was detected. */
                break;
              }
            }
            /* Firmware data should be decrypted. */
            else
            {
              if (BltUtilCryptoAes256Decrypt(&segmentData[i * 16u], 16, cryptoKey) !=
                    BLT_RESULT_OK)
              {
                /* Set error code. */
                result = RESULT_ERROR_CRYPTO_OPERATION;
                /* Stop looping because an error was detected. */
                break;
              }
            }
          }
          /* Update the block base address for the next iteration. */
          blockBaseAddress += 16u;
        }
      }
    }
  }

  /* -------------------- Firmware saving -------------------------------------------- */
  /* Save the currently loaded and en/decrypted firmware data into a new output file.  */
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
  printf("FwCrypto version 1.3.0. Encrypts or decrypts the program data in an\n");
  printf("S-record file.\n");
  printf("Copyright (c) 2017 by Feaser  http://www.feaser.com\n");
  printf("-------------------------------------------------------------------------\n");
} /*** end of DisplayProgramInfo ***/


/************************************************************************************//**
** \brief     Outputs information to the user about how to use this program.
**
****************************************************************************************/
static void DisplayProgramUsage(void)
{
  printf("Usage:    FwCrypto [options] [input file] [output file]\n");
  printf("\n");
  printf("Example:  FwCrypto -en=1 -kf=cryptokey.txt userprog.srec encrypted.srec\n");
  printf("\n");
  printf("Available options:\n");
  printf("  -en=[value]        Configures the cryptography operation to perform as\n");
  printf("                     an 8-bit value. Set the value to 0 to perform decryp-\n");
  printf("                     tion of the program data, set the value to 1 to\n");
  printf("                     perform encryption (Default=1).\n");
  printf("  -kf=[file]         Text file that contains the 256-bit AES cryptography\n");
  printf("                     key (Mandatory). The key should be specified as a\n");
  printf("                     sequence of 64 hexadecimal characters. Example of the\n");
  printf("                     correct content format in the file (on one line):\n");
  printf("                     key=0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF\n");
  printf("                     0123456789ABCDEF\n");
  printf("  -x=[value]:[value] Specifies an address range that is to be excluded.\n");
  printf("                     The first value is the start address and the second\n");
  printf("                     value is the end address. Specify both addresses as\n");
  printf("                     32-bit hexadecimal values e.g. -x=08000000:08000FFF.\n");
  printf("                     Note that both the address and total size of the\n");
  printf("                     range must be 16-byte aligned.\n");
  printf("\n");
  printf("Note that it is not necessary to specify an option if its default value\n");
  printf("is already the desired value.\n");
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
  cryptoKeyFileStr = NULL;
  encryptOperation = true;
  excludeRegion = false;
  excludeRegionBaseAddr = 0x00000000;
  excludeRegionLen = 0;
  inputFileStr = NULL;
  outputFileStr = NULL;

  /* Only continue with valid parameters. Note that at least 4 command line arguments
   * are specified. The first one is always the name of the executable. Additionally,
   * 3 file names are expected: the file with the crypto key, the s-record input file,
   * and the s-record output file.
   */
  if ( (argv != NULL) && (argc >= 4) ) /*lint !e774 */
  {
    /* Loop through all the command line parameters, just skip the 1st one because this
     * is the name of the program, which we are not interested in. There is also no need
     * to iterate over the last 2 parameters, because these are always the s-record input
     * and output file, respectively.
     */
    for (paramIdx = 1; paramIdx < (argc - 2); paramIdx++)
    {
      /* Is this the -en=[value] parameter? */
      if ( (strstr(argv[paramIdx], "-en=") != NULL) &&
           (strlen(argv[paramIdx]) > 4) )
      {
        /* Extract the cryptography operation mode value. */
        static uint8_t enValue;
        sscanf(&argv[paramIdx][4], "%hhu", &enValue);
        /* Convert to boolean. */
        encryptOperation = ((enValue > 0) ? true : false);
        /* Continue with next loop iteration. */
        continue;
      }
      /* Is this the -kf=[file] parameter? */
      if ( (strstr(argv[paramIdx], "-kf=") != NULL) &&
           (strlen(argv[paramIdx]) > 4) )
      {
        /* Store the pointer to the filename of the text file that contains the crypto-
         * graphy key.
         */
        cryptoKeyFileStr = &argv[paramIdx][4];
        /* Continue with next loop iteration. */
        continue;
      }
      /* Is this the -x=[value]:[value] parameter? */
      if ( (strstr(argv[paramIdx], "-x=") != NULL) &&
           (strlen(argv[paramIdx]) >= 6) )
      {
        uint32_t startAddr = 0;
        uint32_t endAddr = 0;
        /* Extract the hexadecimal start and end addresses for the region to exclude. */
        sscanf(&argv[paramIdx][3], "%x:%x", &startAddr, &endAddr);
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
    /* Store the pointer to the filename of the s-record input file. */
    inputFileStr = &argv[argc - 2][0];
    /* Store the pointer to the filename of the s-record output file. */
    outputFileStr = &argv[argc - 1][0];
  }
  /* Update the result value. */
  if ( (cryptoKeyFileStr != NULL) && (inputFileStr != NULL) && (outputFileStr != NULL) )
  {
    result = true;
  }
  /* In case an exclusion region was specified, verify that the base address and region
   * length are both 16-byte aligned.
   */
  if (excludeRegion)
  {
    if ( ((excludeRegionBaseAddr % 16) != 0) || ((excludeRegionLen % 16) != 0) )
    {
      result = false;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of ParseCommandLine ***/


/************************************************************************************//**
** \brief     Extract the 256-bit AES-ECB cryptography key from the file contents.
**            The key can be entered on a line in the text-file using the following
**            format:
**              key=0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
**            So the key consists of 64 hexadecimal characters. 1 Hexadecimal character
**            holds 4 bits, so altogether this makes 256 bits for the key. The extracted
**            key is stored in a byte-array with 32 entries.
** \param     cryptoKeyFile The filename of the text-file that holds the key.
** \param     cryptoKey Pointer to byte array with at least 32 elements where the
**            extracted cryptography key will be written to.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool ExtractCryptoKeyFromFile(char const * cryptoKeyFile, uint8_t * cryptoKey)
{
  bool result = false;
  FILE *fp;
  /* The line is only expected to have "key=" + 64 characters + CR + LF = 70 characters.
   * just make the line buffer a bit bigger in case the file contains other lines. Note
   * that this array was made static to lower the stack load.
   */
  static char line[256];

  /* Check parameters. */
  assert(cryptoKeyFile != NULL);
  assert(cryptoKey != NULL);

  /* Only continue with valid parameters. */
  if ( (cryptoKeyFile != NULL) && (cryptoKey != NULL) ) /*lint !e774 */
  {
    /* Open the file for reading. */
    fp = fopen(cryptoKeyFile, "r");
    /* Only continue if the filepointer is valid. */
    if (fp != NULL)
    {
      /* Start at the beginning of the file. */
      rewind(fp);
      /* Read the entire file, one line at a time. */
      while (fgets(line, sizeof(line)/sizeof(line[0]), fp) != NULL )
      {
        /* Does this line start with "key="? */
        if (strstr(line, "key=") == &line[0])
        {
          /* The key string should have least 64 hexadecimal characters. */
          if (strlen(&line[4]) >= 64)
          {
            /* Read out the 32 bytes of the cryptography key one-by-one. */
            for (uint8_t idx = 0; idx < 32; idx++)
            {
              sscanf(&line[4 + (idx * 2)], "%2hhx", &cryptoKey[idx]);
            }
            /* All key characters read. Update the result value. */
            result = true;
          }
        }
        /* Check if the key was read, in which case there is no need to continue
         * reading from the file.
         */
        if (result)
        {
          break;
        }
      }
      /* Close the file now that we are done with it. */
      fclose(fp);
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of ExtractCryptoKeyFromFile ***/


/*********************************** end of main.c *************************************/


