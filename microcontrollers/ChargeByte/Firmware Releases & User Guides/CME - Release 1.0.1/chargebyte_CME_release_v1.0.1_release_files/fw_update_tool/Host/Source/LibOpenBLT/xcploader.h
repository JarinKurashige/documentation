/************************************************************************************//**
* \file         xcploader.h
* \brief        XCP Loader module header file.
* \ingroup      XcpLoader
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
* \defgroup   XcpLoader XCP version 1.0 protocol
* \brief      This module implements the XCP communication protocol that can be linked 
*             to the Session module. 
* \ingroup    Session
* \details
* This XCP Loader module contains functionality according to the standardized XCP 
* protocol version 1.0. XCP is a universal measurement and calibration communication 
* protocol. Note that only those parts of the XCP master functionality are implemented 
* that are applicable to performing a firmware update on the slave. This means 
* functionality for reading, programming, and erasing (non-volatile) memory.
****************************************************************************************/
#ifndef XCPLOADER_H
#define XCPLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Total number of bytes in a master<->slave data packet. It should be at least
 *         equal or larger than that configured on the slave.
 */
#define XCPLOADER_PACKET_SIZE_MAX   (255u)
  
  
/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief XCP transport layer packet type. */
typedef struct t_xcp_transport_packet
{
  uint8_t data[XCPLOADER_PACKET_SIZE_MAX];       /**< Packet data.                     */
  uint8_t len;                                   /**< Packet length.                   */
} tXcpTransportPacket;

/** \brief XCP transport layer. */
typedef struct t_xcp_transport
{
  /** \brief Initialization of the XCP transport layer. */
  void (*Init) (void const * settings);
  /** \brief Termination the XCP transport layer. */
  void (*Terminate) (void);
  /** \brief Connects the XCP transport layer. */
  bool (*Connect) (void);
  /** \brief Disconnects the XCP transport layer. */
  void (*Disconnect) (void);
  /** \brief Sends an XCP packet and waits for the response to come back. */
  bool (*SendPacket) (tXcpTransportPacket const * txPacket, 
                      tXcpTransportPacket * rxPacket, uint16_t timeout);
} tXcpTransport;

/** \brief XCP protocol specific settings. */
typedef struct t_xcp_loader_settings
{
  /** \brief Command response timeout in milliseconds. */
  uint16_t timeoutT1;            
  /** \brief Start programming timeout in milliseconds. */
  uint16_t timeoutT3;            
  /** \brief Erase memory timeout in milliseconds. */
  uint16_t timeoutT4;            
  /** \brief Program memory and reset timeout in milliseconds. */
  uint16_t timeoutT5;            
  /** \brief Connect response timeout in milliseconds. */
  uint16_t timeoutT6;
  /** \brief Busy wait timer timeout in milliseconds. */
  uint16_t timeoutT7;     
  /** \brief Connection mode used in the XCP connect command. */
  uint8_t connectMode;
  /** \brief Seed/key algorithm library filename. */
  char const * seedKeyFile;
  /** \brief Pointer to the transport layer to use during protocol communications. */
  tXcpTransport const * transport;
  /** \brief Pointer to the settings for the transport layer. */
  void const * transportSettings;
} tXcpLoaderSettings;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
tSessionProtocol const * XcpLoaderGetProtocol(void);  

#ifdef __cplusplus
}
#endif

#endif /* XCPLOADER_H */
/*********************************** end of xcploader.h ********************************/
