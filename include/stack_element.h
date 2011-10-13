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

#ifndef _3DICE_STACK_ELEMENT_H_
#define _3DICE_STACK_ELEMENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "layer.h"
#include "die.h"
#include "channel.h"
#include "floorplan.h"

/******************************************************************************/

  struct StackElement
  {
    /* The id (string) of the stack element */

    String_t Id ;

    /* The type of the stack element */

    StackElement_t Type ;

    /* The pointer to the effective stack element */
    /* corresponding to the value in Type         */

    union {

      Layer*   Layer ;
      Die*     Die ;
      Channel* Channel ;

    } Pointer ;

    /* The number of layer composing the stack element */

    GridDimension_t NLayers ;

    /* A pointer to a Floorplan. This field is   */
    /* used only if Type == TL_STACK_ELEMENT_DIE */

    Floorplan* Floorplan ;

    /* The offset (#of layers) counting from the */
    /* first layer in the stack                  */

    GridDimension_t Offset ;

    /* To collect stack elements in a doubled linked list */

    struct StackElement* Prev ;
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

  void print_formatted_stack_elements_list
  (
    FILE*         stream,
    String_t      prefix,
    StackElement* list
  ) ;

/******************************************************************************/

  void print_detailed_stack_elements_list
  (
    FILE*         stream,
    String_t      prefix,
    StackElement* list
  ) ;

/******************************************************************************/

  void fill_thermal_cell_stack_element
  (
    ThermalCell*          thermalcells,
    Time_t                delta_time,
    Dimensions*           dimensions,
    StackElement*         stack_element
  ) ;

/******************************************************************************/

  void fill_sources_stack_element
  (
    Source_t*             sources,
    Dimensions*           dimensions,
    StackElement*         stack_element
  ) ;

/******************************************************************************/

  void init_power_values_stack_element
  (
    StackElement*         stack_element
  ) ;

/******************************************************************************/

  void insert_power_values_stack_element
  (
    StackElement*         stack_element,
    PowersQueue*          pvalues
  ) ;

/******************************************************************************/

  SystemMatrix fill_system_matrix_stack_element
  (
    SystemMatrix          system_matrix,
    Dimensions*           dimensions,
    ThermalCell*          thermalcells,
    StackElement*         stack_element
  ) ;

/******************************************************************************/

  StackElement* find_stack_element_in_list (StackElement* list, String_t id) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_ELEMENT_H_ */
