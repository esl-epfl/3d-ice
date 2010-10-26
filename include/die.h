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

#ifndef _3DICE_DIE_H_
#define _3DICE_DIE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"
#include "layer.h"
#include "thermal_grid_data.h"
#include "floorplan.h"
#include "conventional_heat_sink.h"
#include "system_matrix.h"

/******************************************************************************/

  struct Die
  {
    /* The id (string) of the die */

    String_t Id ;

    /* For parsing purpose */

    Quantity_t Used ;

    /* The number of layer composing the die */

    GridDimension_t NLayers ;

    /* The list of layers composing the die (from bottom to top) */

    Layer* LayersList ;

    /* Pointer to the source layer */

    Layer* SourceLayer ;

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

  void print_formatted_die
  (
    FILE*    stream,
    String_t prefix,
    Die*     die
  ) ;

/******************************************************************************/

  void print_detailed_die
  (
    FILE*    stream,
    String_t prefix,
    Die*     die
  ) ;

/******************************************************************************/

  void print_formatted_dies_list
  (
    FILE*    stream,
    String_t prefix,
    Die*     list
  ) ;

/******************************************************************************/

  void print_detailed_dies_list
  (
    FILE*    stream,
    String_t prefix,
    Die*     list
  ) ;

/******************************************************************************/

  Die* find_die_in_list (Die* list, String_t id) ;

/******************************************************************************/

  void fill_thermal_grid_data_die
  (
    ThermalGridData* thermalgriddata,
    GridDimension_t  layer_index,
    Die*             die
  ) ;

/******************************************************************************/

  Source_t*               fill_sources_die
  (
    GridDimension_t       layer_index,
    Die*                  die,
    ConventionalHeatSink* conventionalheatsink,
    ThermalGridData*      thermalgriddata,
    Floorplan*            floorplan,
    Source_t*             sources,
    Dimensions*           dimensions
  ) ;

/******************************************************************************/

  SystemMatrix fill_system_matrix_die
  (
    Die*                  die,
    Dimensions*           dimensions,
    ThermalGridData*      thermalgriddata,
    GridDimension_t       layer_index,
    SystemMatrix          system_matrix
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
