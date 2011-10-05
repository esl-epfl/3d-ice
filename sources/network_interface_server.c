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

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <strings.h>
#include <stdio.h>

#include "network_interface_server.h"

int init_server_unix_domain(String_t socket_name)
{
  int sockfd, newsockfd, servlen;
  socklen_t clilen;
  struct sockaddr_un  cli_addr, serv_addr;

  if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    printf("ERROR :: creating socket\n");
    return sockfd;
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;

  strcpy(serv_addr.sun_path, socket_name);
  servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0) {
    printf("ERROR :: binding socket\n");
    return sockfd;
  }

  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  printf("Server initialized. Waiting for a client ...\n");

  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) {
    printf("ERROR :: accepting client\n");
    return newsockfd;
  }

  printf("Client accepted.\n");
  return newsockfd;
}

int init_server_internet_domain(int portno)
{
  int sockfd, newsockfd;
  socklen_t clilen;
  struct sockaddr_in  cli_addr, serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("ERROR :: creating socket\n");
    return sockfd;
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    printf("ERROR :: binding socket\n");
    return sockfd;
  }

  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  printf("Server initialized. Waiting for a client ...\n");

  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) {
    printf("ERROR :: accepting client\n");
    return newsockfd;
  }

  printf("Client accepted.\n");
  return newsockfd;
}

void close_server(int sockfd)
{
  Bool_t end_emulation = TRUE_V;
  if (write(sockfd, &end_emulation, sizeof(Bool_t)) == -1)
      printf("ERROR :: write error\n") ;
  close(sockfd);
}

void get_client_info(int sockfd, Time_t* slot_time, Time_t* step_time, Bool_t* is_transient)
{
  if ( read(sockfd, slot_time, sizeof(Time_t)) == -1)
      printf("ERROR :: read error\n") ;
  if ( read(sockfd, step_time, sizeof(Time_t)) == -1)
      printf("ERROR :: read error\n") ;
  if ( read(sockfd, is_transient, sizeof(Bool_t)) == -1)
      printf("ERROR :: read error\n") ;
}


void get_messages(int sockfd, MessagesQueue* queue)
{
  Quantity_t i;
  Quantity_t num_messages;

  if (read(sockfd, &num_messages, sizeof(Quantity_t)) == -1)
      printf("ERROR :: read error\n") ;

  for (i = 0; i < num_messages; i++) {

    NetworkMessage message;
    if (read(sockfd, &message, sizeof(NetworkMessage)) == -1)
      printf("ERROR :: read error\n") ;

    put_into_messages_queue(queue, message);

  }
}

Power_t* get_power_values_from_network(int sockfd, Power_t* pvalues, Quantity_t num_values, Bool_t* is_terminating)
{
  if (read(sockfd, is_terminating, sizeof(Bool_t)) == -1)
      printf("ERROR :: read error\n") ;

  if (*is_terminating == FALSE_V)
    if (read(sockfd, pvalues, num_values * sizeof(Power_t)) == -1)
      printf("ERROR :: read error\n") ;

  return pvalues;
}

void process_messages
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  StackDescription *stkd,
  ThermalData* tdata
)
{
  MessageNode* tmp;
  for (tmp = mqueue->Head; tmp != NULL; tmp = tmp->Next) {
    NetworkMessage message = tmp->Message;

    switch (message.header) {

    case MT_FLOORPLAN:
      process_floorplan_message(message, tqueue, stkd, tdata);
      break;

    case MT_FLOORPLAN_ELEMENT:
      process_floorplan_element_message(message, tqueue, stkd, tdata);
      break;

    case MT_CHANNEL:
      process_channel_message(message, tqueue, stkd, tdata);
      break;

    case MT_CHANNEL_OUTLET:
      process_channel_outlet_message(message, tqueue, stkd, tdata);
      break;

    case MT_CELL:
      process_cell_message(message, tqueue, stkd, tdata);
      break;

    default:
      fprintf(stderr, "Selecting a message type\n");

      break;
    }
  }
}

void send_results(int sockfd, TemperaturesQueue* queue)
{
  TemperatureNode* tmp;
  for (tmp = queue->Head; tmp != NULL; tmp = tmp->Next) {
    if (write(sockfd, tmp->Values, sizeof(Temperature_t) * tmp->NumValues) == -1)
      printf("ERROR :: write error\n") ;
  }

  while (! is_empty_temperatures_queue(queue) ) {
    FREE_POINTER (free, queue->Head->Values) ;
    pop_from_temperatures_queue(queue) ;
  }
}

void process_floorplan_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
)
{
  Temperature_t* temp_values = malloc(sizeof(Temperature_t) * message.num_results);

  switch (message.temperature_type) {
  case TT_MAX:
    get_all_max_temperatures_of_floorplan(
      stkd,
      tdata,
      message.floorplan_id,
      temp_values
      );
    break;
  case TT_MIN:
    get_all_min_temperatures_of_floorplan(
      stkd,
      tdata,
      message.floorplan_id,
      temp_values
      );
    break;
  case TT_AVG:
    get_all_avg_temperatures_of_floorplan(
      stkd,
      tdata,
      message.floorplan_id,
      temp_values
      );
    break;
  default:
    fprintf(stderr, "Selecting a temperature type");
  }

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

void process_floorplan_element_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
)
{
  Temperature_t* temp_values = malloc(sizeof(Temperature_t) * message.num_results);

  switch (message.temperature_type) {
  case TT_MAX:
    get_max_temperature_of_floorplan_element(
      stkd,
      tdata,
      message.floorplan_id,
      message.floorplan_element_id,
      temp_values
      );
    break;
  case TT_MIN:
    get_min_temperature_of_floorplan_element(
      stkd,
      tdata,
      message.floorplan_id,
      message.floorplan_element_id,
      temp_values
      );
    break;
  case TT_AVG:
    get_avg_temperature_of_floorplan_element(
      stkd,
      tdata,
      message.floorplan_id,
      message.floorplan_element_id,
      temp_values
      );
    break;
  default:
    fprintf(stderr, "Selecting a temperature type");
  }

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}
void process_channel_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
)
{
  Temperature_t* temp_values = malloc(sizeof(Temperature_t) * message.num_results);

  get_cell_temperature(
    stkd,
    tdata,
    message.layer_index,
    message.row_index,
    message.column_index,
    temp_values
    );

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

void process_channel_outlet_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
)
{
  Temperature_t* temp_values = malloc(sizeof(Temperature_t) * message.num_results);

  get_temperature_of_channel_outlet(
    stkd,
    tdata,
    message.channel_id,
    message.outlet_number,
    temp_values
    );

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

void process_cell_message
(
  NetworkMessage message,
  TemperaturesQueue* tqueue,
  StackDescription* stkd,
  ThermalData* tdata
)
{
  Temperature_t* temp_values = malloc(sizeof(Temperature_t) * message.num_results);

  get_cell_temperature(
    stkd,
    tdata,
    message.layer_index,
    message.row_index,
    message.column_index,
    temp_values
    );

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

