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

#include <stdlib.h>
#include <string.h>

#include "stack_element.h"
#include "macros.h"

/******************************************************************************/

void
init_stack_element (StackElement* stack_element)
{
  stack_element->Type            = STACKELEMENT_I ;
  stack_element->Pointer.Layer   = NULL ;
  stack_element->Pointer.Die     = NULL ;
  stack_element->Pointer.Channel = NULL ;
  stack_element->Floorplan       = NULL ;
  stack_element->Id              = STRING_I ;
  stack_element->NLayers         = GRIDDIMENSION_I ;
  stack_element->Offset          = GRIDDIMENSION_I ;
  stack_element->Next            = NULL ;
}

/******************************************************************************/

StackElement* alloc_and_init_stack_element (void)
{
  StackElement* stack_element = malloc (sizeof(*stack_element)) ;

  if (stack_element != NULL) init_stack_element(stack_element) ;

  return stack_element ;
}

/******************************************************************************/

void free_stack_element (StackElement* stack_element)
{
  if (stack_element->Type == TDICE_STACK_ELEMENT_DIE
      && stack_element->Floorplan != NULL)

    FREE_POINTER (free_floorplan, stack_element->Floorplan) ;

  else if (stack_element->Type == TDICE_STACK_ELEMENT_LAYER
           && stack_element->Pointer.Layer != NULL)

    FREE_POINTER (free_layer, stack_element->Pointer.Layer) ;

  FREE_POINTER (free, stack_element->Id) ;
  FREE_POINTER (free, stack_element) ;
}

/******************************************************************************/

void free_stack_elements_list (StackElement* list)
{
  FREE_LIST (StackElement, list, free_stack_element) ;
}

/******************************************************************************/

void print_formatted_stack_elements_list
(
  FILE*         stream,
  String_t      prefix,
  StackElement* list
)
{
  unsigned int max_stk_el_id_length = 0 ;
  unsigned int max_die_id_length = 0 ;

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, list)
  {
    max_stk_el_id_length = MAX (max_stk_el_id_length,
                                strlen(stk_el->Id)) ;

    if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)

      max_die_id_length = MAX (max_die_id_length,
                               strlen(stk_el->Pointer.Die->Id)) ;
  }

  fprintf (stream, "%sstack : \n", prefix) ;

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element, list)
  {
    switch (stack_element->Type)
    {
      case TDICE_STACK_ELEMENT_CHANNEL :

        fprintf (stream,
                 "%s   channel  %-*s ;\n",
                 prefix, max_stk_el_id_length, stack_element->Id) ;
        break ;

      case TDICE_STACK_ELEMENT_DIE :

        fprintf (stream,
                 "%s   die      %-*s %-*s floorplan \"%s\"\n",
                 prefix,
                 max_stk_el_id_length, stack_element->Id,
                 max_die_id_length, stack_element->Pointer.Die->Id,
                 stack_element->Floorplan->FileName) ;
        break ;

      case TDICE_STACK_ELEMENT_LAYER :

        fprintf (stream,
                 "%s   layer    %-*s ;\n",
                 prefix, max_stk_el_id_length, stack_element->Id) ;
        break ;

      case TDICE_STACK_ELEMENT_NONE :

        fprintf (stderr, "Warining: found stack element type none\n") ;
        break ;

      default :

        fprintf (stderr, "Undefined stack element type %d\n",
                 stack_element->Type) ;
    }
  }
}

/******************************************************************************/

void print_detailed_stack_elements_list
(
  FILE*         stream,
  String_t      prefix,
  StackElement* list
)
{
  String_t new_prefix = malloc (sizeof(*new_prefix) * (4 + strlen(prefix))) ;
  if (new_prefix == NULL) return ;
  sprintf (new_prefix, "%s    ", prefix) ;

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, list)
  {
    fprintf (stream,
             "%sstk_el                      = %p\n",
             prefix,   stk_el);

    fprintf (stream,
             "%s  Id                        = " "%s\n",
             prefix,   stk_el->Id);

    fprintf (stream,
             "%s  Type                      = %d\n",
             prefix,   stk_el->Type);

    if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)
    {
     fprintf (stream,
              "%s  Pointer.Die               = %p\n",
              prefix,   stk_el->Pointer.Die);
    }
    if (stk_el->Type == TDICE_STACK_ELEMENT_CHANNEL)
    {
     fprintf (stream,
              "%s  Pointer.Channel           = %p\n",
              prefix,   stk_el->Pointer.Channel);
    }
    else if (stk_el->Type == TDICE_STACK_ELEMENT_LAYER)
    {
      fprintf (stream,
               "%s  Pointer.Layer             = %p\n",
               prefix,   stk_el->Pointer.Layer);
      fprintf (stream, "%s\n", prefix) ;
      print_detailed_layer (stream, new_prefix, stk_el->Pointer.Layer) ;
      fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  NLayers                   = %d\n",
             prefix,   stk_el->NLayers);

    fprintf (stream,
             "%s  Floorplan                 = %p\n",
             prefix,   stk_el->Floorplan);

    fprintf (stream,
             "%s  Offset                    = %d\n",
             prefix,   stk_el->Offset);

    fprintf (stream,
             "%s  Next                      = %p\n",
             prefix,   stk_el->Next);

    fprintf (stream, "%s\n", prefix) ;

  } // FOR_EVERY_ELEMENT_IN_LIST

  FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void fill_thermal_cell_stack_element
(
  ThermalCell*     thermalcells,
  Time_t           delta_time,
  Dimensions*      dimensions,
  StackElement*    stack_element
)
{
  switch (stack_element->Type)
  {
    case TDICE_STACK_ELEMENT_DIE :

      fill_thermal_cell_die
      (
        thermalcells,
        delta_time,
        dimensions,
        stack_element->Offset,
        stack_element->Pointer.Die
      ) ;

      break ;

    case TDICE_STACK_ELEMENT_LAYER :

      fill_thermal_cell_layer
      (
        thermalcells,
        delta_time,
        dimensions,
        stack_element->Offset,
        stack_element->Pointer.Layer
      ) ;

      break ;

    case TDICE_STACK_ELEMENT_CHANNEL :

      fill_thermal_cell_channel
      (
        thermalcells,
        delta_time,
        dimensions,
        stack_element->Offset,
        stack_element->Pointer.Channel
      ) ;

      break ;

    case TDICE_STACK_ELEMENT_NONE :

      fprintf (stderr,
        "Error! Found stack element with unset type\n") ;
      break ;

    default :

      fprintf (stderr,
        "Error! Unknown stack element type %d\n",
        stack_element->Type) ;

  } /* switch stack_element->Type */
}

/******************************************************************************/

void fill_sources_stack_element
(
  Source_t*             sources,
  Dimensions*           dimensions,
  StackElement*         stack_element
)
{
  switch (stack_element->Type)
  {
    case TDICE_STACK_ELEMENT_DIE :

      fill_sources_die
      (
        sources,
        dimensions,
        stack_element->Offset,
        stack_element->Floorplan,
        stack_element->Pointer.Die
      ) ;
      break ;

    case TDICE_STACK_ELEMENT_LAYER :

      break ;

    case TDICE_STACK_ELEMENT_CHANNEL :

      fill_sources_channel
      (
        sources,
        dimensions,
        stack_element->Offset,
        stack_element->Pointer.Channel
      ) ;
      break ;

    case TDICE_STACK_ELEMENT_NONE :

      fprintf (stderr,
        "Error! Found stack element with unset type\n") ;
      break ;

    default :

      fprintf (stderr,
        "Error! Unknown stack element type %d\n",
        stack_element->Type) ;

  } /* switch stack_element->Type */
}

/******************************************************************************/

void init_power_values_stack_element
(
  StackElement*         stack_element
)
{
  switch (stack_element->Type)
  {
    case TDICE_STACK_ELEMENT_DIE :

      init_power_values_die
      (
        stack_element->Floorplan
      ) ;
      break ;

    case TDICE_STACK_ELEMENT_LAYER :

      break;

    case TDICE_STACK_ELEMENT_CHANNEL :

      break;

    case TDICE_STACK_ELEMENT_NONE :

      break;

    default :

      fprintf (stderr,
        "Error! Unknown stack element type %d\n",
        stack_element->Type) ;

  } /* switch stack_element->Type */
}

/******************************************************************************/

void insert_power_values_stack_element
(
  StackElement*         stack_element,
  PowersQueue*          pvalues
)
{
  switch (stack_element->Type)
  {
    case TDICE_STACK_ELEMENT_DIE :

      insert_power_values_die
      (
        stack_element->Floorplan,
        pvalues
      ) ;
      break ;

    case TDICE_STACK_ELEMENT_LAYER :

      break;

    case TDICE_STACK_ELEMENT_CHANNEL :

      break;

    case TDICE_STACK_ELEMENT_NONE :

      break;

    default :

      fprintf (stderr,
        "Error! Unknown stack element type %d\n",
        stack_element->Type) ;

  } /* switch stack_element->Type */
}

/******************************************************************************/

SystemMatrix fill_system_matrix_stack_element
(
  SystemMatrix     system_matrix,
  Dimensions*      dimensions,
  ThermalCell*     thermalcells,
  StackElement*    stack_element
)
{
  switch (stack_element->Type)
  {
    case TDICE_STACK_ELEMENT_DIE :

      system_matrix = fill_system_matrix_die
                      (
                        stack_element->Pointer.Die, dimensions,
                        thermalcells, stack_element->Offset,
                        system_matrix
                      ) ;
      break ;

    case TDICE_STACK_ELEMENT_LAYER :

      system_matrix = fill_system_matrix_layer
                      (
#                       ifdef PRINT_SYSTEM_MATRIX
                        stack_element->Pointer.Layer,
#                       endif
                        dimensions,
                        thermalcells, stack_element->Offset,
                        system_matrix
                      ) ;
      break ;

    case TDICE_STACK_ELEMENT_CHANNEL :

      system_matrix = fill_system_matrix_channel
                      (
                        stack_element->Pointer.Channel,
                        dimensions,
                        thermalcells, stack_element->Offset,
                        system_matrix
                      ) ;
      break ;

    case TDICE_STACK_ELEMENT_NONE :

      fprintf (stderr,
        "Error! Found stack element with unset type\n") ;
      break ;

    default :

      fprintf (stderr,
        "Error! Unknown stack element type %d\n",
        stack_element->Type) ;

  } /* stk_el->Type */

  return system_matrix ;
}

/******************************************************************************/

StackElement*  find_stack_element_in_list (StackElement* list, String_t id)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, list)

    if (strcmp(stk_el->Id, id) == 0)  break ;

 return stk_el ;
}

/******************************************************************************/
