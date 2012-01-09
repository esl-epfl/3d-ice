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
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "ni_client.h"

/******************************************************************************/

Error_t init_client_unix_socket
(
    UnixSocket_t *socket_id,
    String_t      socket_name
)
{
    *socket_id = socket (AF_UNIX, SOCK_STREAM, 0) ;

    if (*socket_id < 0)
    {
        perror ("ERROR :: Creating client unix socket") ;

        return TDICE_FAILURE ;
    }

    struct sockaddr_un server_address ;

    memset (&server_address, '\0', sizeof(struct sockaddr_un)) ;

    server_address.sun_family = AF_UNIX ;

    strcpy (server_address.sun_path, socket_name) ;

    socklen_t servlen = (socklen_t)

        (strlen (server_address.sun_path) + sizeof (server_address.sun_family)) ;

    if (connect (*socket_id, (struct sockaddr *) &server_address, servlen) < 0)
    {
        perror ("ERROR :: Connecting client unix socket") ;

        return TDICE_FAILURE ;
    }

    // fprintf(stdout, "Connection established.\n") ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t init_client_network_socket
(
    NetworkSocket_t *socket_id,
    String_t        host_name,
    int             port_number
)
{
    *socket_id = socket (AF_INET, SOCK_STREAM, 0) ;

    if (*socket_id < 0)
    {
        perror ("ERROR :: Creating client network socket") ;

        return TDICE_FAILURE ;
    }

    struct hostent *server = gethostbyname (host_name) ;  // FIXME this is obsolete

    if (server == NULL)
    {
        fprintf (stderr, "ERROR :: host %s not found\n", host_name) ;

        close_client_network_socket (socket_id) ;

        return TDICE_FAILURE ;
    }

    struct sockaddr_in server_address ;

    memset (&server_address, '\0', sizeof(struct sockaddr_in)) ;

    server_address.sin_family = AF_INET ;
    server_address.sin_port   = htons (port_number) ;

    memcpy

        (&server_address.sin_addr.s_addr, server->h_addr, server->h_length) ;

    int tmp = connect (                    *socket_id,
                       (struct sockaddr *) &server_address,
                                            sizeof(struct sockaddr_in)) ;

    if (tmp < 0)
    {
        perror ("ERROR :: Connecting client network socket") ;

        close_client_network_socket (socket_id) ;

        return TDICE_FAILURE ;
    }

    // fprintf(stdout, "Connection established.\n");

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_client_unix_socket
(
    UnixSocket_t *socket_id
)
{
    if (close (*socket_id) != 0)
    {
        perror ("ERROR :: Closing client unix socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_client_network_socket
(
    NetworkSocket_t *socket_id
)
{
    if (close (*socket_id) != 0)
    {
        perror ("ERROR :: Closing client network socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/
