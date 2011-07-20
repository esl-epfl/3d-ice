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

#ifndef _3DICE_MESSAGES_QUEUE_H_
#define _3DICE_MESSAGES_QUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "macros.h"

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

void init_messages_queue (MessagesQueue* queue) ;

MessagesQueue* alloc_and_init_messages_queue (void) ;

Bool_t is_empty_messages_queue (MessagesQueue* queue) ;

void free_messages_queue (MessagesQueue* queue) ;

void put_into_messages_queue (MessagesQueue* queue, NetworkMessage message) ;

void pop_from_messages_queue (MessagesQueue* queue) ;



#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MESSAGES_QUEUE_H_ */
