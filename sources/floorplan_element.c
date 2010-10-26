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
#include "macros.h"

/******************************************************************************/

void init_floorplan_element (FloorplanElement* floorplan_element)
{
  floorplan_element->Id              = STRING_I ;

  floorplan_element->SW_X            = CELLDIMENSION_I ;
  floorplan_element->SW_Y            = CELLDIMENSION_I ;

  floorplan_element->Length          = CELLDIMENSION_I ;
  floorplan_element->Width           = CELLDIMENSION_I ;

  floorplan_element->EffectiveLength = CELLDIMENSION_I ;
  floorplan_element->EffectiveWidth  = CELLDIMENSION_I ;

  floorplan_element->SW_Row          = GRIDDIMENSION_I ;
  floorplan_element->SW_Column       = GRIDDIMENSION_I ;

  floorplan_element->NE_Row          = GRIDDIMENSION_I ;
  floorplan_element->NE_Column       = GRIDDIMENSION_I ;

  floorplan_element->PowerValues     = NULL ;

  floorplan_element->Next            = NULL ;
}

/******************************************************************************/

FloorplanElement* alloc_and_init_floorplan_element (void)
{
  FloorplanElement* floorplan_element ;

  MALLOC (floorplan_element, 1) ;

  if (floorplan_element != NULL)  init_floorplan_element(floorplan_element) ;

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
  FREE_LIST (FloorplanElement, list, free_floorplan_element) ;
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
                          floorplan_element->SW_X, floorplan_element->SW_Y) ;
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
  FOR_EVERY_ELEMENT_IN_LIST (FloorplanElement, flp_el, list)

    print_floorplan_element (stream, prefix, flp_el) ;
}

/******************************************************************************/

void print_formatted_floorplan_elements_list
(
  FILE*             stream,
  FloorplanElement* list
)
{
  FOR_EVERY_ELEMENT_IN_LIST (FloorplanElement, flp_el, list)
  {
    print_formatted_floorplan_element (stream, flp_el) ;
    fprintf(stream, "\n") ;
  }
}

/******************************************************************************/

FloorplanElement* find_floorplan_element_in_list
(
  FloorplanElement* list,
  String_t          id
)
{
  FOR_EVERY_ELEMENT_IN_LIST (FloorplanElement, flp_el, list)

    if (strcmp (flp_el->Id, id) == 0) break ;

  return flp_el ;
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
  *max_temperature = temperatures
  [
    get_cell_offset_in_layer
    (
      dimensions,
      FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(floorplan_element),
      FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(floorplan_element)
    )
  ] ;

  FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
  {
    FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
    {

      *max_temperature = MAX
      (
        temperatures [get_cell_offset_in_layer (dimensions,
                                                row_index, column_index)],
        *max_temperature
      ) ;

    } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
  } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

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
  *min_temperature = temperatures
  [
    get_cell_offset_in_layer
    (
      dimensions,
      FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(floorplan_element),
      FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(floorplan_element)
    )
  ] ;

  FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
  {
    FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
    {

      *min_temperature = MIN
      (
        temperatures [get_cell_offset_in_layer (dimensions,
                                                row_index, column_index)],
        *min_temperature
      ) ;

    } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
  } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

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
  Quantity_t counter = QUANTITY_I ;

  *avg_temperature = TEMPERATURE_I ;

  FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
  {
    FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
    {

      *avg_temperature
        += temperatures [get_cell_offset_in_layer (dimensions,
                                                   row_index, column_index)] ;

      counter++ ;

    } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
  } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

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
  Quantity_t    counter = QUANTITY_I ;
  Temperature_t tmp     = TEMPERATURE_I ;

  *avg_temperature = TEMPERATURE_I ;
  *max_temperature = *min_temperature = temperatures
  [
    get_cell_offset_in_layer
    (
      dimensions,
      FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(floorplan_element),
      FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(floorplan_element)
    )
  ] ;

  FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
  {
    FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
    {
      tmp = temperatures [get_cell_offset_in_layer (dimensions,
                                                    row_index, column_index)] ;

      *max_temperature = MAX (tmp, *max_temperature) ;

      *min_temperature = MIN (tmp, *min_temperature) ;

      *avg_temperature += tmp ;

      counter++ ;

    } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
  } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

  *avg_temperature /= (Temperature_t) counter ;
}

/******************************************************************************/
