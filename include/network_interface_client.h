/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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

#ifndef _3DICE_SERVER_H_
#define _3DICE_SERVER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "macros.h"
#include "stack_description.h"
#include "thermal_data.h"
#include "messages_queue.h"
#include "temperatures_queue.h"

typedef struct {
  /* Time Values */

  Time_t SlotTime ;
  Time_t StepTime ;

  /* Quantities used for simulation */

  Quantity_t SlotLength ;
  Quantity_t CurrentTime ;

  /* emulate_step() or emulate_slot() */
  Bool_t IsTransient ;

} TimeInfo;

TimeInfo* alloc_and_init_time_info (Time_t slot_time, Time_t step_time, Bool_t is_transient);

void init_time_info (TimeInfo* time_info, Time_t slot_time, Time_t step_time, Bool_t is_transient);

void free_time_info (TimeInfo* time_info);

Time_t get_current_time_client(TimeInfo* time_info);

Bool_t is_slot_time_client(TimeInfo* time_info);

int init_client_unix_domain(String_t socket_name);

int init_client_internet_domain(String_t host_name, int portno);

void close_client(int sockfd);

void send_client_info(int sockfd, Time_t slot_time, Time_t step_time, Bool_t is_transient);

void send_messages(int sockfd, MessagesQueue* queue);

void send_power_values(int sockfd, Power_t* pvalues, Quantity_t num_values, Bool_t is_terminating);

void get_results(int sockfd, TemperaturesQueue* queue);

Bool_t emulate(TimeInfo* time_info, Bool_t is_terminating);

/******************************************************************************/
// creating messages
void nm_get_max_temperature_of_floorplan_element
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t* temp_values
);

void nm_get_min_temperature_of_floorplan_element
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t* temp_values
);

void nm_get_avg_temperature_of_floorplan_element
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t* temp_values
);

void nm_get_cell_temperature
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  GridDimension_t layer_index,
  GridDimension_t row_index,
  GridDimension_t column_index,
  Temperature_t* temp_values
);

void nm_get_all_max_temperatures_of_floorplan
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
);

void nm_get_all_min_temperatures_of_floorplan
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
);

void nm_get_all_avg_temperatures_of_floorplan
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
);

void nm_get_all_temperatures_of_channel_outlets
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          channel_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
);

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CLIENT_H_ */

