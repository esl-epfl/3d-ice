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

#ifndef _3DICE_THERMALCELL_H_
#define _3DICE_THERMALCELL_H_

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
    Conductance_t Top ;
    Conductance_t Bottom ;
    Conductance_t North ;
    Conductance_t South ;
    Conductance_t East ;
    Conductance_t West ;

    Capacity_t    Capacity ;

  } ThermalCell ;

/******************************************************************************/

  void fill_solid_cell_bottom
  (
#   ifdef PRINT_THERMAL_CELLS
    Dimensions*           dimensions,
    GridDimension_t       layer_index,
    GridDimension_t       row_index,
    GridDimension_t       column_index,
#   endif
    ThermalCell*           thermalcell,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    SolidTC_t              solid_tc,
    SolidVHC_t             solid_vhc,
    Time_t                 delta_time
  ) ;

/******************************************************************************/

  void fill_solid_cell_central
  (
#   ifdef PRINT_THERMAL_CELLS
    Dimensions*           dimensions,
    GridDimension_t       layer_index,
    GridDimension_t       row_index,
    GridDimension_t       column_index,
#   endif
    ThermalCell*           thermalcell,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    SolidTC_t              solid_tc,
    SolidVHC_t             solid_vhc,
    Time_t                 delta_time
  ) ;

/******************************************************************************/

  void fill_solid_cell_top
  (
#   ifdef PRINT_THERMAL_CELLS
    Dimensions*           dimensions,
    GridDimension_t       layer_index,
    GridDimension_t       row_index,
    GridDimension_t       column_index,
#   endif
    ThermalCell*           thermalcell,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    SolidTC_t              solid_tc,
    SolidVHC_t             solid_vhc,
    Time_t                 delta_time
  ) ;

/******************************************************************************/

  void fill_solid_cell_conventional_heat_sink
  (
#   ifdef PRINT_THERMAL_CELLS
    Dimensions*           dimensions,
    GridDimension_t       layer_index,
    GridDimension_t       row_index,
    GridDimension_t       column_index,
#   endif
    ThermalCell*          thermalcell,
    CellDimension_t       cell_length,
    CellDimension_t       cell_width,
    CellDimension_t       cell_height,
    SolidTC_t             thermal_conductivity,
    AmbientHTC_t          ambient_htc
  ) ;

/******************************************************************************/

  void fill_liquid_cell
  (
#   ifdef PRINT_THERMAL_CELLS
    GridDimension_t       layer_index,
    GridDimension_t       row_index,
    GridDimension_t       column_index,
#   endif
    Dimensions*            dimensions,
    ThermalCell*           thermalcell,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    CoolantHTCs_t          coolant_htcs,
    CoolantVHC_t           coolant_vhc,
    CoolantFR_t            coolant_fr,
    Time_t                 delta_time
  ) ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMALCELL_H_ */
