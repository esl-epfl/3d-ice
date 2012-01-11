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
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "macros.h"
#include "network_interface.h"

/******************************************************************************/

Error_t init_client_socket (ClientSocket *client_socket)
{
    client_socket->SocketId = socket (AF_INET, SOCK_STREAM, 0) ;

    if (client_socket->SocketId < 0)
    {
        perror ("ERROR :: client socket creation") ;

        return TDICE_FAILURE ;
    }

    memset ((void *) &(client_socket->ServerAddress), 0, sizeof (struct sockaddr_in)) ;

    memset ((void *) &(client_socket->HostName), '\0', sizeof (client_socket->HostName)) ;

    client_socket->PortNumber = 0u ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t connect_to_server
(
    ClientSocket *client_socket,
    String_t      host_name,
    PortNumber_t  port_number
)
{
    strcpy (client_socket->HostName, host_name) ;

    client_socket->PortNumber = port_number ;

    client_socket->ServerAddress.sin_family = AF_INET ;
    client_socket->ServerAddress.sin_port   = htons (port_number) ;

    if (inet_pton (AF_INET, host_name, &(client_socket->ServerAddress).sin_addr) <= 0)
    {
        perror ("ERROR :: server address creation") ;

        close_client_socket (client_socket) ;

        return TDICE_FAILURE ;
    }

    if (connect (client_socket->SocketId,
                 (struct sockaddr *) &client_socket->ServerAddress,
                  sizeof (struct sockaddr_in)) < 0)
    {
        perror ("ERROR :: client to server connection") ;

        close_client_socket (client_socket) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_client_socket
(
    ClientSocket *client_socket
)
{
    if (close (client_socket->SocketId) != 0)
    {
        perror ("ERROR :: Closing client network socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/
