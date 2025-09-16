/************************************************************************************//**
* \file         serialport.h
* \brief        Serial port header file.
* \ingroup      SerialPort
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
* \defgroup   SerialPort Serial port driver
* \brief      This module implements a generic serial port driver.
* \ingroup    Session
****************************************************************************************/
#ifndef SERIALPORT_H
#define SERIALPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Enumeration of the supported baudrates. */
typedef enum
{
  SERIALPORT_BR9600   = 0,                  /**< 9600 bits/sec                         */
  SERIALPORT_BR19200  = 1,                  /**< 19200 bits/sec                        */
  SERIALPORT_BR38400  = 2,                  /**< 38400 bits/sec                        */
  SERIALPORT_BR57600  = 3,                  /**< 57600 bits/sec                        */
  SERIALPORT_BR115200 = 4                   /**< 115200 bits/sec                       */
} tSerialPortBaudrate;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
void SerialPortInit(void);
void SerialPortTerminate(void);
bool SerialPortOpen(char const * portname, tSerialPortBaudrate baudrate);
void SerialPortClose(void);
bool SerialPortWrite(uint8_t const * data, uint32_t length);
bool SerialPortRead(uint8_t * data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* SERIALPORT_H */
/********************************* end of serialport.h *********************************/

