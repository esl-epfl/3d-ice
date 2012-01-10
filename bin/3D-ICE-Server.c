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
#include <arpa/inet.h>
#include <sys/socket.h>

#include "types.h"
#include "stack_description.h"
#include "thermal_data.h"
#include "analysis.h"

int main (int argc, char** argv)
{
    StackDescription stkd ;
    Analysis         analysis ;
    ThermalData      tdata ;

    NetworkSocket_t server_socket ;
    NetworkSocket_t client_socket ;

    struct sockaddr_in server_address ;
    struct sockaddr_in client_address ;

    socklen_t client_length ;

    char client_id [24] ;

    char message [125] ;

    Quantity_t n_flp_el ;

    ssize_t to_send, sent ;

    /* Checks if all arguments are there **************************************/

    if (argc != 2)
    {
        fprintf (stderr, "Usage: \"%s file.stk\n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    /* Parses stack file (fills stack descrition and analysis) ****************/

    fprintf (stdout, "Preparing stk data ... ") ; fflush (stdout) ;

    init_stack_description (&stkd) ;
    init_analysis          (&analysis) ;

    if (fill_stack_description (&stkd, &analysis, argv[1]) != 0)

        return EXIT_FAILURE ;

    if (analysis.AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        fprintf (stderr, "only transient analysis!\n") ;

        goto wrong_analysis_error ;
    }

    fprintf (stdout, "done !\n") ;

    /* Prepares thermal data **************************************************/

    fprintf (stdout, "Preparing thermal data ... ") ; fflush (stdout) ;

    init_thermal_data (&tdata) ;

    if (fill_thermal_data (&tdata, &stkd, &analysis) == TDICE_FAILURE)

        goto ftd_error ;

    fprintf (stdout, "done !\n") ;

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    server_socket = socket (AF_INET, SOCK_STREAM, 0) ;

    if (server_socket < 0)
    {
        perror ("ERROR :: server socket creation") ;

        goto socket_error ;
    }

    fprintf (stdout, "done !\n") ;

    /* Fills server address ***************************************************/

    memset ((void *) &server_address, 0, sizeof (struct sockaddr_in)) ;

    server_address.sin_family      = AF_INET ;
    server_address.sin_port        = htons (10024) ;
    server_address.sin_addr.s_addr = htonl (INADDR_ANY) ;

    /* Binds server address to the scket **************************************/

    fprintf (stdout, "Binding socket to address ... ") ; fflush (stdout) ;

    if (bind (server_socket,
              (struct sockaddr *) &server_address,
              sizeof (struct sockaddr_in)) < 0)
    {
        perror ("ERROR :: server bind") ;

        goto bind_error ;
    }

    fprintf (stdout, "done !\n") ;

    /* Listens for connections on the socket **********************************/

    fprintf (stdout, "Listening ... ") ; fflush (stdout) ;

    if (listen (server_socket, 1) < 0)
    {
        perror ("ERROR :: server listen") ;

        goto listen_error ;
    }

    fprintf (stdout, "done !\n") ;

    /**************************************************************************/

    fprintf (stdout, "Waiting for client ... ") ; fflush (stdout) ;

    client_length = sizeof (struct sockaddr_in) ;

    client_socket = accept (server_socket,
                            ( struct sockaddr *) &client_address,
                            &client_length) ;

    if (client_socket < 0)
    {
        perror ("ERROR :: server accept") ;

        goto accept_error ;
    }

    if (inet_ntop (AF_INET, &client_address.sin_addr, client_id, sizeof (client_id)) == NULL)
    {
        perror ("ERROR :: client identification") ;

        goto client_id_error ;
    }

    fprintf (stdout, "done (%s:%d)!\n", client_id, ntohs (client_address.sin_port)) ;

    /**************************************************************************/

    n_flp_el = get_total_number_of_floorplan_elements (&stkd) ;

    sprintf (message, "%d", n_flp_el) ;

    to_send = strlen (message) ;

    fprintf (stdout, "Sending >%s< (%Zu bytes) to client ... ", message, to_send) ; fflush (stdout) ;

    sent = write (client_socket, message, to_send) ;

    if (sent == -1)
    {
        perror ("ERROR :: write failed") ;

        goto write_error ;
    }

    if (sent == 0 || sent != to_send)
    {
        perror ("ERROR :: wrong write") ;

        goto write_error ;
    }

    fprintf (stdout, "done !\n") ;

    /**************************************************************************/

    close                  (client_socket) ;
    close                  (server_socket) ;
    free_thermal_data      (&tdata) ;
    free_analysis          (&analysis) ;
    free_stack_description (&stkd) ;

    return EXIT_SUCCESS ;

write_error :
                            close                  (client_socket) ;
client_id_error :
accept_error :
listen_error :
bind_error :
                            close                  (server_socket) ;
socket_error :
                            free_thermal_data      (&tdata) ;
ftd_error :
wrong_analysis_error :
                            free_analysis          (&analysis) ;
                            free_stack_description (&stkd) ;

                            return EXIT_FAILURE ;
}
