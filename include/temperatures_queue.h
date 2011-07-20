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

#ifndef _3DICE_TEMPERATURES_QUEUE_H_
#define _3DICE_TEMPERATURES_QUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "macros.h"

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

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_TEMPERATURES_QUEUE_H_ */
