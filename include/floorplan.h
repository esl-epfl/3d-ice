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

#ifndef _3DICE_FLOORPLAN_H_
#define _3DICE_FLOORPLAN_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "dimensions.h"
#include "floorplan_element.h"

  typedef struct
  {
    /* The name of the file scanned to fill the floorplan */

    String_t FileName ;

    /* The number of floorplan elements in the floorplan  */

    Quantity_t NElements ;

    /* The list of floorplan elements */

    FloorplanElement* ElementsList ;

  } Floorplan ;

/******************************************************************************/

  void init_floorplan (Floorplan* floorplan) ;

/******************************************************************************/

  Floorplan* alloc_and_init_floorplan (void) ;

/******************************************************************************/

  void free_floorplan (Floorplan* floorplan) ;

/******************************************************************************/

  int fill_floorplan (Floorplan* floorplan, Dimensions* dim) ;

/******************************************************************************/

  void print_floorplan (FILE* stream, String_t prefix, Floorplan* floorplan) ;

/******************************************************************************/

  Bool_t check_intersections
  (
    Floorplan*        floorplan,
    FloorplanElement* floorplan_element
  ) ;

/******************************************************************************/

  Bool_t check_location
  (
    Floorplan*        floorplan,
    FloorplanElement* floorplan_element,
    Dimensions*       dimensions
  ) ;

/******************************************************************************/

  Bool_t align_to_grid
  (
    Floorplan*        floorplan,
    FloorplanElement* floorplan_element,
    Dimensions*       dimensions
  ) ;

/******************************************************************************/

  int get_max_temperature_floorplan
  (
    Floorplan*     floorplan,
    String_t       floorplan_element_id,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* max_temperature
  );

/******************************************************************************/

  int get_min_temperature_floorplan
  (
    Floorplan*     floorplan,
    String_t       floorplan_element_id,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* min_temperature
  );

/******************************************************************************/

  int get_avg_temperature_floorplan
  (
    Floorplan*     floorplan,
    String_t       floorplan_element_id,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* avg_temperature
  );

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
  );

/******************************************************************************/

  int get_all_max_temperatures_floorplan
  (
    Floorplan*     floorplan,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* max_temperature
  );

/******************************************************************************/

  int get_all_min_temperatures_floorplan
  (
    Floorplan*     floorplan,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* min_temperature
  );

/******************************************************************************/

  int get_all_avg_temperatures_floorplan
  (
    Floorplan*     floorplan,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* avg_temperature
  );

/******************************************************************************/

  int get_all_min_avg_max_temperatures_floorplan
  (
    Floorplan*     floorplan,
    Dimensions*    dimensions,
    Temperature_t* temperatures,
    Temperature_t* min_temperature,
    Temperature_t* avg_temperature,
    Temperature_t* max_temperature
  );

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_H_ */
