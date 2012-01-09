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

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <sys/un.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "ni_server.h"

/******************************************************************************/

Error_t init_server_unix_socket
(
    UnixSocket_t *socket_id,
    String_t      socket_name
)
{
    struct sockaddr_un server_address ;

    memset (&server_address, '\0', sizeof(struct sockaddr_un)) ;

    server_address.sun_family = AF_UNIX ;

    strcpy (server_address.sun_path, socket_name) ;

    socklen_t servlen = (socklen_t)

        strlen (server_address.sun_path) + sizeof(server_address.sun_family) ;

    *socket_id = socket (AF_UNIX, SOCK_STREAM, 0) ;

    if (*socket_id < 0)
    {
        perror ("ERROR :: Connecting server unix socket") ;

        return TDICE_FAILURE ;
    }

    int tmp = bind (*socket_id, (struct sockaddr *) &server_address, servlen) ;

    if (tmp < 0)
    {
        perror ("ERROR :: binding unix socket") ;

        close_server_unix_socket (socket_id) ;

        return TDICE_FAILURE ;
    }

    tmp = listen (*socket_id, 1);

    if (tmp < 0)
    {
        perror ("ERROR :: Listening unix socket") ;

        close_server_unix_socket (socket_id) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t init_server_network_socket
(
    NetworkSocket_t *socket_id,
    int              port_number
)
{
    struct sockaddr_in server_address ;

    memset (&server_address, '\0', sizeof(struct sockaddr_in) ) ;

    server_address.sin_family      = AF_INET ;
    server_address.sin_port        = htons (port_number) ;
    server_address.sin_addr.s_addr = INADDR_ANY ;

    *socket_id = socket (AF_INET, SOCK_STREAM, 0) ;

    if (*socket_id < 0)
    {
        perror ("ERROR :: Creating server network socket\n") ;

        return TDICE_FAILURE ;
    }

    int tmp = bind

        (*socket_id, (struct sockaddr *) &server_address, sizeof(server_address)) ;

    if (tmp < 0)
    {
        perror ("ERROR :: Binding server network socket\n") ;

        close_server_network_socket (socket_id) ;

        return TDICE_FAILURE ;
    }

    tmp = listen (*socket_id, 1) ;

    if (tmp < 0)
    {
        perror ("ERROR :: Listening network socket\n") ;

        close_server_network_socket (socket_id) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_server_unix_socket
(
    UnixSocket_t *socket_id
)
{
    if (close (*socket_id) != 0)
    {
        perror ("ERROR :: Closing server unix socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_server_network_socket
(
    NetworkSocket_t *socket_id
)
{
    if (close (*socket_id) != 0)
    {
        perror ("ERROR :: Closing server network socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/
