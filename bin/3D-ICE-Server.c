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

#include "types.h"
#include "network_interface.h"
#include "stack_description.h"
#include "thermal_data.h"
#include "analysis.h"
#include "powers_queue.h"

#define NSLOTS 10
#define MESSAGE_LENGTH 32

int main (int argc, char** argv)
{
    StackDescription stkd ;
    Analysis         analysis ;
    ThermalData      tdata ;

    Error_t error ;

    Socket server_socket, client_socket ;

    char message [MESSAGE_LENGTH] ;

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

    error = fill_stack_description (&stkd, &analysis, argv[1]) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    if (analysis.AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        fprintf (stderr, "only transient analysis!\n") ;

        goto wrong_analysis_error ;
    }

    fprintf (stdout, "done !\n") ;

    /* Prepares thermal data **************************************************/

    fprintf (stdout, "Preparing thermal data ... ") ; fflush (stdout) ;

    init_thermal_data (&tdata) ;

    error = fill_thermal_data (&tdata, &stkd, &analysis) ;

    if (error != TDICE_SUCCESS)    goto ftd_error ;

    fprintf (stdout, "done !\n") ;

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    init_socket (&server_socket) ;

    error = open_server_socket (&server_socket, 10024) ;

    if (error != TDICE_SUCCESS)    goto socket_error ;

    fprintf (stdout, "done !\n") ;

    /* Waits for a client to connect ******************************************/

    fprintf (stdout, "Waiting for client ... ") ; fflush (stdout) ;

    init_socket (&client_socket) ;

    error = wait_for_client (&server_socket, &client_socket) ;

    if (error != TDICE_SUCCESS)    goto wait_error ;

    fprintf (stdout, "done !\n") ;

    /* Runs the simlation *****************************************************/

    MessageType_t type ;

    do
    {

        error = receive_message_from_socket

            (&client_socket, message, MESSAGE_LENGTH) ;

        if (error != TDICE_SUCCESS)    goto transmission_error ;

        error = get_message_type (message, &type) ;

        if  (error != TDICE_SUCCESS)   goto transmission_error ;

        switch (type)
        {
            case TDICE_EXIT_SIMULATION :

                goto quit ;

            case TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS :

                build_message_reply

                    (TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS,
                    message,
                    get_total_number_of_floorplan_elements (&stkd)) ;

                break ;

            default :

                fprintf (stderr, "ERROR :: received unknown message type") ;
        }

        error = send_message_to_socket (&client_socket, message) ;

        if (error != TDICE_SUCCESS)    goto transmission_error ;

    } while (1) ;

    /**************************************************************************/

quit :

    close_socket           (&client_socket) ;
    close_socket           (&server_socket) ;
    free_thermal_data      (&tdata) ;
    free_analysis          (&analysis) ;
    free_stack_description (&stkd) ;

    return EXIT_SUCCESS ;

transmission_error :
                            close_socket           (&client_socket) ;
wait_error :
                            close_socket           (&server_socket) ;
socket_error :
                            free_thermal_data      (&tdata) ;
ftd_error :
wrong_analysis_error :
                            free_analysis          (&analysis) ;
                            free_stack_description (&stkd) ;

                            return EXIT_FAILURE ;
}
