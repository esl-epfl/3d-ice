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

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <strings.h>
#include <stdio.h>

#include "network_interface.h"


void init_messagess_queue (MessagesQueue* queue)
{
  queue->Head   = NULL ;
  queue->Tail   = NULL ;
  queue->Length = QUANTITY_I ;
}

MessagesQueue* alloc_and_init_messages_queue (void)
{
  MessagesQueue* queue = malloc (sizeof(MessagesQueue)) ;

  if (queue != NULL)  init_messagess_queue (queue) ;

  return queue ;
}

Bool_t is_empty_messages_queue (MessagesQueue* queue)
{
  return (queue->Length == 0) ;
}

void free_messages_queue (MessagesQueue* queue)
{
  while (! is_empty_messages_queue(queue) )

    pop_from_messages_queue(queue) ;

  FREE_POINTER (free, queue) ;
}

void put_into_messages_queue (MessagesQueue* queue, NetworkMessage messages)
{
  MessageNode* tmp = queue->Tail ;

  queue->Tail = malloc (sizeof(MessageNode)) ;

  if ( queue->Tail == NULL )
  {
    fprintf (stderr, "malloc messages node error !!\n") ;
    return ;
  }

  queue->Tail->Message = messages ;
  queue->Tail->Next  = NULL ;

  if (queue->Head == NULL)

    queue->Head = queue->Tail ;

  else

    tmp->Next = queue->Tail ;

  queue->Length++;
}

void pop_from_messages_queue (MessagesQueue* queue)
{
  MessageNode* tmp = queue->Head->Next ;

  FREE_POINTER (free, queue->Head) ;

  queue->Head = tmp ;

  queue->Length--;
}

void init_temperatures_queue (TemperaturesQueue* queue)
{
  queue->Head   = NULL ;
  queue->Tail   = NULL ;
  queue->Length = QUANTITY_I ;
}

TemperaturesQueue* alloc_and_init_temperatures_queue (void)
{
  TemperaturesQueue* queue = malloc (sizeof(TemperaturesQueue)) ;

  if (queue != NULL)  init_temperatures_queue (queue) ;

  return queue ;
}

Bool_t is_empty_temperatures_queue (TemperaturesQueue* queue)
{
  return (queue->Length == 0) ;
}

void free_temperatures_queue (TemperaturesQueue* queue)
{
  while (! is_empty_temperatures_queue(queue) )

    pop_from_temperatures_queue(queue) ;

  FREE_POINTER (free, queue) ;
}

void put_into_temperatures_queue (
  TemperaturesQueue* queue,
  Temperature_t* temp_values,
  Quantity_t num_values
  )
{
  TemperatureNode* tmp = queue->Tail ;

  queue->Tail = malloc (sizeof(TemperatureNode)) ;

  if ( queue->Tail == NULL )
  {
    fprintf (stderr, "malloc temperature node error !!\n") ;
    return ;
  }

  queue->Tail->Values    = temp_values;
  queue->Tail->NumValues = num_values;
  queue->Tail->Next      = NULL;

  if (queue->Head == NULL)
    queue->Head = queue->Tail ;
  else
    tmp->Next = queue->Tail ;

  queue->Length++;
}

void pop_from_temperatures_queue (TemperaturesQueue* queue)
{
  TemperatureNode* tmp = queue->Head->Next ;

  FREE_POINTER (free, queue->Head) ;

  queue->Head = tmp ;

  queue->Length--;
}

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
  write(sockfd, &end_emulation, sizeof(Bool_t));
  close(sockfd);
}

void get_messages(int sockfd, MessagesQueue* queue)
{
  Quantity_t i;
  Quantity_t num_messages;

  read(sockfd, &num_messages, sizeof(Quantity_t));
  for (i = 0; i < num_messages; i++) {

    NetworkMessage message;
    read(sockfd, &message, sizeof(NetworkMessage));
    put_into_messages_queue(queue, message);

  }
}

Power_t* get_power_values_from_network(int sockfd, Power_t* pvalues, Quantity_t num_values, Bool_t* is_valid)
{
  read(sockfd, is_valid, sizeof(Bool_t));

  if (*is_valid) 
    read(sockfd, pvalues, num_values * sizeof(Power_t));

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
  Bool_t end_emulation = FALSE_V;
  write(sockfd, &(end_emulation), sizeof(Bool_t));

  TemperatureNode* tmp;
  for (tmp = queue->Head; tmp != NULL; tmp = tmp->Next) {
    write(sockfd, tmp->Values, sizeof(Temperature_t) * tmp->NumValues);
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

void send_power_values(int sockfd, Power_t* pvalues, Quantity_t num_values, Bool_t is_valid)
{
  write(sockfd, &is_valid, sizeof(Bool_t));

  if (is_valid)
    write(sockfd, pvalues, num_values * sizeof(Power_t));
}

void send_messages(int sockfd, MessagesQueue* queue)
{
  write(sockfd, &(queue->Length), sizeof(Quantity_t));

  MessageNode* tmp;
  for (tmp = queue->Head; tmp != NULL; tmp = tmp->Next)
    write(sockfd, &(tmp->Message), sizeof(NetworkMessage));
}

Bool_t get_results(int sockfd, TemperaturesQueue* queue)
{
  Bool_t end_emulation ;
  read(sockfd, &(end_emulation), sizeof(Bool_t));
  if(end_emulation) {
    return FALSE_V;
  }

  TemperatureNode* tmp;
  for (tmp = queue->Head; tmp != NULL; tmp = tmp->Next) {
    read(sockfd, tmp->Values, sizeof(Temperature_t) * tmp->NumValues);
  }

  return TRUE_V;
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


void nm_get_temperature_of_channel_outlet
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          channel_id,
  GridDimension_t   outlet_number,
  Temperature_t* temp_values
)
{
  NetworkMessage message;

  message.header        = MT_CHANNEL_OUTLET;
  message.outlet_number = outlet_number;
  strcpy(message.channel_id, channel_id);
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

