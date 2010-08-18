/******************************************************************************
 * Header file "3D-ICe/include/floorplan.h"                                   *
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

    Quantity_t NTimeSlots ;
    Quantity_t CurrentTimeSlot ;

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

//  void
//  insert_power_values_floorplan              (
//                                              Floorplan* floorplan,
//                                              Power_t*   power_values
//                                             ) ;

//  int
//  insert_power_value_floorplan_element       (
//                                               Floorplan* floorplan,
//                                               String_t   floorplan_element_id,
//                                               Power_t*   power_value
//                                             ) ;

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
