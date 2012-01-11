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

#include "network_interface.h"

#define MESSAGE_LENGTH 32

int main (int argc, char** argv)
{
    Socket client_socket ;

    char message [MESSAGE_LENGTH] ;

    /* Checks if all arguments are there **************************************/

    if (argc != 1)
    {
        fprintf (stderr, "Usage: \"%s \n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    init_socket (&client_socket) ;

    if (open_client_socket (&client_socket) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    fprintf (stdout, "done !\n") ;

    /* Connect to the server **************************************************/

    fprintf (stdout, "Connecting to server ... ") ; fflush (stdout) ;

    if (connect_client_to_server (&client_socket, "127.0.0.1", 10024) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    fprintf (stdout, "done !\n") ;

    /**************************************************************************/

    fprintf (stdout, "Receiving from server ...") ; fflush (stdout) ;

    if (receive_from_socket (&client_socket, message, (StringLength_t) MESSAGE_LENGTH) != TDICE_SUCCESS)

        goto failure ;

    fprintf (stdout, " ->%s<- ", message) ;

    Quantity_t n_flp_el, n_slots;

    int read = sscanf (message, "%d %d", &n_flp_el, &n_slots) ;

    if (read != 2 || read == EOF)
    {
        fprintf (stderr, "Bad message formatting\n") ;

        goto failure ;
    }

    fprintf (stdout, "done! %d - %d\n", n_flp_el, n_slots) ;

    /**************************************************************************/

    if (close_socket (&client_socket) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    return EXIT_SUCCESS ;

failure :
                    close_socket (&client_socket) ;

                    return EXIT_FAILURE ;
}
