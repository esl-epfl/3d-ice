/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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
#include <sys/time.h>

#include "network_socket.h"
#include "network_message.h"

void seed_random ()
{
    FILE *fp = fopen ("/dev/urandom", "r");
    unsigned int foo;
    struct timeval t;

    if (!fp)
    {
        gettimeofday (&t, NULL);
        foo = t.tv_usec;
    }
    else
    {
        size_t res = fread (&foo, sizeof (foo), 1, fp);
        if (res == 0 ) printf ("fread failed\n");
        fclose (fp);
    }
    srand (foo);
}

#define random_value(min_value, max_value) \
    (   ((double)(min_value))              \
      + ( ( (double)(max_value)-((double)(min_value)) )*rand() / (RAND_MAX+1.0f)) )

#define MAX_SERVER_IP 50

int main (int argc, char** argv)
{
    Socket_t client_socket ;

    NetworkMessage_t client_nflp, client_powers, client_temperatures, client_cores ;
    NetworkMessage_t client_tmap, client_close_sim, server_reply ;

    Quantity_t nflpel, index, index2, nslots, nresults, server_port ;

    char server_ip [MAX_SERVER_IP] ;

    SimResult_t sim_result ;

    OutputInstant_t  instant ;
    OutputType_t     type ;
    OutputQuantity_t quantity ;

    CellIndex_t row, column ;
    CellIndex_t nrows, ncolumns ;

    float power, time, temperature ;

    FILE *tmap ;

    seed_random () ;

    /* Checks if all arguments are there **************************************/

    if (argc != 4)
    {
        fprintf (stderr, "Usage: \"%s nslots server_ip server_port\n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    nslots = atoi (argv[1]) ;

    if (strlen (argv[2]) > MAX_SERVER_IP - 1)
    {
        fprintf (stderr, "Server ip %s too long !!!\n", argv[2]) ;

        return EXIT_FAILURE ;
    }

    strcpy (server_ip, argv[2]) ;

    server_port = atoi (argv[3]) ;

    /* Creates socket *********************************************************/

    fprintf (stdout, "Creating socket ... ") ; fflush (stdout) ;

    socket_init (&client_socket) ;

    if (open_client_socket (&client_socket) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    fprintf (stdout, "done !\n") ;

    /* Connect to the server **************************************************/

    fprintf (stdout, "Connecting to server ... ") ; fflush (stdout) ;

    if (connect_client_to_server (&client_socket, (String_t)server_ip, server_port) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    fprintf (stdout, "done !\n") ;

    /* Creates file for themal maps *******************************************/

    tmap = fopen ("thermal_map.txt", "w") ;

    if (tmap == NULL)

        return EXIT_FAILURE ;

    /* Client-Server Communication ********************************************/
    /**************************************************************************/

    network_message_init (&client_nflp) ;
    build_message_head   (&client_nflp, TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS) ;

    send_message_to_socket      (&client_socket, &client_nflp) ;
    receive_message_from_socket (&client_socket, &client_nflp) ;

    extract_message_word (&client_nflp, &nflpel, 0) ;

    network_message_destroy (&client_nflp) ;

    for ( ; nslots != 0 ; nslots--)
    {
        /* client sends power values ******************************************/

        network_message_init (&client_powers) ;
        build_message_head   (&client_powers, TDICE_INSERT_POWERS_AND_SIMULATE_SLOT) ;
        insert_message_word  (&client_powers, &nflpel) ;

        for (index = 0 ; index != nflpel ; index++)
        {
            power = random_value (0.0, 1.0) ;

            insert_message_word (&client_powers, &power) ;
        }

        send_message_to_socket (&client_socket, &client_powers) ;

        network_message_destroy (&client_powers) ;

        /* Client waits for simulation result *********************************/

        network_message_init (&server_reply) ;

        receive_message_from_socket (&client_socket, &server_reply) ;

        extract_message_word (&server_reply, &sim_result, 0) ;

        if (sim_result != TDICE_SLOT_DONE)
        {
            network_message_destroy (&server_reply) ;

            socket_close (&client_socket) ;

            return EXIT_FAILURE ;
        }

        network_message_destroy (&server_reply) ;

        /* Client sends temperatures request for thermal sensors **************/

        instant  = TDICE_OUTPUT_INSTANT_SLOT ;
        type     = TDICE_OUTPUT_TYPE_TCELL ;
        quantity = TDICE_OUTPUT_QUANTITY_NONE ;

        network_message_init (&client_temperatures) ;
        build_message_head   (&client_temperatures, TDICE_SEND_OUTPUT) ;
        insert_message_word  (&client_temperatures, &instant) ;
        insert_message_word  (&client_temperatures, &type) ;
        insert_message_word  (&client_temperatures, &quantity) ;

        send_message_to_socket (&client_socket, &client_temperatures) ;

        network_message_destroy (&client_temperatures) ;

        /* Client receives temperatures ***************************************/

        network_message_init (&server_reply) ;

        receive_message_from_socket (&client_socket, &server_reply) ;

        extract_message_word (&server_reply, &time,     0) ;
        extract_message_word (&server_reply, &nresults, 1) ;

        fprintf (stdout, "%5.2f sec : \t", time) ;

        for (index = 2, nresults += 2 ; index != nresults ; index++)
        {
            extract_message_word (&server_reply, &temperature, index) ;

            fprintf (stdout, "%5.2f K \t", temperature) ;
        }

        network_message_destroy (&server_reply) ;

        /* Client sends thermal maps request **********************************/

        instant  = TDICE_OUTPUT_INSTANT_SLOT ;
        type     = TDICE_OUTPUT_TYPE_TMAP ;
        quantity = TDICE_OUTPUT_QUANTITY_NONE ;

        network_message_init (&client_tmap) ;
        build_message_head   (&client_tmap, TDICE_SEND_OUTPUT) ;
        insert_message_word  (&client_tmap, &instant) ;
        insert_message_word  (&client_tmap, &type) ;
        insert_message_word  (&client_tmap, &quantity) ;

        send_message_to_socket (&client_socket, &client_tmap) ;

        network_message_destroy (&client_tmap) ;

        /* Client receives thermal maps ***************************************/

        network_message_init (&server_reply) ;

        receive_message_from_socket (&client_socket, &server_reply) ;

        extract_message_word (&server_reply, &time,     0) ;
        extract_message_word (&server_reply, &nresults, 1) ;
        extract_message_word (&server_reply, &nrows,    2) ;
        extract_message_word (&server_reply, &ncolumns, 3) ;

        for (index = 4, index2 = 0 ; index2 != nresults ; index2++)
        {
            for (row = 0 ; row != nrows ; row++)
            {
                for (column = 0 ; column != ncolumns ; column++, index++)
                {
                    extract_message_word (&server_reply, &temperature, index) ;

                    fprintf (tmap, "%5.2f ", temperature) ;
                }
                fprintf (tmap, "\n") ;
            }
            fprintf (tmap, "\n") ;
        }

        network_message_destroy (&server_reply) ;

        /* Client sends temperatures request for cores ************************/

        instant  = TDICE_OUTPUT_INSTANT_SLOT ;
        type     = TDICE_OUTPUT_TYPE_TFLPEL ;
        quantity = TDICE_OUTPUT_QUANTITY_AVERAGE ;

        network_message_init (&client_cores) ;
        build_message_head   (&client_cores, TDICE_SEND_OUTPUT) ;
        insert_message_word  (&client_cores, &instant) ;
        insert_message_word  (&client_cores, &type) ;
        insert_message_word  (&client_cores, &quantity) ;

        send_message_to_socket (&client_socket, &client_cores) ;

        network_message_destroy (&client_cores) ;

        /* Client receives cores temperatures  ********************************/

        network_message_init (&server_reply) ;

        receive_message_from_socket (&client_socket, &server_reply) ;

        extract_message_word (&server_reply, &time,     0) ;
        extract_message_word (&server_reply, &nresults, 1) ;

        for (index = 2, nresults += 2 ; index != nresults ; index++)
        {
            extract_message_word (&server_reply, &temperature, index) ;

            fprintf (stdout, "%5.2f K \t", temperature) ;
        }

        fprintf (stdout, "\n") ;

        network_message_destroy (&server_reply) ;
    }

    fclose (tmap) ;

    /* Closes the simulation on the server ************************************/

    network_message_init (&client_close_sim) ;
    build_message_head   (&client_close_sim, TDICE_EXIT_SIMULATION) ;

    send_message_to_socket (&client_socket, &client_close_sim) ;

    network_message_destroy (&client_close_sim) ;

    /* Closes client sockek ***************************************************/

    if (socket_close (&client_socket) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    return EXIT_SUCCESS ;
}
