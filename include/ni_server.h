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

#ifndef _3DICE_SERVER_H_
#define _3DICE_SERVER_H_

/*! \file ni_server.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <netinet/in.h>

#include "types.h"
#include "ni_client.h"

/******************************************************************************/

    /*! \struct ServerSocket
     *
     *  \brief Structure used to set up network connections in the server side
     *
     */

    struct ServerSocket
    {
        /*! The descriptor of the socket (like a unique id) */

        NetworkSocket_t    SocketId ;

        /*! The address of the server that receives data from the client */

        struct sockaddr_in ServerAddress ;
    } ;

    /*! Definition of the type ServerSocket */

    typedef struct ServerSocket ServerSocket ;



    /*! Initializes a server socket
     *
     * The function creates the socket used by the server. Then it binds
     * the socket to the \a port_numebr over any kind of address. It finally
     * enables the listening.
     *
     * \param server_socket the address of the ServerSocket to initialize
     * \param port_number the port number of the server
     *
     * \return \c TDICE_SUCCESS if the initialization succeeded
     * \return \c TDICE_FAILURE if the initialization fails. A message will be
     *                          printed on standard error
     */

    Error_t init_server_socket

        (ServerSocket *server_socket, int port_number) ;



    /*! Waits unitl a client sends a connect to the server
     *
     * \param server_socket the address of the ServerSocket that will wait
     * \param client_socket the address of the ClientSocket that will connect_to_server
     *
     * \return \c TDICE_SUCCESS if the connection with the client succeeded
     * \return \c TDICE_FAILURE if the connection fails. A message will be
     *                          printed on standard error
     */

    Error_t wait_for_client

        (ServerSocket *server_socket, ClientSocket *client_socket) ;


    /*! Closes a server socket
     *
     * \param server_socket the address of the ServerSocket to close
     *
     * \return \c TDICE_SUCCESS if the closure succeeded
     * \return \c TDICE_FAILURE if the closure fails. A message will be
     *                          printed on standard error
     */

    Error_t close_server_socket (ServerSocket *server_socket) ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SERVER_H_ */
