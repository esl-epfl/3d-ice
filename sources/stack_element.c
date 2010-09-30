/******************************************************************************
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
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *                                                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdlib.h>

#include "stack_element.h"

/******************************************************************************/

void
init_stack_element (StackElement* stack_element)
{
  stack_element->Type          = TDICE_STACK_ELEMENT_NONE ;
  stack_element->Pointer.Layer = NULL ;
  stack_element->Pointer.Die   = NULL ;
  stack_element->Floorplan     = NULL ;
  stack_element->Id            = NULL ;
  stack_element->NLayers       = 0 ;
  stack_element->LayersOffset  = 0 ;
  stack_element->Next          = NULL ;
}

/******************************************************************************/

StackElement* alloc_and_init_stack_element (void)
{
  StackElement* stack_element
    = (StackElement* ) malloc( sizeof(StackElement) ) ;

  if (stack_element != NULL) init_stack_element(stack_element) ;

  return stack_element ;
}

/******************************************************************************/

void free_stack_element (StackElement* stack_element)
{
  if (stack_element->Type == TDICE_STACK_ELEMENT_DIE
      && stack_element->Floorplan != NULL)

    free_floorplan (stack_element->Floorplan) ;

  else if (stack_element->Type == TDICE_STACK_ELEMENT_LAYER
           && stack_element->Pointer.Layer != NULL)

    free_layer (stack_element->Pointer.Layer) ;

  free (stack_element->Id) ;
  free (stack_element) ;
}

/******************************************************************************/

void free_stack_elements_list (StackElement* list)
{
  StackElement* next ;

  for ( ; list != NULL; list = next)
  {
    next = list->Next ;
    free_stack_element (list) ;
  }
}

/******************************************************************************/

void print_stack_elements_list
(
  FILE*         stream,
  String_t      prefix,
  StackElement* list
)
{
  fprintf (stream, "%sStack:\n", prefix) ;

  for ( ; list != NULL ; list = list->Next)
  {
    fprintf (stream,
      "%s  Stackelement (l %-2d)\t%s\t",
      prefix, list->LayersOffset, list->Id);

    switch (list->Type)
    {
      case TDICE_STACK_ELEMENT_NONE :

        fprintf (stream, "NO TYPE\n") ;
        break ;

      case TDICE_STACK_ELEMENT_DIE :

        fprintf (stream,
          "die     (%s) %s \n",
          list->Pointer.Die->Id, list->Floorplan->FileName) ;
        break ;

      case TDICE_STACK_ELEMENT_LAYER :

        fprintf (stream,
          "layer   (%s) %5.2f um\n",
          list->Pointer.Layer->Material->Id, list->Pointer.Layer->Height) ;
        break ;

      case TDICE_STACK_ELEMENT_CHANNEL :

        fprintf (stream,
          "channel\n") ;
        break ;

      default :

        fprintf (stream, "Error! Unknown type %d\n", list->Type) ;
        break ;
    }
  }
}

/******************************************************************************/

StackElement*  find_stack_element_in_list (StackElement* list, String_t id)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp(list->Id, id) == 0)  break ;

 return list ;
}

/******************************************************************************/
