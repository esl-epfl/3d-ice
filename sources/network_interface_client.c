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

#include "network_interface_client.h"

TimeInfo* alloc_and_init_time_info (Time_t slot_time, Time_t step_time, Bool_t is_transient)
{
  TimeInfo* time_info = malloc (sizeof(TimeInfo)) ;

  if (time_info != NULL)
    init_time_info(time_info, slot_time, step_time, is_transient);

  return time_info ;
}

void init_time_info (TimeInfo* time_info, Time_t slot_time, Time_t step_time, Bool_t is_transient)
{
  time_info->SlotTime     = slot_time;
  time_info->StepTime     = step_time;
  time_info->SlotLength   = (Quantity_t) ( slot_time / step_time ) ;
  time_info->CurrentTime  = QUANTITY_I ;
  time_info->IsTransient  = is_transient;
}

void free_time_info (TimeInfo* time_info)
{
  free(time_info);
  time_info = NULL;
}


Time_t get_current_time_client(TimeInfo* time_info)
{
  return time_info->CurrentTime * time_info->StepTime ;
}


Bool_t is_slot_time_client(TimeInfo* time_info)
{
  return (time_info->CurrentTime % time_info->SlotLength == 0) ? TRUE_V : FALSE_V;
}

/******************************************************************************/

int init_client_unix_domain(String_t socket_name)
{
  int sockfd, servlen;
  struct sockaddr_un serv_addr;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("ERROR :: Creating socket");
    return sockfd;
  }

  bzero((char *)&serv_addr,sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, socket_name);
  servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

  if (connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0) {
    printf("ERROR :: Connection error\n");
    return sockfd;
  }

  printf("Connection established.\n");
  return sockfd;
}

int init_client_internet_domain(String_t host_name, int portno)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("ERROR :: Creating socket");
    return sockfd;
  }

  server = gethostbyname(host_name);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    return -1;
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    printf("ERROR :: Connection error\n");
    return sockfd;
  }

  printf("Connection established.\n");
  return sockfd;
}

void close_client(int sockfd)
{
  close(sockfd);
}

void send_client_info(int sockfd, Time_t slot_time, Time_t step_time, Bool_t is_transient)
{
  if ( write(sockfd, &(slot_time), sizeof(Time_t)) == -1 )
      printf("ERROR :: write error\n") ;
  if ( write(sockfd, &(step_time), sizeof(Time_t)) == -1 )
      printf("ERROR :: write error\n") ;
  if ( write(sockfd, &(is_transient), sizeof(Bool_t)) == -1 )
      printf("ERROR :: write error\n") ;
}

void send_power_values(int sockfd, Power_t* pvalues, Quantity_t num_values, Bool_t is_terminating)
{
  if ( write(sockfd, &is_terminating, sizeof(Bool_t)) == -1 )

      printf("ERROR :: write error\n") ;

  if (is_terminating == FALSE_V)

    if ( write(sockfd, pvalues, num_values * sizeof(Power_t)) == -1 )

      printf("ERROR :: write error\n") ;
}

void send_messages(int sockfd, MessagesQueue* queue)
{
  if ( write(sockfd, &(queue->Length), sizeof(Quantity_t)) == -1 )

      printf("ERROR :: write error\n") ;

  MessageNode* tmp;
  for (tmp = queue->Head; tmp != NULL; tmp = tmp->Next)
    if ( write(sockfd, &(tmp->Message), sizeof(NetworkMessage)) == -1 )
      printf("ERROR :: write error\n") ;
}

void get_results(int sockfd, TemperaturesQueue* queue)
{
  TemperatureNode* tmp;
  for (tmp = queue->Head; tmp != NULL; tmp = tmp->Next) {
    if ( read(sockfd, tmp->Values, sizeof(Temperature_t) * tmp->NumValues) == -1 )
      printf("ERROR :: read error\n") ;
  }
}

Bool_t emulate(TimeInfo* time_info, Bool_t is_terminating)
{
  if (time_info->IsTransient)
    time_info->CurrentTime++;
  else
    time_info->CurrentTime += time_info->SlotLength;

  return (is_terminating) ? FALSE_V : TRUE_V;
}

void nm_get_max_temperature_of_floorplan_element
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t floorplan_id,
  String_t floorplan_element_id,
  Temperature_t* temp_values
)
{
  NetworkMessage message;

  message.header       = MT_FLOORPLAN;
  strcpy(message.floorplan_id, floorplan_id);
  strcpy(message.floorplan_element_id, floorplan_element_id);
  message.temperature_type = TT_MAX;
  message.num_results = 1;

  put_into_messages_queue(mqueue, message);
  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}


void nm_get_min_temperature_of_floorplan_element
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t* temp_values
)
{
  NetworkMessage message;

  message.header       = MT_FLOORPLAN;
  strcpy(message.floorplan_id, floorplan_id);
  strcpy(message.floorplan_element_id, floorplan_element_id);
  message.temperature_type = TT_MIN;
  message.num_results = 1;

  put_into_messages_queue(mqueue, message);
  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

void nm_get_avg_temperature_of_floorplan_element
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t* temp_values
)
{
  NetworkMessage message;

  message.header       = MT_FLOORPLAN;
  strcpy(message.floorplan_id, floorplan_id);
  strcpy(message.floorplan_element_id, floorplan_element_id);
  message.temperature_type = TT_AVG;
  message.num_results = 1;

  put_into_messages_queue(mqueue, message);
  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}


void nm_get_cell_temperature
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  GridDimension_t layer_index,
  GridDimension_t row_index,
  GridDimension_t column_index,
  Temperature_t* temp_values
)
{
  NetworkMessage message;

  message.header       = MT_CELL;
  message.layer_index  = layer_index;
  message.row_index    = row_index;
  message.column_index = column_index;
  message.num_results  = 1;

  put_into_messages_queue(mqueue, message);
  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

void nm_get_all_max_temperatures_of_floorplan
(
  MessagesQueue *mqueue,
  TemperaturesQueue *tqueue,
  String_t floorplan_id,
  Temperature_t *temp_values,
  Quantity_t num_results
)
{
  NetworkMessage message;

  message.header           = MT_FLOORPLAN;
  message.temperature_type = TT_MAX;
  strcpy(message.floorplan_id, floorplan_id);
  message.num_results      = num_results;

  put_into_messages_queue(mqueue, message);

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}


void nm_get_all_min_temperatures_of_floorplan
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
)
{
  NetworkMessage message;

  message.header           = MT_FLOORPLAN;
  message.temperature_type = TT_MIN;
  strcpy(message.floorplan_id, floorplan_id);
  message.num_results      = num_results;

  put_into_messages_queue(mqueue, message);

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}



void nm_get_all_avg_temperatures_of_floorplan
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          floorplan_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
)
{
  NetworkMessage message;

  message.header           = MT_FLOORPLAN;
  message.temperature_type = TT_AVG;
  strcpy(message.floorplan_id, floorplan_id);
  message.num_results      = num_results;

  put_into_messages_queue(mqueue, message);

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}


void nm_get_all_temperatures_of_channel_outlets
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          channel_id,
  Temperature_t*    temp_values,
  Quantity_t        num_results
)
{
  NetworkMessage message;

  message.header      = MT_CHANNEL;
  strcpy(message.channel_id, channel_id);
  message.num_results = num_results;

  put_into_messages_queue(mqueue, message);

  put_into_temperatures_queue(tqueue, temp_values, message.num_results);
}

