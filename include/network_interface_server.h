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

// Functions related to server

int init_server_unix_domain(String_t socket_name);

int init_server_internet_domain(int portno);

void close_server(int sockfd);

void get_client_info(int sockfd, Time_t* slot_time, Time_t* step_time, Bool_t* is_transient);

void get_messages(int sockfd, MessagesQueue* queue);

Power_t* get_power_values_from_network
(
  int sockfd,
  Power_t* pvalues,
  Quantity_t num_values,
  Bool_t* is_terminating
 );

void process_messages
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  StackDescription *stkd,
  ThermalData* tdata
);

void send_results(int sockfd, TemperaturesQueue* queue);

void process_floorplan_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
);

void process_floorplan_element_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
);

void process_channel_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
);

void process_cell_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
);


#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SERVER_H_ */

