/******************************************************************************
 * Header file "3D-ICe/include/stack_element.h"                               *
 *                                                                            *
 * This file is part of 3D-ICe (http://esl.epfl.ch/3D-ICe), revision 0.1      *
 *                                                                            *
 * 3D-ICe is free software: you can redistribute it and/or modify it under    *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or any later         *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICe is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with 3D-ICe.  If not, see <http://www.gnu.org/licenses/>.                  *
 *                                                                            *
 * Copyright (C) 2010,                                                        *
 * Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne.    *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * Authors: Alessandro Vincenzi, Arvind Sridhar.                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                          threed-ice@esl.epfl.ch *
 ******************************************************************************/

#ifndef _3DICE_STACK_ELEMENT_H_
#define _3DICE_STACK_ELEMENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "layer.h"
#include "die.h"
#include "floorplan.h"

  enum StackElement_t
  {
    TL_STACK_ELEMENT_NONE = 0,
    TL_STACK_ELEMENT_LAYER   ,
    TL_STACK_ELEMENT_CHANNEL ,
    TL_STACK_ELEMENT_DIE
  } ;

  union StackElement_p
  {
    Layer* Layer ;
    Die*   Die ;
  } ;

  struct StackElement
  {
    /* The id (string) of the stack element */

    String_t Id ;

    /* The type of the stack element */

    enum StackElement_t Type ;

    /* The pointer to the effective stack element */
    /* corresponding to the value in Type         */

    union StackElement_p Pointer ;

    /* The number of layer composing the stack element */

    Quantity_t NLayers ;

    /* A pointer to a Floorplan. This field is   */
    /* used only if Type == TL_STACK_ELEMENT_DIE */

    Floorplan* Floorplan ;

    /* The offset (#of layers) counting from the */
    /* first layer in the stack                  */

    Quantity_t LayersOffset ;

    /* To collect stack elements in a linked list */

    struct StackElement* Next ;

  } ;

  typedef struct StackElement StackElement ;

/******************************************************************************/

  void init_stack_element (StackElement* stack_element) ;

/******************************************************************************/

  StackElement* alloc_and_init_stack_element (void) ;

/******************************************************************************/

  void free_stack_element (StackElement* stack_element) ;

/******************************************************************************/

  void free_stack_elements_list (StackElement* list) ;

/******************************************************************************/

  void print_stack_elements_list
  (
    FILE*         stream,
    String_t      prefix,
    StackElement* list
  ) ;

/******************************************************************************/

  StackElement* find_stack_element_in_list (StackElement* list, String_t id) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_ELEMENT_H_ */
