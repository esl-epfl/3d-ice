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
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "macros.h"
#include "network_interface.h"

/******************************************************************************/

void init_socket (Socket *socket)
{
    socket->Id = 0 ;

    memset ((void *) &(socket->Address), 0, sizeof (struct sockaddr_in)) ;

    memset ((void *) &(socket->HostName), '\0', sizeof (socket->HostName)) ;

    socket->PortNumber = 0u ;
}

/******************************************************************************/

Error_t open_client_socket (Socket *client)
{
    client->Id = socket (AF_INET, SOCK_STREAM, 0) ;

    if (client->Id < 0)
    {
        perror ("ERROR :: client socket creation") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t open_server_socket
(
    Socket       *server,
    PortNumber_t  port_number
)
{
    server->Id = socket (AF_INET, SOCK_STREAM, 0) ;

    if (server->Id < 0)
    {
        perror ("ERROR :: server socket creation") ;

        return TDICE_FAILURE ;
    }

    server->Address.sin_family      = AF_INET ;
    server->Address.sin_port        = htons (port_number) ;
    server->Address.sin_addr.s_addr = htonl (INADDR_ANY) ;

    if (bind (server->Id, (struct sockaddr *) &server->Address,
              sizeof (struct sockaddr_in)) < 0)
    {
        perror ("ERROR :: server bind") ;

        close_socket (server) ;

        return TDICE_FAILURE ;
    }

    if (listen (server->Id, 1) < 0)
    {
        perror ("ERROR :: server listen") ;

        close_socket (server) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t connect_client_to_server
(
    Socket       *client,
    String_t      host_name,
    PortNumber_t  port_number
)
{
    strcpy (client->HostName, host_name) ;

    client->PortNumber = port_number ;

    client->Address.sin_family = AF_INET ;
    client->Address.sin_port   = htons (port_number) ;

    if (inet_pton (AF_INET, host_name, &(client->Address).sin_addr) <= 0)
    {
        perror ("ERROR :: server address creation") ;

        close_socket (client) ;

        return TDICE_FAILURE ;
    }

    if (connect (client->Id, (struct sockaddr *) &client->Address,
                 sizeof (struct sockaddr_in)) < 0)
    {
        perror ("ERROR :: client to server connection") ;

        close_socket (client) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t wait_for_client (Socket *server, Socket *client)
{
    socklen_t length = sizeof (struct sockaddr_in) ;

    client->Id = accept

        (server->Id, (struct sockaddr *) &(client->Address), &length) ;

    if (client->Id < 0)
    {
        perror ("ERROR :: server accept") ;

        close_socket (server) ;

        return TDICE_FAILURE ;
    }

    client->PortNumber = ntohs (client->Address.sin_port) ;

    if (inet_ntop (AF_INET, &client->Address.sin_addr,
                   client->HostName, sizeof (client->HostName)) == NULL)
    {
        perror ("ERROR :: client name translation") ;

        close_socket (client) ;
        close_socket (server) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t send_message_to_socket
(
    Socket   *socket,
    String_t  message
)
{
    ssize_t bwritten ;

    // here sizeof(message) is the dimension in bytes of a poiter to char
    // while strlen(message) is the length of the message, i.e. the number
    // of bytes before '\0' to be sent.

    StringLength_t length = strlen (message) ;

    // Sends the dimension of the message as first information

    bwritten = write (socket->Id, &length, sizeof(StringLength_t)) ;

    if (bwritten < 0)
    {
        perror ("ERROR :: write message length failure") ;

        return TDICE_FAILURE ;
    }

    if (bwritten != sizeof(StringLength_t))
    {
        fprintf (stderr, "ERROR :: write message length failure") ;

        return TDICE_FAILURE ;
    }

    // then it sends the real message ...

    while (length > 0)
    {
        // writes up to "length" bytes taken from "message" to the socket
        // returns the number of bytes sent, or -1

        bwritten = write (socket->Id, message, length) ;

        if (bwritten < 0)
        {
            if (errno == EINTR)

                continue ;

            else
            {
                perror ("ERROR :: write message failure") ;

                return TDICE_FAILURE ;
            }
        }

        // updates missing bytes to send and move the pointer forward

        length  -= bwritten ;
        message += bwritten ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t receive_message_from_socket
(
    Socket         *socket,
    String_t        message,
    StringLength_t  length
)
{
    ssize_t bread ;

    // here sizeof(message) is the dimension in bytes of a poiter to char
    // while strlen(message) is the length of the message, i.e. the number
    // of bytes before '\0' to be sent. The latter cannot be used since
    // message might contain garbage values, i.e. the value '\0' might
    // be in any location.

    StringLength_t message_length ;

    bread = read (socket->Id, &message_length, sizeof(StringLength_t)) ;

    if (bread < 0)
    {
        perror ("ERROR :: read message length failure") ;

        return TDICE_FAILURE ;
    }

    if (bread != sizeof(StringLength_t))
    {
        fprintf (stderr, "ERROR :: read message length failure") ;

        return TDICE_FAILURE ;
    }

    if (message_length > length)
    {
        fprintf (stderr, "ERROR :: not enough space to receive\n") ;

        return EXIT_FAILURE ;
    }

    while (message_length > 0)
    {
        bread = read (socket->Id, message, message_length) ;

        if (bread < 0)
        {
            if (errno == EINTR)

                continue ;

            else
            {
                perror ("ERROR :: read failure") ;

                return TDICE_FAILURE ;
            }
        }
        else if (bread == 0)

            break ;

        message_length -= bread ;
        message        += bread ;
    }

    *message = '\0' ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t close_socket (Socket *socket)
{
    if (close (socket->Id) != 0)
    {
        perror ("ERROR :: Closing network socket") ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/
