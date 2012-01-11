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

    Socket server_socket, client_socket ;

    char message [MESSAGE_LENGTH] ;

    Quantity_t n_flp_el ;

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

    /* Generates output headres ***********************************************/

    if (generate_analysis_headers (&analysis, stkd.Dimensions, "% ") != TDICE_SUCCESS)
    {
        fprintf (stderr, "error in initializing output files \n ");

        goto header_error ;
    }

    /* Prepares thermal data **************************************************/

    fprintf (stdout, "Preparing thermal data ... ") ; fflush (stdout) ;

    init_thermal_data (&tdata) ;

    if (fill_thermal_data (&tdata, &stkd, &analysis) == TDICE_FAILURE)

        goto ftd_error ;

    fprintf (stdout, "done !\n") ;

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    init_socket (&server_socket) ;

    if (open_server_socket (&server_socket, 10024) != TDICE_SUCCESS)

        goto socket_error ;

    fprintf (stdout, "done !\n") ;

    /* Waits for a client to connect ******************************************/

    fprintf (stdout, "Waiting for client ... ") ; fflush (stdout) ;

    init_socket (&client_socket) ;

    if (wait_for_client (&server_socket, &client_socket) != TDICE_SUCCESS)

        goto wait_error ;

    fprintf (stdout, "done !\n") ;

    /* Sends data to client ***************************************************/

    fprintf (stdout, "Sending nflpel to client ... ") ; fflush (stdout) ;

    n_flp_el = get_total_number_of_floorplan_elements (&stkd) ;

    sprintf (message, "%d %d", n_flp_el, NSLOTS) ;

    fprintf (stdout, " ->%s<- ", message) ;

    // here sizeof(message) is the dimension of the vector (i.e. the value
    // used in the declaration while strlen(message) is the number of
    // characters (bytes) before the '\0'.

    if (send_message_to_socket (&client_socket, message) != TDICE_SUCCESS)

        goto send_error ;

    fprintf (stdout, "done !\n") ;

    /* Receives all the power values from the client **************************/

    Quantity_t power_c, slot_c ;

    PowersQueue queue ;

    init_powers_queue (&queue) ;

    for (slot_c = 0 ; slot_c < NSLOTS ; slot_c++)
    {
        for (power_c = 0 ; power_c < n_flp_el ; power_c++)
        {
            if (receive_message_from_socket (&client_socket, message, (StringLength_t) MESSAGE_LENGTH) != TDICE_SUCCESS)

                goto receive_error ;

            Power_t tmp ;

            if (sscanf (message, "%lf", &tmp) != 1)
            {
                fprintf (stderr, "Bad message formatting\n") ;

                goto receive_error ;
            }

            put_into_powers_queue (&queue, tmp) ;
        }

        if (   insert_power_values   (&stkd, &queue) != TDICE_SUCCESS
            || is_empty_powers_queue (&queue)        == false)
        {
            fprintf (stderr, "Received wrong number of power values at slot %d\n", slot_c) ;

            goto receive_error ;
        }
    }

    /* Runs the simlation and generates output ********************************/

    SimResult_t result ;

    do
    {
        result = emulate_step (&tdata, &stkd, &analysis) ;

        if (result == TDICE_STEP_DONE || result == TDICE_SLOT_DONE)
        {
            fprintf (stdout, "%.3f ", get_simulated_time (&analysis)) ;

            fflush (stdout) ;

            generate_analysis_output

                (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_STEP) ;
        }

        if (result == TDICE_SLOT_DONE)
        {
            fprintf (stdout, "\n") ;

            generate_analysis_output

                (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_SLOT) ;
        }

    } while (result != TDICE_END_OF_SIMULATION) ;

    generate_analysis_output

        (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_FINAL) ;


    close_socket           (&client_socket) ;
    close_socket           (&server_socket) ;
    free_thermal_data      (&tdata) ;
    free_analysis          (&analysis) ;
    free_stack_description (&stkd) ;

    return EXIT_SUCCESS ;

receive_error :
send_error :
                            close_socket           (&client_socket) ;
wait_error :
                            close_socket           (&server_socket) ;
socket_error :
                            free_thermal_data      (&tdata) ;
header_error :
ftd_error :
wrong_analysis_error :
                            free_analysis          (&analysis) ;
                            free_stack_description (&stkd) ;

                            return EXIT_FAILURE ;
}
