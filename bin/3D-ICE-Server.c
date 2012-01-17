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
#include "network_message.h"
#include "stack_description.h"
#include "thermal_data.h"
#include "analysis.h"
#include "powers_queue.h"

int main (int argc, char** argv)
{
    StackDescription stkd ;
    Analysis         analysis ;
    ThermalData      tdata ;

    Error_t error ;

    Socket server_socket, client_socket ;

    NetworkMessage message ;

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

    /* Generates output headers ***********************************************/

    error = generate_analysis_headers (&analysis, stkd.Dimensions, "% ") ;

    if (error != TDICE_SUCCESS)
    {
        fprintf (stderr, "error in initializing output files \n") ;

        goto gah_error ;
    }

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

    init_network_message (&message) ;

    do
    {
        error = receive_message_from_socket (&client_socket, &message) ;

        if (error != TDICE_SUCCESS)
        {
            fprintf (stderr, "error: receive message from socket\n") ;

            goto transmission_error ;
        }

        switch (*message.Type)
        {
            case TDICE_EXIT_SIMULATION :
            {
                goto quit ;
            }
            case TDICE_RESET_THERMAL_STATE :
            {
                reset_thermal_state (&tdata, &analysis) ;

                break ;
            }
            case TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS :
            {
                build_message_head

                    (&message, TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS) ;

                Quantity_t nflpel = get_total_number_of_floorplan_elements (&stkd) ;

                error = insert_message_word (&message, &nflpel) ;

                if (error != TDICE_SUCCESS)
                {
                    fprintf (stderr, "error: insert message word\n") ;

                    goto message_error ;
                }

                error = send_message_to_socket (&client_socket, &message) ;

                if (error != TDICE_SUCCESS)
                {
                    fprintf (stderr, "error: send message to socket\n") ;

                    goto message_error ;
                }

                break ;
            }
            case TDICE_INSERT_POWERS_AND_SIMULATE_SLOT :
            {
                Quantity_t nflpel, index ;

                PowersQueue queue ;

                init_powers_queue (&queue) ;

                error = extract_message_word (&message, &nflpel, 0) ;

                if (error != TDICE_SUCCESS)
                {
                    fprintf (stderr, "error: extract message word 0\n") ;

                    goto message_error ;
                }

                for (index = 1, nflpel++ ; index != nflpel ; index++)
                {
                    float power_value ;

                    error = extract_message_word (&message, &power_value, index) ;

                    if (error != TDICE_SUCCESS)
                    {
                        fprintf (stderr, "error: extract message word %d\n", index) ;

                        goto message_error ;
                    }

                    put_into_powers_queue (&queue, power_value) ;
                }

                error = insert_power_values (&stkd, &queue) ;

                if (error != TDICE_SUCCESS)
                {
                    fprintf (stderr, "error: insert power values\n") ;

                    goto message_error ;
                }

                SimResult_t result = emulate_slot (&tdata, &stkd, &analysis) ;

                if (result != TDICE_SLOT_DONE)
                {
                    fprintf (stderr, "error %d: emulate slot\n", result) ;

                    goto sim_error ;
                }

                generate_analysis_output

                    (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_SLOT) ;

                break ;
            }
            default :

                fprintf (stderr, "ERROR :: received unknown message type") ;
        }

    } while (1) ;

    /**************************************************************************/

quit :

    close_socket           (&client_socket) ;
    close_socket           (&server_socket) ;
    free_thermal_data      (&tdata) ;
    free_analysis          (&analysis) ;
    free_stack_description (&stkd) ;

    return EXIT_SUCCESS ;

sim_error :
message_error:
transmission_error :
                            close_socket           (&client_socket) ;
wait_error :
                            close_socket           (&server_socket) ;
socket_error :
                            free_thermal_data      (&tdata) ;
ftd_error :
gah_error :
wrong_analysis_error :
                            free_analysis          (&analysis) ;
                            free_stack_description (&stkd) ;

                            return EXIT_FAILURE ;
}
