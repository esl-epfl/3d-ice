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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include "types.h"

int main (int argc, char** argv)
{
    NetworkSocket_t client_socket ;

    struct sockaddr_in server_address ;

    char message [125] ;

    ssize_t received ;

    /* Checks if all arguments are there **************************************/

    if (argc != 1)
    {
        fprintf (stderr, "Usage: \"%s \n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    client_socket = socket (AF_INET, SOCK_STREAM, 0) ;

    if (client_socket < 0)
    {
        perror ("ERROR :: client socket creation") ;

        return EXIT_FAILURE ;
    }

    fprintf (stdout, "done !\n") ;

    /* Fills server address ***************************************************/

    memset ((void *) &server_address, 0, sizeof (struct sockaddr_in)) ;

    server_address.sin_family = AF_INET ;
    server_address.sin_port   = htons (10024) ;

    if (inet_pton (AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        perror ("ERROR :: server address creation") ;

        goto server_id_error ;
    }

    /* Connect to the server **************************************************/

    fprintf (stdout, "Connecting to server ... ") ; fflush (stdout) ;

    if (connect (client_socket,
                 (struct sockaddr *) &server_address,
                  sizeof (struct sockaddr_in)) < 0)
    {
        perror ("ERROR :: client to server connection") ;

        goto connect_error ;
    }

    fprintf (stdout, "done !\n") ;

    /**************************************************************************/

    received = read (client_socket, message, 125) ;

    message [received] = '\0' ;

    fprintf (stdout, "Received >%s< (%Zu bytes) from server\n", message, received) ;

    /**************************************************************************/

    close (client_socket) ;

    return EXIT_SUCCESS ;

connect_error :
server_id_error :
                             close (client_socket) ;

                             return EXIT_FAILURE ;

}
