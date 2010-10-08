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

#include <string.h>
#include <stdlib.h>

#include "floorplan_element.h"

#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

/******************************************************************************/

void init_floorplan_element (FloorplanElement* floorplan_element)
{
  floorplan_element->Id              = NULL ;

  floorplan_element->SW_X            = 0.0 ;
  floorplan_element->SW_Y            = 0.0 ;

  floorplan_element->Length          = 0.0 ;
  floorplan_element->Width           = 0.0 ;

  floorplan_element->EffectiveLength = 0.0 ;
  floorplan_element->EffectiveWidth  = 0.0 ;

  floorplan_element->SW_Row          = 0 ;
  floorplan_element->SW_Column       = 0 ;

  floorplan_element->NE_Row          = 0 ;
  floorplan_element->NE_Column       = 0 ;

  floorplan_element->PowerValues     = NULL ;

  floorplan_element->Next            = NULL ;
}

/******************************************************************************/

FloorplanElement* alloc_and_init_floorplan_element (void)
{
  FloorplanElement* floorplan_element
    = (FloorplanElement* ) malloc ( sizeof(FloorplanElement) );

  if (floorplan_element != NULL)

    init_floorplan_element(floorplan_element) ;

  return floorplan_element ;
}

/*****************************************************************************/

void free_floorplan_element (FloorplanElement* floorplan_element)
{
  free (floorplan_element->Id) ;
  free_powers_queue (floorplan_element->PowerValues) ;
  free (floorplan_element) ;
}

/******************************************************************************/

void free_floorplan_elements_list (FloorplanElement* list)
{
  FloorplanElement* next ;

  for ( ; list != NULL ; list = next)
  {
    next = list->Next ;
    free_floorplan_element (list) ;
  }
}

/******************************************************************************/

void print_floorplan_element
(
  FILE*             stream,
  String_t          prefix,
  FloorplanElement* floorplan_element
)
{
  fprintf (stream, "%sFloorplan Element %s",  prefix, floorplan_element->Id) ;
  fprintf (stream, " ( %.1f , %.1f ) %.1f x %.1f \n",
    floorplan_element->SW_X, floorplan_element->SW_Y,
    floorplan_element->Length, floorplan_element->Width) ;
  fprintf (stream, "%s  Rows (%d - %d)",
    prefix, floorplan_element->SW_Row, floorplan_element->NE_Row) ;
  fprintf (stream, " Columns (%d - %d)\n",
    floorplan_element->SW_Column, floorplan_element->NE_Column) ;

  fprintf (stream,    "%s  Power values ", prefix) ;

  print_powers_queue(stream, prefix, floorplan_element->PowerValues) ;
}

/******************************************************************************/

void print_formatted_floorplan_element
(
  FILE*             stream,
  FloorplanElement* floorplan_element
)
{
  fprintf (stream, "%s:", floorplan_element->Id) ;
  fprintf (stream, "   position %.1f, %.1f ;\n",
                          floorplan_element->SW_X, floorplan_element->SW_Y);
  fprintf (stream, "   dimension %.1f, %.1f ;\n",
                          floorplan_element->Length,
                          floorplan_element->Width) ;
  fprintf (stream, "   power values ") ;
  print_formatted_powers_queue(stream, floorplan_element->PowerValues) ;
  fprintf (stream, "\n") ;
}

/******************************************************************************/

void print_floorplan_elements_list
(
  FILE*             stream,
  String_t          prefix,
  FloorplanElement* list
)
{
  for ( ; list != NULL ; list = list->Next)

    print_floorplan_element (stream, prefix, list) ;
}

/******************************************************************************/

void print_formatted_floorplan_elements_list
(
  FILE*             stream,
  FloorplanElement* list
)
{
  for ( ; list != NULL ; list = list->Next)
  {
    print_formatted_floorplan_element (stream, list) ;
    fprintf(stream, "\n");
  }
}

/******************************************************************************/

FloorplanElement* find_floorplan_element_in_list
(
  FloorplanElement* list,
  String_t          id
)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp (list->Id, id) == 0) break ;

  return list ;
}

/******************************************************************************/

Bool_t check_intersection
(
  FloorplanElement* floorplan_element_a,
  FloorplanElement* floorplan_element_b
)
{
  if ((floorplan_element_a->SW_X + floorplan_element_a->Length)
         <= floorplan_element_b->SW_X
      || floorplan_element_a->SW_X
           >= (floorplan_element_b->SW_X + floorplan_element_b->Length))

    return FALSE_V ;

  if ((floorplan_element_a->SW_Y + floorplan_element_a->Width)
         <= floorplan_element_b->SW_Y
      || floorplan_element_a->SW_Y
           >= (floorplan_element_b->SW_Y + floorplan_element_b->Width))

    return FALSE_V ;

  return TRUE_V ;
}

/******************************************************************************/

void get_max_temperature_floorplan_element
(
  FloorplanElement* floorplan_element,
  Dimensions*       dimensions,
  Temperature_t*    temperatures,
  Temperature_t*    max_temperature
)
{
  RowIndex_t    row    = floorplan_element->SW_Row ;
  ColumnIndex_t column = floorplan_element->SW_Column ;

  *max_temperature
    = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  for
  (
    ;
   row <= floorplan_element->NE_Row ;
   row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column <= floorplan_element->NE_Column ;
      column++
    )
      *max_temperature
        = MAX
          (
            temperatures [get_cell_offset_in_layer (dimensions, row, column)],
            *max_temperature
          ) ;
}

/******************************************************************************/

void get_min_temperature_floorplan_element
(
  FloorplanElement* floorplan_element,
  Dimensions*       dimensions,
  Temperature_t*    temperatures,
  Temperature_t*    min_temperature
)
{
  RowIndex_t    row    = floorplan_element->SW_Row ;
  ColumnIndex_t column = floorplan_element->SW_Column ;

  *min_temperature
    = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  for
  (
    ;
    row <= floorplan_element->NE_Row ;
    row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column <= floorplan_element->NE_Column ;
      column++
    )
      *min_temperature
        = MIN
          (
            temperatures [get_cell_offset_in_layer (dimensions, row, column)],
            *min_temperature
          ) ;
}

/******************************************************************************/

void get_avg_temperature_floorplan_element
(
  FloorplanElement* floorplan_element,
  Dimensions*       dimensions,
  Temperature_t*    temperatures,
  Temperature_t*    avg_temperature
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;
  Quantity_t counter = 0 ;

  *avg_temperature = 0.0 ;

  for
  (
    row =  floorplan_element->SW_Row ;
    row <= floorplan_element->NE_Row ;
    row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column <= floorplan_element->NE_Column ;
      column++,
      counter++
    )

      *avg_temperature
        += temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  *avg_temperature /= (Temperature_t) counter ;
}

/******************************************************************************/

void get_min_avg_max_temperatures_floorplan_element
(
  FloorplanElement* floorplan_element,
  Dimensions*       dimensions,
  Temperature_t*    temperatures,
  Temperature_t*    min_temperature,
  Temperature_t*    avg_temperature,
  Temperature_t*    max_temperature
)
{
  RowIndex_t    row    = floorplan_element->SW_Row ;
  ColumnIndex_t column = floorplan_element->SW_Column ;
  Quantity_t counter = 0 ;
  Temperature_t temp ;

  *max_temperature = *min_temperature
    = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  *avg_temperature = 0.0 ;

  for
  (
    ;
    row <= floorplan_element->NE_Row ;
    row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column <= floorplan_element->NE_Column ;
      column++, counter++
    )
    {
      temp = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

      *max_temperature = MAX (temp, *max_temperature) ;

      *min_temperature = MIN (temp, *min_temperature) ;

      *avg_temperature += temp ;
    }

  *avg_temperature /= (Temperature_t) counter ;
}

/******************************************************************************/
