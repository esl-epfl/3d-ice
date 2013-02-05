/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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
#include "network_socket.h"
#include "network_message.h"
#include "stack_file_parser.h"
#include "stack_description.h"
#include "thermal_data.h"
#include "analysis.h"
#include "output.h"
#include "powers_queue.h"

int main (int argc, char** argv)
{
    StackDescription_t stkd ;
    Analysis_t         analysis ;
    Output_t           output ;
    ThermalData_t      tdata ;

    Error_t error ;

    Quantity_t server_port, slot_counter = 0u ;

    Socket_t server_socket, client_socket ;

    NetworkMessage_t request, reply ;

    bool headers = false ;

    /* Checks if all arguments are there **************************************/

    if (argc != 3)
    {
        fprintf (stderr, "Usage: \"%s file.stk server_port\n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    server_port = atoi (argv[2]) ;

    /* Parses stack file (fills stack descrition and analysis) ****************/

    fprintf (stdout, "Preparing stk data ... ") ; fflush (stdout) ;

    stack_description_init (&stkd) ;
    analysis_init          (&analysis) ;
    output_init            (&output) ;

    error = parse_stack_description_file (argv[1], &stkd, &analysis, &output) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    if (analysis.AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        fprintf (stderr, "only transient analysis!\n") ;

        goto wrong_analysis_error ;
    }

    fprintf (stdout, "done !\n") ;

    /* Prepares thermal data **************************************************/

    fprintf (stdout, "Preparing thermal data ... ") ; fflush (stdout) ;

    thermal_data_init (&tdata) ;

    error = thermal_data_build

        (&tdata, &stkd.StackElements, stkd.Dimensions, &analysis) ;

    if (error != TDICE_SUCCESS)    goto ftd_error ;

    fprintf (stdout, "done !\n") ;

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    socket_init (&server_socket) ;

    error = open_server_socket (&server_socket, server_port) ;

    if (error != TDICE_SUCCESS)    goto socket_error ;

    fprintf (stdout, "done !\n") ;

    /* Waits for a client to connect ******************************************/

    fprintf (stdout, "Waiting for client ... ") ; fflush (stdout) ;

    socket_init (&client_socket) ;

    error = wait_for_client (&server_socket, &client_socket) ;

    if (error != TDICE_SUCCESS)    goto wait_error ;

    fprintf (stdout, "done !\n") ;

    /* Runs the simlation *****************************************************/

    do
    {
        network_message_init (&request) ;

        receive_message_from_socket (&client_socket, &request) ;

        switch (*request.Type)
        {

        /**********************************************************************/

            case TDICE_EXIT_SIMULATION :
            {
                network_message_destroy (&request) ;

                goto quit ;
            }

        /**********************************************************************/

            case TDICE_RESET_THERMAL_STATE :
            {
                reset_thermal_state (&tdata, &analysis) ;

                break ;
            }

        /**********************************************************************/

            case TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS :
            {
                network_message_init (&reply) ;

                build_message_head   (&reply, TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS) ;

                Quantity_t nflpel = get_total_number_of_floorplan_elements (&stkd) ;

                insert_message_word (&reply, &nflpel) ;

                send_message_to_socket (&client_socket, &reply) ;

                network_message_destroy (&reply) ;

                break ;
            }

        /**********************************************************************/

            case TDICE_INSERT_POWERS :
            {
                Quantity_t nflpel, index ;

                PowersQueue_t queue ;

                powers_queue_init (&queue) ;

                extract_message_word (&request, &nflpel, 0) ;

                powers_queue_build (&queue, nflpel) ;

                for (index = 1, nflpel++ ; index != nflpel ; index++)
                {
                    float power_value ;

                    extract_message_word (&request, &power_value, index) ;

                    put_into_powers_queue (&queue, power_value) ;
                }

                error = insert_power_values (&tdata.PowerGrid, &queue) ;

                if (error != TDICE_SUCCESS)
                {
                    fprintf (stderr, "error: insert power values\n") ;

                    powers_queue_destroy (&queue) ;

                    goto sim_error ;
                }

                powers_queue_destroy (&queue) ;

                network_message_init (&reply) ;
                build_message_head   (&reply, TDICE_INSERT_POWERS) ;
                insert_message_word (&reply, &error) ;

                send_message_to_socket (&client_socket, &reply) ;

                network_message_destroy (&reply) ;

                break ;
            }

        /**********************************************************************/

            case TDICE_SEND_OUTPUT :
            {
                OutputInstant_t  instant ;
                OutputType_t     type ;
                OutputQuantity_t quantity ;

                extract_message_word (&request, &instant,  0) ;
                extract_message_word (&request, &type,     1) ;
                extract_message_word (&request, &quantity, 2) ;

                network_message_init (&reply) ;
                build_message_head   (&reply, TDICE_SEND_OUTPUT) ;

                float   time = get_simulated_time (&analysis) ;
                Quantity_t n = get_number_of_inspection_points (&output, instant, type, quantity) ;

                insert_message_word (&reply, &time) ;
                insert_message_word (&reply, &n) ;

                if (n > 0)
                {
                    error = fill_output_message

                        (&output, stkd.Dimensions,
                         tdata.Temperatures, tdata.PowerGrid.Sources,
                         instant, type, quantity, &reply) ;

                    if (error != TDICE_SUCCESS)
                    {
                        fprintf (stderr, "error: generate message content\n") ;

                        network_message_destroy (&reply) ;

                        goto sim_error ;
                    }
                }

                send_message_to_socket (&client_socket, &reply) ;

                network_message_destroy (&reply) ;

                break ;
            }

        /**********************************************************************/

            case TDICE_PRINT_OUTPUT :
            {
                OutputInstant_t  instant ;

                extract_message_word (&request, &instant,  0) ;

                if (headers == false)
                {
                    Error_t error = generate_output_headers

                        (&output, stkd.Dimensions, (String_t)"% ") ;

                    if (error != TDICE_SUCCESS)
                    {
                        fprintf (stderr, "error in initializing output files \n ");

                        goto sim_error ;
                    }

                    headers = true ;
                }

                generate_output

                    (&output, stkd.Dimensions,
                     tdata.Temperatures, tdata.PowerGrid.Sources,
                     get_simulated_time (&analysis), instant) ;

                break ;
            }

        /**********************************************************************/

            case TDICE_SIMULATE_SLOT :
            {
                network_message_init (&reply) ;
                build_message_head   (&reply, TDICE_SIMULATE_SLOT) ;

                SimResult_t result = emulate_slot (&tdata, stkd.Dimensions, &analysis) ;

                insert_message_word (&reply, &result) ;

                send_message_to_socket (&client_socket, &reply) ;

                network_message_destroy (&reply) ;

                if (result == TDICE_END_OF_SIMULATION)
                {
                    network_message_destroy (&request) ;

                    goto quit ;
                }
                else if (result != TDICE_SLOT_DONE)
                {
                    fprintf (stderr, "error %d: emulate slot\n", result) ;

                    goto sim_error ;
                }

                fprintf (stdout, "%.3f ", get_simulated_time (&analysis)) ;

                fflush (stdout) ;

                if (++slot_counter == 10)
                {
                    fprintf (stdout, "\n") ;

                    slot_counter = 0 ;
                }

                break ;
            }

        /**********************************************************************/

            case TDICE_SIMULATE_STEP :
            {
                network_message_init (&reply) ;
                build_message_head   (&reply, TDICE_SIMULATE_STEP) ;

                SimResult_t result = emulate_step (&tdata, stkd.Dimensions, &analysis) ;

                insert_message_word (&reply, &result) ;

                send_message_to_socket (&client_socket, &reply) ;

                network_message_destroy (&reply) ;

                if (result == TDICE_END_OF_SIMULATION)
                {
                    network_message_destroy (&request) ;

                    goto quit ;
                }
                else if (result != TDICE_STEP_DONE && result != TDICE_SLOT_DONE)
                {
                    fprintf (stderr, "error %d: emulate step\n", result) ;

                    goto sim_error ;
                }

                fprintf (stdout, "%.3f ", get_simulated_time (&analysis)) ;

                fflush (stdout) ;

                if (slot_completed (&analysis))

                    fprintf (stdout, "\n") ;

                break ;
            }

        /**********************************************************************/

            default :

                fprintf (stderr, "ERROR :: received unknown message type") ;
        }

        network_message_destroy (&request) ;

    } while (1) ;

    /**************************************************************************/

quit :

    socket_close              (&client_socket) ;
    socket_close              (&server_socket) ;
    thermal_data_destroy      (&tdata) ;
    stack_description_destroy (&stkd) ;
    output_destroy            (&output) ;

    return EXIT_SUCCESS ;

sim_error :
                            network_message_destroy   (&request) ;
                            socket_close              (&client_socket) ;
wait_error :
                            socket_close              (&server_socket) ;
socket_error :
                            thermal_data_destroy      (&tdata) ;
ftd_error :
wrong_analysis_error :
                            stack_description_destroy (&stkd) ;
                            output_destroy            (&output) ;

                            return EXIT_FAILURE ;
}
