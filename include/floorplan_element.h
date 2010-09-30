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

#ifndef _3DICE_FLOORPLAN_ELEMENT_H_
#define _3DICE_FLOORPLAN_ELEMENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"
#include "dimensions.h"
#include "powers_queue.h"

  struct FloorplanElement
  {
    /* The id (string) of the floorplan element */

    String_t Id ;

    /* The south-west X position [um] */

    CellDimension_t SW_X ;

    /* The south-west Y position [um] */

    CellDimension_t SW_Y ;

    /* The length of the floorplan element [um] */

    CellDimension_t Length ;

    /* The width of the floorplan element [um] */

    CellDimension_t Width ;

    /* The length of the floorplan element [um] */
    /* considering the distribution of cells    */

    CellDimension_t EffectiveLength ;

    /* The width of the floorplan element [um] */
    /* considering the distribution of cells    */

    CellDimension_t EffectiveWidth ;

    /* The south-west row index */

    RowIndex_t SW_Row ;

    /* The south-west column index */

    ColumnIndex_t SW_Column ;

    /* The north-east row index */

    RowIndex_t NE_Row ;

    /* The north-east column index */

    ColumnIndex_t NE_Column ;

    /* The list of power values */

    PowersQueue* PowerValues;

    /* To collect floorplan elements in a linked list */

    struct FloorplanElement* Next ;

  } ;

  typedef struct FloorplanElement FloorplanElement ;

/******************************************************************************/

  void init_floorplan_element (FloorplanElement* floorplan_element) ;

/******************************************************************************/

  FloorplanElement* alloc_and_init_floorplan_element (void) ;

/******************************************************************************/

  void free_floorplan_element (FloorplanElement* floorplan_element) ;

/******************************************************************************/

  void free_floorplan_elements_list (FloorplanElement* list) ;

/******************************************************************************/

  FloorplanElement* find_floorplan_element_in_list
  (
    FloorplanElement* list,
    String_t id
  ) ;

/******************************************************************************/

  void print_floorplan_element
  (
    FILE*             stream,
    String_t          prefix,
    FloorplanElement* floorplan_element
  ) ;

/******************************************************************************/

  void print_formatted_floorplan_element
  (
    FILE*             stream,
    FloorplanElement* floorplan_element
  ) ;

/******************************************************************************/

  void print_floorplan_elements_list
  (
    FILE*             stream,
    String_t          prefix,
    FloorplanElement* list
  ) ;

/******************************************************************************/

  void print_formatted_floorplan_elements_list
  (
    FILE*             stream,
    FloorplanElement* list
  ) ;

/******************************************************************************/

  Bool_t check_intersection
  (
    FloorplanElement* floorplan_element_a,
    FloorplanElement* floorplan_element_b
  ) ;

/******************************************************************************/

  void get_max_temperature_floorplan_element
  (
    FloorplanElement* floorplan_element,
    Dimensions*       dimensions,
    Temperature_t*    temperatures,
    Temperature_t*    max_temperature
  );

/******************************************************************************/

  void get_min_temperature_floorplan_element
  (
    FloorplanElement* floorplan_element,
    Dimensions*       dimensions,
    Temperature_t*    temperatures,
    Temperature_t*    min_temperature
  );

/******************************************************************************/

  void get_avg_temperature_floorplan_element
  (
    FloorplanElement* floorplan_element,
    Dimensions*       dimensions,
    Temperature_t*    temperatures,
    Temperature_t*    avg_temperature
  );

/******************************************************************************/

  void get_min_avg_max_temperatures_floorplan_element
  (
    FloorplanElement* floorplan_element,
    Dimensions*       dimensions,
    Temperature_t*    temperatures,
    Temperature_t*    min_temperature,
    Temperature_t*    avg_temperature,
    Temperature_t*    max_temperature
  );

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_ELEMENT_H_ */
