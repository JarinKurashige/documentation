/************************************************************************************//**
* \file         firmware.h
* \brief        Firmware data module header file.
* \ingroup      Firmware
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
* \defgroup   Firmware Firmware Data Module
* \brief      Module with functionality to load, manipulate and store firmware data.
* \ingroup    Library
* \details
* The Firmwarwe Data module contains functionality to load, manipulate and store firmware
* data. It contains an interface for linking firmware file parsers that handle the
* loading and saving the firmware data from and to a file in the correct format. For 
* example the Motorola S-record format.
****************************************************************************************/
#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Groups information together of a firmware segment, such that it can be used
 *         as a node in a linked list. 
 */
typedef struct t_firmware_segment
{
  /** \brief Start memory address of the segment. */
  uint32_t base;  
  /** \brief Number of data bytes in the segment. */
  uint32_t length;                
  /** \brief Pointer to array with the segment's data bytes. */
  uint8_t *data;                  
  /** \brief Pointer to the previous node, or NULL if it is the first one. */
  struct t_firmware_segment * prev;
  /** \brief Pointer to the next node, or NULL if it is the last one. */
  struct t_firmware_segment * next;
} tFirmwareSegment;

/** \brief Firmware file parser. */
typedef struct t_firmware_parser
{
  /** \brief Extract the firmware segments from the firmware file and add them as nodes
   *         to the linked list.
   */
  bool (* LoadFromFile) (char const * firmwareFile, uint32_t addressOffset);
  /** \brief Write all the firmware segments from the linked list to the specified
   *         firmware file.
   */
  bool (* SaveToFile) (char const * firmwareFile);
} tFirmwareParser;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void FirmwareInit(tFirmwareParser const * parser);
void FirmwareTerminate(void);
bool FirmwareLoadFromFile(char const * firmwareFile, uint32_t addressOffset);
bool FirmwareSaveToFile(char const * firmwareFile);
uint32_t FirmwareGetSegmentCount(void);
tFirmwareSegment * FirmwareGetSegment(uint32_t segmentIdx);
bool FirmwareAddData(uint32_t address, uint32_t len, uint8_t const * data);
bool FirmwareRemoveData(uint32_t address, uint32_t len);
void FirmwareClearData(void);


#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
/********************************* end of firmware.h ***********************************/

 
 
