/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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

#ifndef _3DICE_CLIENT_H_
#define _3DICE_CLIENT_H_

/*! \file ni_client.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <netinet/in.h>

#include "types.h"

/******************************************************************************/

    /*! \struct ClientSocket
     *
     *  \brief Structure used to set up network connections in the client side
     *
     */

    struct ClientSocket
    {
        /*! The descriptor of the socket (like a unique id) */

        NetworkSocket_t    SocketId ;

        /*! The address of the server with which the client will communicate */

        struct sockaddr_in ServerAddress ;
    } ;

    /*! Definition of the type ClientSocket */

    typedef struct ClientSocket ClientSocket ;


    /*! Initializes a client socket
     *
     * \param client_socket the address of the ClientSocket to initialize
     *
     * \return \c TDICE_SUCCESS if the initialization succeeded
     * \return \c TDICE_FAILURE if the initialization fails. A message will be
     *                          printed on standard error
     */

    Error_t init_client_socket (ClientSocket *client_socket) ;



    /*! Connects the client to a server
     *
     * Prepares the address of the server and establish the connection.
     * The server side must be waiting for a connection. On error, the
     * socket is closed.
     *
     * \param client_socket the address of the ClientSocket to initialize
     * \param host_name the ip address of the server
     * \param port_number the port number of the server
     *
     * \return \c TDICE_SUCCESS if the connection succeeded
     * \return \c TDICE_FAILURE if the connections fails. A message will be
     *                          printed on standard error
     */

    Error_t connect_to_server

        (ClientSocket *client_socket, String_t host_name, int port_number) ;



    /*! Closes a client socket
     *
     * \param client_socket the address of the ClientSocket to close
     *
     * \return \c TDICE_SUCCESS if the closure succeeded
     * \return \c TDICE_FAILURE if the closure fails. A message will be
     *                          printed on standard error
     */

    Error_t close_client_socket (ClientSocket *client_socket) ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CLIENT_H_ */
