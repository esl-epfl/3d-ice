/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdlib.h>
#include <time.h>

#include "stack_description.h"
#include "analysis.h"
#include "thermal_data.h"

#include "network_interface_server.h"
#include "NIPreamble.h"

int
main(int argc, char** argv)
{
  StackDescription stkd ;
  Analysis         analysis ;
  ThermalData      tdata ;

  int sockfd;
  Time_t slot_time;
  Time_t step_time;
  Bool_t is_transient;
  int (*emulate)(ThermalData*, StackDescription*, Analysis*) = NULL;

  MessagesQueue*     mqueue;
  TemperaturesQueue* tqueue;

  // Checks if there are the all the arguments
  ////////////////////////////////////////////////////////////////////////////

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]) ;
    return EXIT_FAILURE ;
  }

  // initlization for the communication
#if INTERNET_DOMAIN
  sockfd = init_server_internet_domain(PORTNO);
#else
  sockfd = init_server_unix_domain(SOCKET_NAME);
#endif
  get_client_info(sockfd, &slot_time, &step_time, &is_transient);
  /*printf("%f, %f, %d\n", slot_time, step_time, is_transient);*/

  if (is_transient)
    emulate = &emulate_step;
  else
    emulate = &emulate_slot;

  // Init StackDescription and parse the input file
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing stk data ...\n") ;

  init_stack_description (&stkd) ;

  init_analysis (&analysis) ;

  if (fill_stack_description (&stkd, &analysis, argv[1]) != 0)

    return EXIT_FAILURE ;

  // Init thermal data and fill it using the StackDescription
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing thermal data ...\n") ;

  init_thermal_data (&tdata) ;

  if (fill_thermal_data (&tdata, &stkd, &analysis) != 0)
  {
    fprintf(stderr, "fill thermal data failed\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  if ( sockfd < 0)
    printf("error initilizing network interface\n");
  mqueue = alloc_and_init_messages_queue();
  tqueue = alloc_and_init_temperatures_queue();

  init_power_values (&stkd); // initilize the power values got from a stack description file
    // FIXME is it really needed ???

  // this is used for terminating the simulation
  Bool_t * is_terminating = malloc(sizeof(Bool_t));

  Quantity_t num_floorplan_elements = get_total_number_of_floorplan_elements(&stkd);
  Power_t* pvalues = malloc(num_floorplan_elements * sizeof(Power_t));

  // get network messages
  get_messages(sockfd, mqueue);

  do {

    if (slot_completed(&analysis)) {  // CHECKME
      pvalues = get_power_values_from_network(
        sockfd,
        pvalues,
        num_floorplan_elements,
        is_terminating
        );

      if (insert_power_values (&stkd, pvalues) == FALSE_V)
      {
        printf("error inserting dynamic power values");
        break;
      }
    }

    // process network messages
    process_messages(mqueue, tqueue, &stkd, &tdata);

    // send results
    send_results(sockfd, tqueue);

  } while (*is_terminating == FALSE_V && (*emulate)(&tdata, &stkd, &analysis) != 1) ;

  // close server
  close_server(sockfd);

  // free all data
  ////////////////////////////////////////////////////////////////////////////
  free_messages_queue    (mqueue) ;
  free_temperatures_queue(tqueue) ;
  free_thermal_data      (&tdata) ;
  free_stack_description (&stkd) ;
  free(pvalues);

  return EXIT_SUCCESS ;
}
