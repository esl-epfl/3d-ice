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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "ni_server.h"

/******************************************************************************/

Error_t init_server_socket
(
    ServerSocket *server_socket,
    int           port_number
)
{
    server_socket->SocketId = socket (AF_INET, SOCK_STREAM, 0) ;

    if (server_socket->SocketId < 0)
    {
        perror ("ERROR :: server socket creation") ;

        return TDICE_FAILURE ;
    }

    memset ((void *) &(server_socket->ServerAddress), 0, sizeof (struct sockaddr_in)) ;

    server_socket->ServerAddress.sin_family      = AF_INET ;
    server_socket->ServerAddress.sin_port        = htons (port_number) ;
    server_socket->ServerAddress.sin_addr.s_addr = htonl (INADDR_ANY) ;

    if (bind (server_socket->SocketId,
              (struct sockaddr *) &server_socket->ServerAddress,
              sizeof (struct sockaddr_in)) < 0)
    {
        perror ("ERROR :: server bind") ;

        close_server_socket (server_socket) ;

        return TDICE_FAILURE ;
    }

    if (listen (server_socket->SocketId, 1) < 0)
    {
        perror ("ERROR :: server listen") ;

        close_server_socket (server_socket) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t wait_for_client
(
    ServerSocket *server_socket,
    ClientSocket *client_socket
)
{
    socklen_t length = sizeof (struct sockaddr_in) ;

    client_socket->SocketId = accept (server_socket->SocketId,
                                      (struct sockaddr *) &(client_socket->ServerAddress),
                                      &length) ;

    if (client_socket->SocketId < 0)
    {
        perror ("ERROR :: server accept") ;

        close_server_socket (server_socket) ;

        return TDICE_FAILURE ;
    }

    client_socket->PortNumber = ntohs (client_socket->ServerAddress.sin_port) ;

    if (inet_ntop (AF_INET, &client_socket->ServerAddress.sin_addr,
                   client_socket->HostName, sizeof (client_socket->HostName)) == NULL)
    {
        perror ("ERROR :: client name translation") ;

        close_client_socket (client_socket) ;
        close_server_socket (server_socket) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_server_socket
(
    ServerSocket *server_socket
)
{
    if (close (server_socket->SocketId) != 0)
    {
        perror ("ERROR :: Closing server network socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/
