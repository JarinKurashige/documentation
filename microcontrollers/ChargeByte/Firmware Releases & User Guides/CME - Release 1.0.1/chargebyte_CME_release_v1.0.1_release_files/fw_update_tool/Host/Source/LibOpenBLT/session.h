/************************************************************************************//**
* \file         session.h
* \brief        Communication session module header file.
* \ingroup      Session
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
* \defgroup   Session Communication Session Module
* \brief      Module with functionality to communicate with the bootloader on the target
*             system.
* \ingroup    Library
* \details
* The Communication Session module handles the communication with the bootloader during
* firmware updates on the target system. It contains an interface to link the desired
* communication protocol that should be used for the communication. For example the XCP
* protocol.
****************************************************************************************/
#ifndef SESSION_H
#define SESSION_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Session communication protocol interface. */
typedef struct t_session_protocol
{
  /** \brief Initializes the protocol module. */
  void (* Init) (void const * settings);
  /** \brief Terminates the protocol module. */
  void (* Terminate) (void);
  /** \brief Starts the firmware update session. This is where the connection with the
   *         target is made and the bootloader on the target is activated.
   */
  bool (* Start) (void);
  /** \brief Stops the firmware update. This is where the bootloader starts the user
   *         program on the target if a valid one is present. After this the connection
   *         with the target is severed.
   */
  void (* Stop) (void);
  /** \brief Requests the bootloader to erase the specified range of memory on the
   *         target. The bootloader aligns this range to hardware specified erase blocks.
   */
  bool (* ClearMemory) (uint32_t address, uint32_t len);
  /** \brief Requests the bootloader to program the specified data to memory. In case of
   *         non-volatile memory, the application needs to make sure the memory range
   *         was erased beforehand.
   */
  bool (* WriteData) (uint32_t address, uint32_t len, uint8_t const * data);  
  /** \brief Request the bootloader to upload the specified range of memory. The data is
   *         stored in the data byte array to which the pointer was specified.
   */
  bool (* ReadData) (uint32_t address, uint32_t len, uint8_t * data);
} tSessionProtocol;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void SessionInit(tSessionProtocol const * protocol, void const * protocolSettings);
void SessionTerminate(void);
bool SessionStart(void);
void SessionStop(void);
bool SessionClearMemory(uint32_t address, uint32_t len);
bool SessionWriteData(uint32_t address, uint32_t len, uint8_t const * data);  
bool SessionReadData(uint32_t address, uint32_t len, uint8_t * data);


#ifdef __cplusplus
}
#endif

#endif /* SESSION_H */
/********************************* end of session.h ************************************/

 
 
