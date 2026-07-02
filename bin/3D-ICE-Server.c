/******************************************************************************
 * This file is part of 3D-ICE, version 4.0 .                                 *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Kai Zhu                     Luis Costero                          *
 *          Marina Zapater              David Atienza                         *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"
#include "network_socket.h"
#include "network_message.h"
#include "stack_file_parser.h"
#include "stack_description.h"
#include "thermal_data.h"
#include "analysis.h"
#include "output.h"
#include "powers_queue.h"

#define MAX_OUTPUT_FILES_TO_TRANSFER 1024u
#define TERMINATION_SENTINEL (-1.0f)
#define DEFAULT_POWER_TRACE_POLL_SECONDS 1u

typedef enum
{
    SERVER_MODE_SOCKET,
    SERVER_MODE_LOCAL_TRACE
} ServerMode_t ;

typedef struct
{
    ServerMode_t mode ;
    char        *stk_file ;
    Quantity_t   server_port ;
    char        *power_trace_file ;
    int          follow_power_trace ;
    int          terminate_on_sentinel ;
    unsigned int poll_seconds ;
} ServerOptions_t ;

static void insert_message_bytes
(
    NetworkMessage_t *message,
    unsigned char    *bytes,
    Quantity_t        nbytes
)
{
    Quantity_t index ;

    for (index = 0u ; index < nbytes ; index += sizeof (MessageWord_t))
    {
        MessageWord_t word = 0u ;
        Quantity_t remaining = nbytes - index ;
        Quantity_t chunk =
            remaining < sizeof (MessageWord_t) ? remaining : sizeof (MessageWord_t) ;

        memcpy (&word, bytes + index, chunk) ;

        insert_message_word (message, &word) ;
    }
}

static bool output_file_already_added
(
    String_t    file_name,
    String_t   added_files [],
    Quantity_t nfiles
)
{
    Quantity_t index ;

    for (index = 0u ; index != nfiles ; index++)
    {
        if (strcmp (file_name, added_files [index]) == 0)

            return true ;
    }

    return false ;
}

static Error_t append_file_to_message
(
    NetworkMessage_t *message,
    String_t          file_name,
    String_t          added_files [],
    Quantity_t       *nfiles
)
{
    if (file_name == NULL || file_name [0] == '\0')

        return TDICE_SUCCESS ;

    if (output_file_already_added (file_name, added_files, *nfiles) == true)

        return TDICE_SUCCESS ;

    if (*nfiles == MAX_OUTPUT_FILES_TO_TRANSFER)

        return TDICE_FAILURE ;

    FILE *file = fopen (file_name, "rb") ;

    if (file == NULL)
    {
        fprintf (stderr, "warning: cannot open output file %s for transfer\n", file_name) ;

        return TDICE_SUCCESS ;
    }

    if (fseek (file, 0L, SEEK_END) != 0)
    {
        fclose (file) ;

        return TDICE_FAILURE ;
    }

    long file_length_long = ftell (file) ;

    if (file_length_long < 0)
    {
        fclose (file) ;

        return TDICE_FAILURE ;
    }

    rewind (file) ;

    Quantity_t file_name_length = (Quantity_t) strlen (file_name) ;
    Quantity_t file_length      = (Quantity_t) file_length_long ;

    insert_message_word  (message, &file_name_length) ;
    insert_message_word  (message, &file_length) ;
    insert_message_bytes (message, (unsigned char *) file_name, file_name_length) ;

    if (file_length != 0u)
    {
        unsigned char *file_content = (unsigned char *) malloc (file_length) ;

        if (file_content == NULL)
        {
            fclose (file) ;

            return TDICE_FAILURE ;
        }

        size_t nread = fread (file_content, 1u, file_length, file) ;

        fclose (file) ;

        if (nread != file_length)
        {
            free (file_content) ;

            return TDICE_FAILURE ;
        }

        insert_message_bytes (message, file_content, file_length) ;

        free (file_content) ;
    }
    else
    {
        fclose (file) ;
    }

    added_files [*nfiles] = file_name ;
    (*nfiles)++ ;

    return TDICE_SUCCESS ;
}

static Error_t append_output_files_from_list
(
    NetworkMessage_t      *message,
    InspectionPointList_t *list,
    String_t               added_files [],
    Quantity_t            *nfiles
)
{
    InspectionPointListNode_t *ipn ;

    for (ipn  = inspection_point_list_begin (list) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        if (append_file_to_message
            (message, ipoint->FileName, added_files, nfiles) != TDICE_SUCCESS)

            return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

static Error_t build_output_files_message
(
    Output_t         *output,
    NetworkMessage_t *message
)
{
    Quantity_t nfiles = 0u ;
    String_t added_files [MAX_OUTPUT_FILES_TO_TRANSFER] ;

    build_message_head  (message, TDICE_SEND_OUTPUT_FILES) ;
    insert_message_word (message, &nfiles) ;

    if (append_output_files_from_list
        (message, &output->InspectionPointListFinal, added_files, &nfiles) != TDICE_SUCCESS)

        return TDICE_FAILURE ;

    if (append_output_files_from_list
        (message, &output->InspectionPointListSlot, added_files, &nfiles) != TDICE_SUCCESS)

        return TDICE_FAILURE ;

    if (append_output_files_from_list
        (message, &output->InspectionPointListStep, added_files, &nfiles) != TDICE_SUCCESS)

        return TDICE_FAILURE ;

    memcpy (message->Content, &nfiles, sizeof (MessageWord_t)) ;

    return TDICE_SUCCESS ;
}

static Error_t write_output_instant
(
    Output_t        *output,
    Dimensions_t    *dimensions,
    ThermalData_t   *tdata,
    Analysis_t      *analysis,
    bool            *headers,
    OutputInstant_t  instant
)
{
    Error_t error ;

    if (*headers == false)
    {
        error = generate_output_headers

            (output, dimensions, (String_t)"% ") ;

        if (error != TDICE_SUCCESS)
        {
            fprintf (stderr, "error in initializing output files \n ") ;

            return error ;
        }

        *headers = true ;
    }

    return generate_output

        (output, dimensions,
         tdata->Temperatures, tdata->PowerGrid.Sources,
         get_simulated_time (analysis), analysis->CurrentTime,
         analysis->SlotLength, instant) ;
}

static void print_usage (char *exe_name)
{
    fprintf (stderr, "Usage: \"%s file.stk server_port\"\n", exe_name) ;
    fprintf (stderr,
        "       \"%s file.stk --power-trace trace.txt --until-minus-one [--follow] [--poll seconds]\"\n",
        exe_name) ;
}

static int is_positive_integer (char *value)
{
    if (*value == '\0')

        return 0 ;

    for ( ; *value != '\0' ; value++)
    {
        if (*value < '0' || *value > '9')

            return 0 ;
    }

    return 1 ;
}

static void server_options_init (ServerOptions_t *options)
{
    options->mode                  = SERVER_MODE_SOCKET ;
    options->stk_file              = NULL ;
    options->server_port           = 0u ;
    options->power_trace_file      = NULL ;
    options->follow_power_trace    = 0 ;
    options->terminate_on_sentinel = 0 ;
    options->poll_seconds          = DEFAULT_POWER_TRACE_POLL_SECONDS ;
}

static Error_t parse_server_options
(
    int              argc,
    char           **argv,
    ServerOptions_t *options
)
{
    int arg_index ;

    server_options_init (options) ;

    if (argc < 3)
    {
        print_usage (argv [0]) ;

        return TDICE_FAILURE ;
    }

    options->stk_file = argv [1] ;

    if (argc == 3 && argv [2][0] != '-')
    {
        options->mode        = SERVER_MODE_SOCKET ;
        options->server_port = (Quantity_t) atoi (argv [2]) ;

        return TDICE_SUCCESS ;
    }

    options->mode = SERVER_MODE_LOCAL_TRACE ;

    for (arg_index = 2 ; arg_index != argc ; arg_index++)
    {
        if (strcmp (argv [arg_index], "--power-trace") == 0)
        {
            if (++arg_index == argc)
            {
                fprintf (stderr, "--power-trace requires a file path\n") ;
                print_usage (argv [0]) ;

                return TDICE_FAILURE ;
            }

            options->power_trace_file = argv [arg_index] ;
        }
        else if (strcmp (argv [arg_index], "--follow") == 0)
        {
            options->follow_power_trace = 1 ;
        }
        else if (strcmp (argv [arg_index], "--until-minus-one") == 0)
        {
            options->terminate_on_sentinel = 1 ;
        }
        else if (strcmp (argv [arg_index], "--poll") == 0)
        {
            if (++arg_index == argc || is_positive_integer (argv [arg_index]) == 0)
            {
                fprintf (stderr, "--poll requires a positive integer number of seconds\n") ;
                print_usage (argv [0]) ;

                return TDICE_FAILURE ;
            }

            options->poll_seconds = (unsigned int) atoi (argv [arg_index]) ;

            if (options->poll_seconds == 0u)
            {
                fprintf (stderr, "--poll requires a positive integer number of seconds\n") ;
                print_usage (argv [0]) ;

                return TDICE_FAILURE ;
            }
        }
        else
        {
            fprintf (stderr, "Unknown server option %s\n", argv [arg_index]) ;
            print_usage (argv [0]) ;

            return TDICE_FAILURE ;
        }
    }

    if (options->power_trace_file == NULL)
    {
        fprintf (stderr, "local server mode requires --power-trace\n") ;
        print_usage (argv [0]) ;

        return TDICE_FAILURE ;
    }

    if (options->terminate_on_sentinel == 0)
    {
        fprintf (stderr, "local server mode requires --until-minus-one\n") ;
        print_usage (argv [0]) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

static Error_t insert_power_values_from_array
(
    ThermalData_t *tdata,
    Quantity_t     nflpel,
    float         *powers
)
{
    PowersQueue_t queue ;
    Quantity_t    index ;
    Error_t       error ;

    powers_queue_init (&queue) ;
    powers_queue_build (&queue, nflpel) ;

    for (index = 0u ; index != nflpel ; index++)

        put_into_powers_queue (&queue, powers [index]) ;

    error = insert_power_values (&tdata->PowerGrid, &queue) ;

    powers_queue_destroy (&queue) ;

    return error ;
}

static SimResult_t simulate_slot_and_write_output
(
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output,
    ThermalData_t      *tdata,
    bool               *headers
)
{
    SimResult_t result = emulate_slot (tdata, stkd->Dimensions, analysis) ;

    if (result == TDICE_SLOT_DONE &&
        write_output_instant
        (output, stkd->Dimensions, tdata, analysis,
         headers, TDICE_OUTPUT_INSTANT_SLOT) != TDICE_SUCCESS)
    {
        fprintf (stderr, "error: generate slot output\n") ;

        result = TDICE_SOLVER_ERROR ;
    }

    return result ;
}

static Error_t read_power_trace_value
(
    FILE        *power_trace,
    int          follow_power_trace,
    Quantity_t   slot_index,
    Quantity_t   value_index,
    unsigned int poll_seconds,
    float       *power
)
{
    int waiting = 0 ;

    for ( ; ; )
    {
        int result = fscanf (power_trace, "%f", power) ;

        if (result == 1)

            return TDICE_SUCCESS ;

        if (result == EOF && follow_power_trace != 0)
        {
            if (waiting == 0)
            {
                fprintf (stdout,
                    "Waiting for power trace values for slot %u, element %u ...\n",
                    (unsigned int) slot_index,
                    (unsigned int) value_index) ;
                fflush (stdout) ;

                waiting = 1 ;
            }

            clearerr (power_trace) ;
            sleep (poll_seconds) ;

            continue ;
        }

        fprintf (stderr,
            "Cannot read power trace value for slot %u, element %u\n",
            (unsigned int) slot_index,
            (unsigned int) value_index) ;

        return TDICE_FAILURE ;
    }
}

static FILE *open_power_trace_file
(
    char        *power_trace_file,
    int          follow_power_trace,
    unsigned int poll_seconds
)
{
    FILE *power_trace ;
    int   waiting = 0 ;

    for ( ; ; )
    {
        power_trace = fopen (power_trace_file, "r") ;

        if (power_trace != NULL)

            return power_trace ;

        if (follow_power_trace == 0)
        {
            fprintf (stderr, "Cannot open power trace file %s\n", power_trace_file) ;

            return NULL ;
        }

        if (waiting == 0)
        {
            fprintf (stdout, "Waiting for power trace file %s ...\n", power_trace_file) ;
            fflush (stdout) ;

            waiting = 1 ;
        }

        sleep (poll_seconds) ;
    }
}

static int is_termination_slot
(
    float      *powers,
    Quantity_t  nflpel
)
{
    Quantity_t index ;

    for (index = 0u ; index != nflpel ; index++)
    {
        if (powers [index] != TERMINATION_SENTINEL)

            return 0 ;
    }

    return 1 ;
}

static void print_slot_progress
(
    Analysis_t  *analysis,
    Quantity_t  *slot_counter
)
{
    fprintf (stdout, "%.3f ", get_simulated_time (analysis)) ;

    fflush (stdout) ;

    if (++(*slot_counter) == 10u)
    {
        fprintf (stdout, "\n") ;

        *slot_counter = 0u ;
    }
}

static Error_t run_local_power_trace_mode
(
    ServerOptions_t    *options,
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output,
    ThermalData_t      *tdata,
    bool               *headers
)
{
    FILE       *power_trace ;
    float      *powers ;
    Quantity_t  nflpel ;
    Quantity_t  slot_index ;
    Quantity_t  slot_counter = 0u ;
    Quantity_t  index ;
    Error_t     error ;

    fprintf (stdout, "Running local power trace mode.\n") ;
    fflush (stdout) ;

    power_trace = open_power_trace_file
        (options->power_trace_file,
         options->follow_power_trace,
         options->poll_seconds) ;

    if (power_trace == NULL)

        return TDICE_FAILURE ;

    nflpel = get_total_number_of_floorplan_elements (stkd) ;

    if (nflpel == 0u)
    {
        fprintf (stderr, "error: stack has no floorplan elements\n") ;
        fclose (power_trace) ;

        return TDICE_FAILURE ;
    }

    powers = (float *) malloc (sizeof (float) * nflpel) ;

    if (powers == NULL)
    {
        fprintf (stderr, "error: cannot allocate local power slot\n") ;
        fclose (power_trace) ;

        return TDICE_FAILURE ;
    }

    for (slot_index = 0u ; ; slot_index++)
    {
        SimResult_t result ;

        for (index = 0u ; index != nflpel ; index++)
        {
            if (read_power_trace_value
                (power_trace,
                 options->follow_power_trace,
                 slot_index,
                 index,
                 options->poll_seconds,
                 &powers [index]) != TDICE_SUCCESS)
            {
                free (powers) ;
                fclose (power_trace) ;

                return TDICE_FAILURE ;
            }
        }

        if (options->terminate_on_sentinel != 0 &&
            is_termination_slot (powers, nflpel) != 0)
        {
            fprintf (stdout,
                "Received all-minus-one termination slot at slot %u; stopping simulation.\n",
                (unsigned int) slot_index) ;

            break ;
        }

        error = insert_power_values_from_array (tdata, nflpel, powers) ;

        if (error != TDICE_SUCCESS)
        {
            fprintf (stderr, "error: insert power values\n") ;
            free (powers) ;
            fclose (power_trace) ;

            return TDICE_FAILURE ;
        }

        result = simulate_slot_and_write_output (stkd, analysis, output, tdata, headers) ;

        if (result == TDICE_END_OF_SIMULATION)

            break ;

        if (result != TDICE_SLOT_DONE)
        {
            fprintf (stderr, "error %d: emulate slot\n", result) ;
            free (powers) ;
            fclose (power_trace) ;

            return TDICE_FAILURE ;
        }

        print_slot_progress (analysis, &slot_counter) ;
    }

    free (powers) ;
    fclose (power_trace) ;

    return TDICE_SUCCESS ;
}

int main (int argc, char** argv)
{
    StackDescription_t stkd ;
    Analysis_t         analysis ;
    Output_t           output ;
    ThermalData_t      tdata ;

    Error_t error ;

    ServerOptions_t options ;

    Quantity_t slot_counter = 0u ;

    Socket_t server_socket, client_socket ;

    NetworkMessage_t request, reply ;

    bool headers = false ;

    /* Checks if all arguments are there **************************************/

    if (parse_server_options (argc, argv, &options) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    /* Parses stack file (fills stack descrition and analysis) ****************/

    fprintf (stdout, "Preparing stk data ... ") ; fflush (stdout) ;

    stack_description_init (&stkd) ;
    analysis_init          (&analysis) ;
    output_init            (&output) ;

    error = parse_stack_description_file (options.stk_file, &stkd, &analysis, &output) ;

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

        (&tdata, &stkd.StackElements, stkd.Dimensions, &analysis, &stkd.Materials) ;

    if (error != TDICE_SUCCESS)    goto ftd_error ;

    fprintf (stdout, "done !\n") ;

    if (options.mode == SERVER_MODE_LOCAL_TRACE)
    {
        error = run_local_power_trace_mode
            (&options, &stkd, &analysis, &output, &tdata, &headers) ;

        thermal_data_destroy      (&tdata) ;
        stack_description_destroy (&stkd) ;
        output_destroy            (&output) ;

        return error == TDICE_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE ;
    }

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    socket_init (&server_socket) ;

    error = open_server_socket (&server_socket, options.server_port) ;

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

        switch (*request.MType)
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
                     get_simulated_time (&analysis), analysis.CurrentTime, 
                     analysis.SlotLength, instant) ;

                break ;
            }

        /**********************************************************************/

            case TDICE_SEND_OUTPUT_FILES :
            {
                network_message_init (&reply) ;

                if (build_output_files_message (&output, &reply) != TDICE_SUCCESS)
                {
                    fprintf (stderr, "error: build output files message\n") ;

                    network_message_destroy (&reply) ;

                    goto sim_error ;
                }

                send_message_to_socket (&client_socket, &reply) ;

                network_message_destroy (&reply) ;

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
