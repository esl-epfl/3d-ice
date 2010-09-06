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

#ifndef _3DICE_LAYER_H_
#define _3DICE_LAYER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"
#include "material.h"
#include "dimensions.h"
#include "conductances.h"
#include "floorplan.h"
#include "environment_heat_sink.h"

/******************************************************************************/

  struct Layer
  {
    /* The heigh of the layer in um. (1 cell) */

    CellDimension_t Height ;

    /* The material composing the layer */

    Material* Material ;

    /* The offset (#of layers) counting from the first layer in the die */

    LayerIndex_t LayersOffset ;

    /* To collect layers in a linked list */

    struct Layer* Next ;
  } ;

  typedef struct Layer Layer;

/******************************************************************************/

  void init_layer (Layer* layer) ;

/******************************************************************************/

  Layer* alloc_and_init_layer (void) ;

/******************************************************************************/

  void free_layer (Layer* layer) ;

/******************************************************************************/

  void free_layers_list (Layer* list) ;

/******************************************************************************/

  void print_layer (FILE* stream, String_t prefix, Layer* layer) ;

/******************************************************************************/

  void print_layers_list (FILE* stream, String_t prefix, Layer* list) ;

/******************************************************************************/

  Conductances* fill_conductances_layer
  (
    Layer*               layer,
    Conductances*        conductances,
    Dimensions*          dimensions,
    EnvironmentHeatSink* environmentheatsink,
    LayerIndex_t         current_layer
  ) ;

/******************************************************************************/

  Capacity_t* fill_capacities_layer
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t current_layer,
#   endif
    Layer*       layer,
    Capacity_t*  capacities,
    Dimensions*  dimensions,
    Time_t       delta_time
  ) ;

/******************************************************************************/

  Source_t* fill_sources_active_layer
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t      current_layer,
    Layer*            layer,
#   endif
    Floorplan*        floorplan,
    Source_t*         sources,
    Dimensions*       dimensions
  ) ;

/******************************************************************************/

  Source_t* fill_sources_empty_layer
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t current_layer,
    Layer*       layer,
#   endif
    Source_t*    sources,
    Dimensions*  dimensions
  ) ;

/******************************************************************************/

  Quantity_t fill_system_matrix_layer
  (
#   ifdef PRINT_SYSTEM_MATRIX
    Layer*               layer,
#   endif
    Dimensions*          dimensions,
    Conductances*        conductances,
    Capacity_t*          capacities,
    EnvironmentHeatSink* environmentheatsink,
    LayerIndex_t         current_layer,
    ColumnIndex_t*       column_pointers,
    RowIndex_t*          row_indices,
    SystemMatrixValue_t* values
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_LAYER_H_ */
