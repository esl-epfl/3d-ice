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

#ifndef _3DICE_NETWORK_INTERFACE_H_
#define _3DICE_NETWORK_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "macros.h"
#include "stack_description.h"
#include "thermal_data.h"

#define MAX_ID_SIZE 16

enum MessageType
{
  MT_NONE = 0,
  MT_FLOORPLAN,
  MT_FLOORPLAN_ELEMENT,
  MT_CHANNEL,
  MT_CHANNEL_OUTLET,
  MT_CELL
} ;

typedef enum MessageType MessageType;

enum TemperatureType
{
  TT_NONE = 0,
  TT_MAX,
  TT_MIN,
  TT_AVG
} ;

typedef enum TemperatureType TemperatureType;

typedef struct {

  MessageType       header;

  char              floorplan_id[MAX_ID_SIZE];
  char              floorplan_element_id[MAX_ID_SIZE];

  char              channel_id[MAX_ID_SIZE];

  GridDimension_t   layer_index;
  GridDimension_t   row_index;
  GridDimension_t   column_index;

  GridDimension_t   outlet_number;

  TemperatureType   temperature_type;

  Quantity_t        num_results;

} NetworkMessage;

struct MessageNode
{
  NetworkMessage Message ;
  struct MessageNode* Next ;
} ;

typedef struct MessageNode MessageNode ;

typedef struct
{
  MessageNode* Head ;
  MessageNode* Tail ;
  Quantity_t Length ;

} MessagesQueue ;

struct TemperatureNode
{
  Temperature_t* Values ;
  Quantity_t NumValues;
  struct TemperatureNode* Next ;
} ;

typedef struct TemperatureNode TemperatureNode ;

typedef struct
{
  TemperatureNode* Head ;
  TemperatureNode* Tail ;
  Quantity_t Length ;

} TemperaturesQueue ;

// Functions related to MessagesQueue

void init_messages_queue (MessagesQueue* queue) ;

MessagesQueue* alloc_and_init_messages_queue (void) ;

Bool_t is_empty_messages_queue (MessagesQueue* queue) ;

void free_messages_queue (MessagesQueue* queue) ;

void put_into_messages_queue (MessagesQueue* queue, NetworkMessage message) ;

void pop_from_messages_queue (MessagesQueue* queue) ;

// Functions related to MessagesQueue
//
void init_temperatures_queue (TemperaturesQueue* queue);

TemperaturesQueue* alloc_and_init_temperatures_queue (void);

Bool_t is_empty_temperatures_queue (TemperaturesQueue* queue);

void free_temperatures_queue (TemperaturesQueue* queue);

void put_into_temperatures_queue
(
  TemperaturesQueue* queue,
  Temperature_t* temp_values,
  Quantity_t num_values
);

void pop_from_temperatures_queue (TemperaturesQueue* queue);

// Functions related to server

int init_server_unix_domain(String_t socket_name);

int init_server_internet_domain(int portno);

void close_server(int sockfd);

void get_messages(int sockfd, MessagesQueue* queue);

Power_t* get_power_values_from_network
(
  int sockfd,
  Power_t* pvalues,
  Quantity_t num_values,
  Bool_t* is_valid
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

void process_channel_outlet_message
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


// Functions related to client

int init_client_unix_domain(String_t socket_name);

int init_client_internet_domain(String_t host_name, int portno);

void close_client(int sockfd);

void send_messages(int sockfd, MessagesQueue* queue);

void send_power_values(int sockfd, Power_t* pvalues, Quantity_t num_values, Bool_t is_valid);

Bool_t get_results(int sockfd, TemperaturesQueue* queue);

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

void nm_get_temperature_of_channel_outlet
(
  MessagesQueue* mqueue,
  TemperaturesQueue* tqueue,
  String_t          channel_id,
  GridDimension_t   outlet_number,
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

#endif /* _3DICE_NETWORK_INTERFACE_H_ */

