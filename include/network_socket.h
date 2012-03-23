/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_NETWORK_SOCKET_H_
#define _3DICE_NETWORK_SOCKET_H_

/*! \file network_socket.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <netinet/in.h>

#include "types.h"
#include "network_message.h"

/******************************************************************************/

    /*! \struct Socket
     *
     *  \brief Structure used to set up and use network connections
     *
     */

    struct Socket
    {
        /*! The descriptor of the socket (like a unique id - file descriptor) */

        NetworkSocket_t Id ;

        /*! The IP address related to the socket */

        struct sockaddr_in Address ;

        /*! The IP address, as a string */

        char HostName [32] ;

        /*! The port number (host horder) */

        PortNumber_t PortNumber ;
    } ;

    /*! Definition of the type Socket */

    typedef struct Socket Socket ;


    /*! Initializes a socket
     *
     * \param socket the address of the Socket to initialize
     */

    void init_socket (Socket *socket) ;



    /*! Open a socket for the client side
     *
     * \param client the address of the Socket to open
     *
     * \return \c TDICE_SUCCESS if the opening succeeded
     * \return \c TDICE_FAILURE if the opening fails. A message will be
     *                          printed on standard error
     */

    Error_t open_client_socket (Socket *client) ;



    /*! Open a socket for the server side
     *
     * The function creates the socket. Then it binds the socket to the
     * \a port_numebr over any kind of address. It finally enables the
     * listening.
     *
     * \param server      the address of the Socket to open
     * \param port_number the port number of the server
     *
     * \return \c TDICE_SUCCESS if the opening succeeded
     * \return \c TDICE_FAILURE if the opening fails. A message will be
     *                          printed on standard error
     */

    Error_t open_server_socket
    (
        Socket       *server,
        PortNumber_t  port_number
    ) ;



    /*! Connects the client to a server
     *
     * Prepares the address of the server and establish the connection.
     * The server side must be waiting for a connection. On error, the
     * socket is closed.
     *
     * \param client      the address of the ClientSocket to initialize
     * \param host_name   the ip address of the server (as dotted string)
     * \param port_number the port number of the server
     *
     * \return \c TDICE_SUCCESS if the connection succeeded
     * \return \c TDICE_FAILURE if the connections fails. A message will be
     *                          printed on standard error
     */

    Error_t connect_client_to_server
    (
        Socket       *client,
        String_t      host_name,
        PortNumber_t  port_number
    ) ;



    /*! Waits unitl a client sends a connect to the server
     *
     * \param server the address of the ServerSocket that will wait
     * \param client the address of the ClientSocket that will connect_to_server
     *
     * \return \c TDICE_SUCCESS if the connection with the client succeeded
     * \return \c TDICE_FAILURE if the connection fails. A message will be
     *                          printed on standard error
     */

    Error_t wait_for_client (Socket *server, Socket *client) ;



    /*! Sends a message to a socket
     *
     * \param socket  the socket where the message will be sent
     * \param message the address of message to send
     *
     * \return \c TDICE_SUCCESS if the operation succeeded
     * \return \c TDICE_FAILURE if the operation fails. A message will be
     *                          printed on standard error
     */

    Error_t send_message_to_socket
    (
        Socket         *socket,
        NetworkMessage *message
    ) ;



    /*! Receive a text message from a socket
     *
     * \param socket  the socket where the message will be received
     * \param message the address of message to receive
     *
     * \return \c TDICE_SUCCESS if the operation succeeded
     * \return \c TDICE_FAILURE if the operation fails. A message will be
     *                          printed on standard error
     */

    Error_t receive_message_from_socket
    (
        Socket         *socket,
        NetworkMessage *message
    ) ;



    /*! Closes a socket
     *
     * \param socket the address of the Socket to close
     *
     * \return \c TDICE_SUCCESS if the closure succeeded
     * \return \c TDICE_FAILURE if the closure fails. A message will be
     *                          printed on standard error
     */

    Error_t close_socket (Socket *socket) ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_NETWORK_SOCKET_H_ */
