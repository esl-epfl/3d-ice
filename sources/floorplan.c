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

#include "floorplan.h"
#include "macros.h"
#include "../bison/floorplan_parser.h"
#include "../flex/floorplan_scanner.h"

/******************************************************************************/

extern int floorplan_parse
(
  Floorplan*  floorplan,
  Dimensions* dimensions,
  yyscan_t    scanner
) ;

/******************************************************************************/

void init_floorplan (Floorplan* floorplan)
{
  floorplan->FileName     = STRING_I ;
  floorplan->NElements    = QUANTITY_I ;
  floorplan->ElementsList = NULL ;
}

/******************************************************************************/

Floorplan* alloc_and_init_floorplan (void)
{
  Floorplan* floorplan = malloc (sizeof(Floorplan));

  if (floorplan != NULL) init_floorplan (floorplan) ;

  return floorplan ;
}

/******************************************************************************/

void free_floorplan (Floorplan* floorplan)
{
  FREE_POINTER (free_floorplan_elements_list, floorplan->ElementsList) ;
  FREE_POINTER (free,                         floorplan->FileName) ;
  FREE_POINTER (free,                         floorplan) ;
}

/******************************************************************************/

int fill_floorplan (Floorplan* floorplan, Dimensions* dimensions)
{
  FILE*    input ;
  int      result ;
  yyscan_t scanner ;

  input = fopen (floorplan->FileName, "r") ;
  if(input == NULL)
  {
    perror(floorplan->FileName) ;
    return 1 ;
  }

  floorplan_lex_init  (&scanner) ;
  floorplan_set_in    (input, scanner) ;
  //floorplan_set_debug (1, scanner) ;

  result = floorplan_parse (floorplan, dimensions, scanner) ;

  floorplan_lex_destroy (scanner) ;
  fclose (input) ;

  return result ;
}

/******************************************************************************/

void print_floorplan (FILE* stream, String_t prefix, Floorplan* floorplan)
{
  fprintf(stream,
    "%sFloorplan read from file %s\n", prefix, floorplan->FileName) ;

  print_detailed_floorplan_elements_list (stream, prefix, floorplan->ElementsList) ;
}

/******************************************************************************/

Error_t fill_sources_floorplan
(
    Source_t   *sources,
    Dimensions *dimensions,
    Floorplan  *floorplan
)
{
    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement, floorplan_element, floorplan->ElementsList)

        if (fill_sources_floorplan_element (sources, dimensions, floorplan_element) == TDICE_FAILURE)

            return TDICE_FAILURE ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void init_power_values_floorplan
(
  Floorplan*   floorplan
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement, floorplan_element,
                                     floorplan->ElementsList)

    init_power_values_floorplan_element
    (
      floorplan_element
    ) ;
}

/******************************************************************************/

void insert_power_values_floorplan
(
  Floorplan*   floorplan,
  PowersQueue* pvalues
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement, floorplan_element,
                                     floorplan->ElementsList)

    insert_power_values_floorplan_element
    (
      floorplan_element,
      pvalues
    ) ;
}

/******************************************************************************/

int get_max_temperature_floorplan
(
  Floorplan*     floorplan,
  String_t       floorplan_element_id,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* max_temperature
)
{
  FloorplanElement* flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;
  if (flp_el == NULL)

    return -3 ;

  get_max_temperature_floorplan_element
  (
    flp_el,
    dimensions,
    temperatures,
    max_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_min_temperature_floorplan
(
  Floorplan*     floorplan,
  String_t       floorplan_element_id,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* min_temperature
)
{
  FloorplanElement* flp_el = find_floorplan_element_in_list
                             (
                                floorplan->ElementsList,
                                floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_min_temperature_floorplan_element
  (
    flp_el,
    dimensions,
    temperatures,
    min_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_avg_temperature_floorplan
(
  Floorplan*     floorplan,
  String_t       floorplan_element_id,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* avg_temperature
)
{
  FloorplanElement* flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_avg_temperature_floorplan_element
  (
    flp_el,
    dimensions,
    temperatures,
    avg_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_min_avg_max_temperatures_floorplan
(
  Floorplan*     floorplan,
  String_t       floorplan_element_id,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* min_temperature,
  Temperature_t* avg_temperature,
  Temperature_t* max_temperature
)
{
  FloorplanElement* flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_min_avg_max_temperatures_floorplan_element
  (
    flp_el,
    dimensions,
    temperatures,
    min_temperature, avg_temperature, max_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_max_temperatures_floorplan
(
  Floorplan*     floorplan,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* max_temperature
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement,
                                     flp_el,
                                     floorplan->ElementsList)

    get_max_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      max_temperature++
    ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_min_temperatures_floorplan
(
  Floorplan*     floorplan,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* min_temperature
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement,
                                     flp_el,
                                     floorplan->ElementsList)

    get_min_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      min_temperature++
    ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_avg_temperatures_floorplan
(
  Floorplan*     floorplan,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* avg_temperature
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement,
                                     flp_el,
                                     floorplan->ElementsList)

    get_avg_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      avg_temperature++
    ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_min_avg_max_temperatures_floorplan
(
  Floorplan*     floorplan,
  Dimensions*    dimensions,
  Temperature_t* temperatures,
  Temperature_t* min_temperature,
  Temperature_t* avg_temperature,
  Temperature_t* max_temperature
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement,
                                     flp_el,
                                     floorplan->ElementsList)

    get_min_avg_max_temperatures_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      min_temperature++, avg_temperature++, max_temperature++
    ) ;

  return 0 ;
}

/******************************************************************************/
