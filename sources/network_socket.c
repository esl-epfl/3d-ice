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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "macros.h"
#include "network_socket.h"

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
    Socket         *socket,
    NetworkMessage *message
)
{
    // length stores the total number of bytes to send
    // --> The length of the message times the size of a word

    size_t        length = (size_t) *message->Length * sizeof (MessageWord_t) ;

    // Begin points to th beginning of the message ...

    MessageWord_t *begin = (MessageWord_t *) message->Memory ;

    while (length > 0)
    {

        // sends "length" bytes ...

        ssize_t bwritten = write (socket->Id, begin, length) ;

        // and check how many bytes are actually sent

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

        // decreases by the #bytes sent

        length -= (size_t) bwritten ;

        // moves the pointer forward (# of words sent)

        begin += (MessageWord_t) (bwritten / sizeof (MessageWord_t)) ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t receive_message_from_socket
(
    Socket         *socket,
    NetworkMessage *message
)
{
    MessageWord_t message_length ;

    // reads the first word : the number of words to receive

    ssize_t bread = read (socket->Id, &message_length, sizeof(message_length)) ;

    if (bread < 0 || bread != sizeof(message_length))
    {
        perror ("ERROR :: read message length failure") ;

        return TDICE_FAILURE ;
    }

    if (message_length > message->MaxLength)

        increase_message_memory (message, message_length) ;

    // stores the length of the message

    *message->Length = (MessageWord_t) message_length ;

    // now one whord has already been read !

    message_length-- ;

    // scale the remaining number of words to read to the number of bytes

    message_length *= sizeof (MessageWord_t) ;

    MessageWord_t *begin = message->Type ;

    while (message_length > 0)
    {
        bread = read (socket->Id, begin, message_length) ;

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

        // decreases by the #bytes read

        message_length -= (size_t) bread ;

        // moves the pointer forward (# of words received)

        begin += (MessageWord_t) (bread / sizeof(MessageWord_t)) ;
    }

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
