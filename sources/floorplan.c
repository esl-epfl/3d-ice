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

#include "floorplan.h"
#include "../bison/floorplan_parser.h"
#include "../flex/floorplan_scanner.h"

extern int floorplan_parse
(
  Floorplan*  floorplan,
  Dimensions* dimensions,
  yyscan_t    scanner
) ;

/******************************************************************************/

void init_floorplan (Floorplan* floorplan)
{
  floorplan->FileName          = NULL ;
  floorplan->NElements         = 0 ;
  floorplan->ElementsList      = NULL ;
}

/******************************************************************************/

Floorplan* alloc_and_init_floorplan (void)
{
  Floorplan* floorplan = (Floorplan* ) malloc ( sizeof(Floorplan) );

  if (floorplan != NULL) init_floorplan (floorplan) ;

  return floorplan ;
}

/******************************************************************************/

void free_floorplan (Floorplan* floorplan)
{
  free_floorplan_elements_list (floorplan->ElementsList) ;
  free (floorplan->FileName) ;
  free (floorplan) ;
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
    return 1;
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

  print_floorplan_elements_list (stream, prefix, floorplan->ElementsList) ;
}

/******************************************************************************/

Bool_t check_intersections
(
  Floorplan*        floorplan,
  FloorplanElement* floorplan_element
)
{
  Bool_t result = FALSE_V ;
  FloorplanElement* tmp = floorplan->ElementsList ;

  for ( ; tmp != NULL; tmp = tmp->Next)

    if (check_intersection (tmp, floorplan_element) == TRUE_V)
    {
        fprintf (stderr,
          "%s: found intersection between %s and %s.\n",
          floorplan->FileName, tmp->Id, floorplan_element->Id) ;
        result ++ ;
    }

  return result ;
}

/******************************************************************************/

Bool_t check_location
(
  Floorplan*        floorplan,
  FloorplanElement* floorplan_element,
  Dimensions*       dimensions
)
{
  if (   (floorplan_element->SW_X <  0)
          || (floorplan_element->SW_X + floorplan_element->Length
              > get_chip_length (dimensions))
      || (floorplan_element->SW_Y <  0)
          || (floorplan_element->SW_Y + floorplan_element->Width
              > get_chip_width (dimensions)) )
  {
    fprintf (stderr,
      "%s: floorplan element %s is outside of the floorplan.\n",
      floorplan->FileName, floorplan_element->Id) ;
    return TRUE_V ;
  }

  return FALSE_V ;
}

/******************************************************************************/

Bool_t align_to_grid
(
  Floorplan*        floorplan,
  FloorplanElement* floorplan_element,
  Dimensions*       dimensions
)
{
  CellDimension_t cx, cy ;
  ColumnIndex_t column ;
  RowIndex_t row ;

  /* West side */

  cx     = get_cell_length (dimensions, 0) / 2.0 ;
  column = 0 ;

  while ( cx < floorplan_element->SW_X )
  {
    cx += (get_cell_length (dimensions, column) / 2.0)
          + (get_cell_length (dimensions, column + 1) / 2.0) ;
    column++ ;
  }

  floorplan_element->SW_Column = column ;

  /* East side */

  while (cx < floorplan_element->SW_X + floorplan_element->Length)
  {
    cx += get_cell_length (dimensions, column) / 2.0
          + (get_cell_length (dimensions, column + 1) / 2.0) ;

   column ++ ;
  }

  floorplan_element->NE_Column = column - 1;

  /* Effective length */

  for
  (
    column =  floorplan_element->SW_Column ;
    column <= floorplan_element->NE_Column ;
    column ++
  )
    floorplan_element->EffectiveLength += get_cell_length (dimensions, column) ;

  /* South side */

  cy  = (get_cell_width (dimensions) / 2.0);
  row = 0 ;

  while ( cy < floorplan_element->SW_Y )
  {
    cy += get_cell_width (dimensions) ;
    row++ ;
  }

  floorplan_element->SW_Row = row ;

  /* North side */

  while ( cy <= floorplan_element->SW_Y + floorplan_element->Width )
  {
    cy += get_cell_width (dimensions) ;
    row++ ;
  }

  floorplan_element->NE_Row = row - 1 ;

  /* Effective width */

  floorplan_element->EffectiveWidth =
    get_cell_width (dimensions)
    * (floorplan_element->NE_Row - floorplan_element->SW_Row + 1) ;

  if (floorplan_element->NE_Row - floorplan_element->SW_Row == 0
      && floorplan_element->NE_Column - floorplan_element->SW_Column == 0)
  {
    fprintf (stderr,
      "%s: no cells belong to floorplan element %s.\n",
      floorplan->FileName, floorplan_element->Id) ;
    return TRUE_V ;
  }

  return FALSE_V ;
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
  );

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
  );

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
  );

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
  );

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
  FloorplanElement* flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)

    get_max_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      max_temperature++
    );

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
  FloorplanElement* flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)
  {
    get_min_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      min_temperature++
    );
  }

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
  FloorplanElement* flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)

    get_avg_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      avg_temperature++
    );

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
  FloorplanElement* flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)

    get_min_avg_max_temperatures_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      min_temperature++, avg_temperature++, max_temperature++
    );

  return 0 ;
}

/******************************************************************************/
