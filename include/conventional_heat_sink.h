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

#ifndef _3DICE_CONVENTIONAL_HEAT_SINK_H_
#define _3DICE_CONVENTIONAL_HEAT_SINK_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"
#include "thermal_cell.h"
#include "layer.h"

/******************************************************************************/

  typedef struct
  {
    /* The heat transfert coefficient */

    AmbientHTC_t AmbientHTC ;

    /* The temperarute of the environment in [K] */

    Temperature_t AmbientTemperature ;

    /* Pointer to the top layer in the 3DIC */

    Layer* TopLayer ;

    /* Is the top layer a source layer ? */

    Bool_t IsSourceLayer ;

  } ConventionalHeatSink ;

/******************************************************************************/

  void init_conventional_heat_sink (ConventionalHeatSink* conventionalheatsink) ;

/******************************************************************************/

  ConventionalHeatSink* alloc_and_init_conventional_heat_sink (void) ;

/******************************************************************************/

  void free_conventional_heat_sink (ConventionalHeatSink* conventionalheatsink) ;

/******************************************************************************/

  void print_formatted_conventional_heat_sink
  (
    FILE*                 stream,
    String_t              prefix,
    ConventionalHeatSink* conventionalheatsink
  ) ;

/******************************************************************************/

  void print_detailed_conventional_heat_sink
  (
    FILE*                 stream,
    String_t              prefix,
    ConventionalHeatSink* conventionalheatsink
  ) ;

/******************************************************************************/

    /*! Applies the conventional heat sink to the grid of thermal cells
     *
     *  The function adapts the top-most layer of the 3d stack to connect it to
     *  the enviroment and to enable heat dissipation.
     *
     *  \param thermal_cells          pointer to the first thermal cell in the 3d grid
     *  \param dimensions             pointer to the structure storing the dimensions
     *  \param conventional_heat_sink pointer to the heat sink structure
     */

    void fill_thermal_cell_conventional_heat_sink
    (
       ThermalCell          *thermal_cells,
       Dimensions           *dimensions,
       ConventionalHeatSink *conventional_heat_sink
    ) ;

/******************************************************************************/

    /*! Fills the source vector to enable heat dissipation through the heat sink
     *
     *  \param sources                pointer to the first element in the source vector
     *  \param thermal_cells          pointer to the first thermal cell in the 3d grid
     *  \param dimensions             pointer to the structure storing the dimensions
     *  \param conventional_heat_sink pointer to the heat sink structure
     */

    void fill_sources_conventional_heat_sink
    (
        Source_t             *sources,
        ThermalCell          *thermal_cells,
        Dimensions           *dimensions,
        ConventionalHeatSink *conventional_heat_sink
    ) ;

/******************************************************************************/

  void fill_system_matrix_conventional_heat_sink
  (
    SystemMatrix          system_matrix,
    Dimensions*           dimensions,
    ThermalCell*          thermalcells
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CONVENTIONAL_HEAT_SINK_H_ */
