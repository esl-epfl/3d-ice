/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE, version 1.0 .                                 *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                           3d-ice@listes.epfl.ch *
 * 1015 Lausanne, Switzerland                  http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_POWERSQUEUE_H_
#define _3DICE_POWERSQUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"

/******************************************************************************/

  struct PowerNode
  {
    Power_t Value ;
    struct PowerNode* Next ;
  } ;

  typedef struct PowerNode PowerNode ;

/******************************************************************************/

  typedef struct
  {
    PowerNode* Head ;
    PowerNode* Tail ;
    Quantity_t Length ;

  } PowersQueue ;

/******************************************************************************/

  void init_powers_queue (PowersQueue* queue) ;

/******************************************************************************/

  PowersQueue* alloc_and_init_powers_queue (void) ;

/******************************************************************************/

  Bool_t is_empty_powers_queue (PowersQueue* queue) ;

/******************************************************************************/

  void free_powers_queue (PowersQueue* queue) ;

/******************************************************************************/

  void put_into_powers_queue (PowersQueue* queue, Power_t power) ;

/******************************************************************************/

  Power_t get_from_powers_queue (PowersQueue* queue) ;

/******************************************************************************/

  void pop_from_powers_queue (PowersQueue* queue) ;

/******************************************************************************/

  void print_powers_queue (FILE* stream, String_t prefix, PowersQueue* queue) ;

/******************************************************************************/

  void print_formatted_powers_queue (FILE* stream, PowersQueue* queue) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_POWERSQUEUE_H_ */
