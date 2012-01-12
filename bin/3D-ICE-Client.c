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
#include <sys/time.h>

#include "network_interface.h"

#define MESSAGE_LENGTH 32

// void seed_random ()
// {
//     FILE *fp = fopen ("/dev/urandom", "r");
//     unsigned int foo;
//     struct timeval t;
// 
//     if (!fp)
//     {
//         gettimeofday (&t, NULL);
//         foo = t.tv_usec;
//     }
//     else
//     {
//         size_t res = fread (&foo, sizeof (foo), 1, fp);
//         if (res == 0 ) printf ("fread failed\n");
//         fclose (fp);
//     }
//     srand (foo);
// }
// 
// #define random_value(min_value, max_value)
//     (   ((double)(min_value))             
//       + ( ( (double)(max_value)-((double)(min_value)) )*rand() / (RAND_MAX+1.0f)) )

int main (int argc, char** argv)
{
    Socket client_socket ;

    Error_t error ;

    char message [MESSAGE_LENGTH] ;
// 
//     seed_random () ;

    /* Checks if all arguments are there **************************************/

    if (argc != 1)
    {
        fprintf (stderr, "Usage: \"%s \n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    init_socket (&client_socket) ;

    error = open_client_socket (&client_socket) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    fprintf (stdout, "done !\n") ;

    /* Connect to the server **************************************************/

    fprintf (stdout, "Connecting to server ... ") ; fflush (stdout) ;

    error = connect_client_to_server (&client_socket, "127.0.0.1", 10024) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    fprintf (stdout, "done !\n") ;

    /**************************************************************************/

    build_message_request (TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS, message) ;

    error = send_message_to_socket (&client_socket, message) ;

    if (error != TDICE_SUCCESS)    goto socket_error ;

    error =

        receive_message_from_socket (&client_socket, message, MESSAGE_LENGTH) ;

    if (error != TDICE_SUCCESS)    goto socket_error ;

    // FIXME this is just to check if it works ...

    fprintf (stdout, "There are %d floorplan elements in the stack\n", (int) message[4]) ;

    build_message_request (TDICE_EXIT_SIMULATION, message) ;

    error = send_message_to_socket (&client_socket, message) ;

    if (error != TDICE_SUCCESS)    goto socket_error ;

    /**************************************************************************/

    error = close_socket (&client_socket) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    return EXIT_SUCCESS ;

socket_error :

    close_socket (&client_socket) ;

    return EXIT_FAILURE ;
}
