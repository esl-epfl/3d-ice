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

#include "network_interface.h"
#include "NIPreamble.h"

int main(int argc, char** argv)
{
  int i;

#if DYNAMIC_POWER_VALUES
  srand(time(NULL));
  Power_t* pvalues = malloc(sizeof(Power_t) * 8);

  Quantity_t counter = QUANTITY_I;
#endif

  // initilization for the communication
#if INTERNET_DOMAIN
  int sockfd                = init_client_internet_domain(HOST_NAME, PORTNO);
#else
  int sockfd                = init_client_unix_domain(SOCKET_NAME);
#endif

  MessagesQueue*     mqueue = alloc_and_init_messages_queue();
  TemperaturesQueue* tqueue = alloc_and_init_temperatures_queue();

  // create network messages
  Temperature_t* max_results = malloc(sizeof(Temperature_t) * 4);
  Temperature_t cell1, cell2, outlet;

  nm_get_all_max_temperatures_of_floorplan (mqueue, tqueue, "die2", max_results, 4);
  nm_get_cell_temperature (mqueue, tqueue, 2, 11, 4, &cell1);
  nm_get_cell_temperature (mqueue, tqueue, 6, 34, 84, &cell2);
  nm_get_temperature_of_channel_outlet (mqueue, tqueue, "channel1", 0, &outlet);

  // send_messages
  send_messages(sockfd, mqueue);

  Bool_t is_emulating;
  while (1) {

#if DYNAMIC_POWER_VALUES
    for (i = 0; i < 8; i++) {
      pvalues[i] = rand() % 30;
    }
    counter++;

    if (counter < 100)
      send_power_values(sockfd, pvalues, 8, TRUE_V);
    else
      send_power_values(sockfd, pvalues, 8, FALSE_V);
#endif

    is_emulating = get_results(sockfd, tqueue);
    if (is_emulating == FALSE_V)
      break;

    for (i = 0 ; i < 4 ; i++)
      printf("%7.3f  ", max_results[i]) ;
    printf("%7.3f  %7.3f  %7.3f\n", cell1, cell2, outlet) ;
  }

  close_client(sockfd);

  free_temperatures_queue(tqueue);
  free_messages_queue(mqueue);

  return EXIT_SUCCESS ;
}
