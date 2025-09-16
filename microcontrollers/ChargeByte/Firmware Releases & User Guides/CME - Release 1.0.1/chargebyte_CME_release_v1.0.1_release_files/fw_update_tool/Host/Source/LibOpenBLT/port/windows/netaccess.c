/************************************************************************************//**
* \file         port/windows/netaccess.c
* \brief        TCP/IP network access source file.
* \ingroup      NetAccess
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
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include <winsock2.h>                       /* for WinSock2 definitions                */
#include <ws2tcpip.h>                       /* for WinSock2 TCP/IP protocol extensions */
#include "netaccess.h"                      /* TCP/IP network access module            */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Boolean flag to keep track if the Winsock library is initialized. */
static bool winsockInitialized;

/** \brief The socket that is used as an endpoint for the TCP/IP network
 * communication.
 */
static SOCKET netAccessSocket;


/************************************************************************************//**
** \brief     Initializes the network access module.
**
****************************************************************************************/
void NetAccessInit(void)
{
  WSADATA wsa;

  /* Invalidate the socket. */
  netAccessSocket = INVALID_SOCKET;
  
  /* Init locals. */
  winsockInitialized = false;
  
  /* Attempt to initialize the Winsock library. */
  if (WSAStartup(MAKEWORD(2,2),&wsa) == 0)
  {
    /* Update flag. */
    winsockInitialized = true;
  }
} /*** end of NetAccessInit ***/


/************************************************************************************//**
** \brief     Terminates the network access module.
**
****************************************************************************************/
void NetAccessTerminate(void)
{
  /* Make sure to disconnect form the server. */
  NetAccessDisconnect();
  
  /* Cleanup the Winsock library, if it was initialized. */
  if (winsockInitialized)
  {
    /* Update flag. */
    winsockInitialized = false;
    /* Free Winsock resources. */
    (void)WSACleanup();
  }
} /*** end of NetAccessTerminate ***/


/************************************************************************************//**
** \brief     Connects to the TCP/IP server at the specified address and the given port.
** \param     address The address of the server. This can be a hostname (such as
**            mydomain.com) or an IP address (such as 127.0.0.1).
** \param     port The port number on the server to connect to.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool NetAccessConnect(char const * address, uint16_t port)
{
  bool result = false;
  struct addrinfo hints = { 0 };
  struct addrinfo * serverinfo = NULL;
  struct sockaddr_in serverIPv4 = { 0 };
  struct sockaddr_in6 serverIPv6 = { 0 };
  bool serverIPv4found = false;
  bool serverIPv6found = false;
  
  /* Check parameters. */
  assert(address != NULL);
  assert(port > 0);

  /* Start by invalidating the socket. */
  netAccessSocket = INVALID_SOCKET;

  /* Only continue with valid parameters and an initialized Winsock. */
  if ( (address != NULL) && (port > 0) && (winsockInitialized) ) /*lint !e774 */
  {
    /* Set result to true and only reset it to false upon detection of a problem. */
    result = true;

    /* Initialize hints structure to aid in hostname resolving. Note that AF_UNSPEC is
     * used to support both IPv4 and IPv6.
     */
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = (int)SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    /* Attempt to resolve the hostname. This converts the hostname to an IP address, if
     * it wasn't already an IP address.
     */
    if (getaddrinfo(address, NULL, &hints, &serverinfo) != 0)
    {
      /* Could not resolve the hostname. */
      result = false;
    }
    /* Sanity check on the pointer that should now be initialized and contain data. */
    else
    {
      if (serverinfo == NULL)
      {
        result = false;
      }
    }
    /* The serverinfo pointer now points to an array with results of the hostname
     * resolving. We only need one, so grab the first valid one. Prefer IPv4 over IPv6.
     */
    if (result)
    {
      /* Point to the first entry. */
      struct addrinfo * entry = serverinfo;
      /* Loop over the entries until a valid one was found. */
      while (entry != NULL)
      {
        /* Does this entry contain an IPv4 address? */
        if (entry->ai_family == AF_INET)
        {
          /* Copy this one for later usage. */
          memcpy (&serverIPv4, entry->ai_addr, entry->ai_addrlen);
          serverIPv4.sin_family = AF_INET;
          serverIPv4.sin_port = htons(port);
          /* Set flag so we know which socket address variable to use later on. */
          serverIPv4found = true;
          /* No need to go over the other entries, since we found a valid one. */
          break;
        }
        /* Does this entry contain an IPv6 address? */
        if (entry->ai_family == AF_INET6)
        {
          /* Copy this one for later usage. */
          memcpy (&serverIPv6, entry->ai_addr, entry->ai_addrlen);
          serverIPv6.sin6_family = AF_INET6;
          serverIPv6.sin6_port = htons(port);
          /* Set flag so we know which socket address variable to use later on. */
          serverIPv6found = true;
          /* No need to go over the other entries, since we found a valid one. */
          break;
        }
        /* Move on to the next one. */
        entry = entry->ai_next;
      }
    }
    /* Check that a valid entry was found. */
    if (result)
    {
      if ( (!serverIPv4found) && (!serverIPv6found) )
      {
        result = false;
      }
    }
    /* Create the socket. */
    if (result)
    {
      /* Create the socket based on the family type. */
      if (serverIPv4found)
      {
        netAccessSocket = socket(serverIPv4.sin_family, (int)SOCK_STREAM, (int)IPPROTO_TCP);
      }
      else
      {
        netAccessSocket = socket(serverIPv6.sin6_family, (int)SOCK_STREAM, (int)IPPROTO_TCP);
      }
      /* Check the socket. */
      if (netAccessSocket == INVALID_SOCKET)
      {
        /* Could not create the socket. */
        result = false;
      }
    }
    /* Connect the socket. */
    if (result)
    {
      /* Get the socket address pointer based on the family type. */
      struct sockaddr * server_address;
      if (serverIPv4found)
      {
        server_address = (struct sockaddr *)&serverIPv4; /*lint !e740 */
      }
      else
      {
        server_address = (struct sockaddr *)&serverIPv6; /*lint !e740 */
      }
      /* Attempt to connect. */
      if (connect(netAccessSocket, server_address, sizeof(struct sockaddr)) != 0)
      {
        /* Could not connect. Close the socket and negate result value. */
        (void)closesocket(netAccessSocket);
        netAccessSocket = INVALID_SOCKET;
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of NetAccessConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the TCP/IP server.
**
****************************************************************************************/
void NetAccessDisconnect(void)
{
  /* Only perform disconnect with an initialized Winsock. */
  if (winsockInitialized)
  {
    /* Close the socket if it is open. */
    if (netAccessSocket != INVALID_SOCKET)
    {
      (void)closesocket(netAccessSocket);
      netAccessSocket = INVALID_SOCKET;
    }
  }
} /*** end of NetAccessDisconnect ***/


/************************************************************************************//**
** \brief     Sends data to the TCP/IP server.
** \param     data Pointer to byte array with data to send.
** \param     length Number of bytes to send.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool NetAccessSend(uint8_t const * data, uint32_t length)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);

  /* Only continue with valid parameters and an initialized Winsock. */
  if ( (data != NULL) && (length > 0) && (winsockInitialized) ) /*lint !e774 */
  {
    /* Only continue with a valid socket. */
    if (netAccessSocket != INVALID_SOCKET)
    {
      /* Attempt to send the data. */
      if (send(netAccessSocket, (char const *)data, (int)length, 0) != SOCKET_ERROR)
      {
        /* Successfully send the data. */
        result = true;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of NetAccessSend ***/


/************************************************************************************//**
** \brief     Receives data from the TCP/IP server in a blocking manner.
** \param     data Pointer to byte array to store the received data.
** \param     length Holds the max number of bytes that can be stored into the byte
**            array. This function also overwrites this value with the number of bytes
**            that were actually received.
** \param     timeout Timeout in milliseconds for the data reception.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool NetAccessReceive(uint8_t * data, uint32_t * length, uint32_t timeout)
{
  bool result = false;
  int tv;
  int receivedLen;

  /* Check parameters. */
  assert(data != NULL);
  assert(length != NULL);
  assert(timeout > 0);

  /* Only continue with valid parameters and an initialized Winsock. */
  if ( (data != NULL) && (length != NULL) && (timeout > 0) &&
       (winsockInitialized) ) /*lint !e774 */
  {
    /* Only continue with a valid socket. */
    if (netAccessSocket != INVALID_SOCKET)
    {
      /* Configure the timeout for the receive operation. */
      tv = (int)timeout;
      if (setsockopt(netAccessSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(int)) != SOCKET_ERROR)
      {
        /* Attempt to receive data. */
        receivedLen = recv(netAccessSocket, (char *)data, (int)*length, 0);
        /* Process the result. Everything < 0 indicate that an error occured. A value of
         * zero is also treated as an error, since data was expected.
         */
        if ((receivedLen != SOCKET_ERROR) && (receivedLen > 0))
        {
          /* Store the number of received bytes. */
          *length = (uint32_t)receivedLen;
          /* Successfully received data. */
          result = true;
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of NetAccessReceive ***/


/*********************************** end of netaccess.c ********************************/

