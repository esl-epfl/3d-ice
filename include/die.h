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

#ifndef _3DICE_DIE_H_
#define _3DICE_DIE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"
#include "layer.h"
#include "conductances.h"
#include "floorplan.h"
#include "environment_heat_sink.h"

/******************************************************************************/

  struct Die
  {
    /* The id (string) of the die */

    String_t Id ;

    /* The list of layers composing the die (from bottom to top) */

    Layer* LayersList ;

    /* Pointer to the source layer */

    Layer* SourceLayer ;

    /* The number of layer composing the die */

    Quantity_t NLayers ;

    /* To collect dies in a linked list */

    struct Die* Next ;

  } ;

  typedef struct Die Die ;

/******************************************************************************/

  void init_die (Die* die) ;

/******************************************************************************/

  Die* alloc_and_init_die (void) ;

/******************************************************************************/

  void free_die (Die* die) ;

/******************************************************************************/

  void free_dies_list (Die* list) ;

/******************************************************************************/

  void print_die (FILE* stream, String_t prefix, Die* die) ;

/******************************************************************************/

  void print_dies_list (FILE* stream, String_t prefix, Die* list) ;

/******************************************************************************/

  Die* find_die_in_list (Die* list, String_t id) ;

/******************************************************************************/

  Conductances* fill_conductances_die
  (
    Die*                 die,
    Conductances*        conductances,
    Dimensions*          dimensions,
    EnvironmentHeatSink* environmentheatsink,
    LayerIndex_t         current_layer
  ) ;

/******************************************************************************/

  Capacity_t* fill_capacities_die
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t  current_layer,
#   endif
    Die*          die,
    Capacity_t*   capacities,
    Dimensions*   dimensions,
    Time_t        delta_time
  ) ;

/******************************************************************************/

  Source_t*      fill_sources_die
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t current_layer,
#   endif
    Die*         die,
    Floorplan*   floorplan,
    Source_t*    sources,
    Dimensions*  dimensions
  ) ;

/******************************************************************************/

  Quantity_t fill_system_matrix_die
  (
    Die*                 die,
    Dimensions*          dimensions,
    Conductances*        conductances,
    Capacity_t*          capacities,
    EnvironmentHeatSink* environmentheatsink,
    LayerIndex_t         current_layer,
    RowIndex_t*          row_offsets,
    ColumnIndex_t*       column_indices,
    SystemMatrixValue_t* values
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
