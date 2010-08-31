/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
 ******************************************************************************/

#ifndef _3DICE_CONDUCTANCES_H_
#define _3DICE_CONDUCTANCES_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"
#include "dimensions.h"

/******************************************************************************/

  typedef struct
  {
    Conductance_t North;
    Conductance_t South;

    Conductance_t East;
    Conductance_t West;

    Conductance_t Top;
    Conductance_t Bottom;

  } Conductances ;

/******************************************************************************/

  void fill_conductances_bottom_solid_cell
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity,
    EnvironmentHTC_t       environment_heat_transfer_coefficient
  ) ;

/******************************************************************************/

  void fill_conductances_central_solid_cell
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity,
    EnvironmentHTC_t       environment_heat_transfer_coefficient
  ) ;

/******************************************************************************/

  void fill_conductances_top_solid_cell
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity,
    EnvironmentHTC_t       environment_heat_transfer_coefficient
  ) ;

/******************************************************************************/

  void fill_conductances_top_solid_cell_ehtc
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity,
    EnvironmentHTC_t       environment_heat_transfer_coefficient
  ) ;

/******************************************************************************/

  void fill_conductances_wall_cell
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity
  ) ;

/******************************************************************************/

  void fill_conductances_liquid_cell
  (
#   ifdef PRINT_CONDUCTANCES
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Dimensions*            dimensions,
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    CoolantHTCs_t          coolant_htcs,
    CoolantVHC_t           coolant_vhc,
    CoolantFR_t            coolant_fr
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif    /* _3DICE_CONDUCTANCES_H_ */
