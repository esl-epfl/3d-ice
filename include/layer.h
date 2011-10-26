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

#ifndef _3DICE_LAYER_H_
#define _3DICE_LAYER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"
#include "material.h"
#include "dimensions.h"
#include "thermal_cell.h"
#include "floorplan.h"
#include "system_matrix.h"

/******************************************************************************/

  struct Layer
  {
    /* The heigh of the layer in um. (1 cell) */

    CellDimension_t Height ;

    /* The offset (#of layers) counting from the first layer in the die */

    GridDimension_t Offset ;

    /* The material composing the layer */

    Material* Material ;

    /* To collect layers in a double linked list */

    struct Layer* Next ;
    struct Layer* Prev ;
  } ;

  typedef struct Layer Layer ;

/******************************************************************************/

  void init_layer (Layer* layer) ;

/******************************************************************************/

  Layer* alloc_and_init_layer (void) ;

/******************************************************************************/

  void free_layer (Layer* layer) ;

/******************************************************************************/

  void free_layers_list (Layer* list) ;

/******************************************************************************/

  void print_formatted_layer
  (
    FILE*    stream,
    String_t prefix,
    Layer*   layer
  ) ;

/******************************************************************************/

  void print_detailed_layer
  (
    FILE*    stream,
    String_t prefix,
    Layer*   layer
  ) ;

/******************************************************************************/

  void print_formatted_layers_list
  (
    FILE*    stream,
    String_t prefix,
    Layer*   list
  ) ;

/******************************************************************************/

  void print_detailed_layers_list
  (
    FILE*    stream,
    String_t prefix,
    Layer*   list
  ) ;

/******************************************************************************/

  void fill_thermal_cell_layer
  (
    ThermalCell*     thermalcells,
    Time_t           delta_time,
    Dimensions*      dimensions,
    GridDimension_t  layer_index,
    Layer*           layer
  ) ;

/******************************************************************************/

  int fill_sources_layer
  (
    Source_t*             sources,
    Dimensions*           dimensions,
    GridDimension_t       layer_index,
    Floorplan*            floorplan,
    Layer*                layer
  ) ;

/******************************************************************************/

  SystemMatrix fill_system_matrix_layer
  (
    Layer*                layer,
    Dimensions*           dimensions,
    ThermalCell*          thermalcells,
    GridDimension_t       layer_index,
    SystemMatrix          system_matrix
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_LAYER_H_ */
