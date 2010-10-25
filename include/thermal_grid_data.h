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

#ifndef _3DICE_THERMALGRIDDATA_H_
#define _3DICE_THERMALGRIDDATA_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"
#include "dimensions.h"

/******************************************************************************/

  /* The type of a layer */

  typedef enum
  {
    TDICE_LAYER_NONE = 0,
    TDICE_LAYER_SOLID,
    TDICE_LAYER_CHS,
    TDICE_LAYER_CHANNEL

  } Layer_t ;

  typedef enum
  {
    TDICE_CONDUCTANCE_NORTH = 0,
    TDICE_CONDUCTANCE_SOUTH,
    TDICE_CONDUCTANCE_EAST,
    TDICE_CONDUCTANCE_WEST,
    TDICE_CONDUCTANCE_TOP,
    TDICE_CONDUCTANCE_BOTTOM

  } Conductance_d ;

  typedef struct
  {
    Layer_t         Type ;
    SolidTC_t       ThermalConductivity ;
    SolidVHC_t      SolidVHC ;
    CellDimension_t Height ;

  } PerLayerData ;

  typedef struct
  {
    PerLayerData* LayersData ;

    CoolantVHC_t  CoolantVHC  ;
    CoolantHTCs_t CoolantHTCs ;
    CoolantFR_t   CoolantFR   ;

    AmbientHTC_t AmbientHTC ;

    Time_t DeltaTime ;

  } ThermalGridData ;

/******************************************************************************/

  void init_thermal_grid_data (ThermalGridData* thermalgriddata) ;

/******************************************************************************/

  int alloc_thermal_grid_data
  (
    ThermalGridData* thermalgriddata,
    Quantity_t       quantity,
    CoolantVHC_t     coolant_vhc,
    CoolantHTCs_t    coolant_htcs,
    CoolantFR_t      coolant_fr,
    AmbientHTC_t     ambient_htc,
    Time_t           delta_time
  ) ;

/******************************************************************************/

  void free_thermal_grid_data (ThermalGridData* thermalgriddata) ;

/******************************************************************************/

  void fill_thermal_grid_data
  (
    ThermalGridData* thermalgriddata,
    GridDimension_t  layer_index,
    Layer_t          type,
    SolidTC_t        thermal_conductivity,
    SolidVHC_t       solid_vhc,
    CellDimension_t  height
  ) ;

/******************************************************************************/

  Capacity_t get_capacity
  (
    ThermalGridData* thermalgriddata,
    Dimensions*      dimensions,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index
  ) ;

/******************************************************************************/

  Conductance_t get_conductance
  (
    ThermalGridData* thermalgriddata,
    Dimensions*      dimensions,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    Conductance_d    direction
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMALGRIDDATA_H_ */
