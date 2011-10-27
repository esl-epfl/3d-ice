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
#include "thermal_cell.h"
#include "floorplan.h"
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

    /* The offset (#layers) of the source layer within the die */

    GridDimension_t SourceLayerOffset ;

    /* Pointer to the top-most layer */

    Layer* TopLayer ;

    /* Pointer to the source layer */

    Layer* SourceLayer ;

    /* Pointer to the bottom-most layer */

    Layer* BottomLayer ;

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

    /*! Fills the thermal cells corresponding to a die
     *
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param delta_time    the time resolution of the thermal simulation
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param layer_index   offset (#layers) of the die within the stack
     *  \param die           pointer to the die
     */

    void fill_thermal_cell_die
    (
        ThermalCell     *thermal_cells,
        Time_t           delta_time,
        Dimensions      *dimensions,
        GridDimension_t  layer_index,
        Die             *die
    ) ;

/******************************************************************************/

    /*! Fills the source vector corresponding to a die
     *
     *  \param sources     pointer to the first element in the source vector
     *  \param dimensions  pointer to the structure storing the dimensions
     *  \param layer_index offset (#layers) of the die within the stack
     *  \param floorplan   pointer to the floorplan placed on the source layer
     *  \param die         pointer to the die
     *
     *  \return \c TDICE_SUCCESS if the source vector has been filled correctly
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t fill_sources_die
    (
        Source_t        *sources,
        Dimensions      *dimensions,
        GridDimension_t  layer_index,
        Floorplan       *floorplan,
        Die             *die
    ) ;

/******************************************************************************/

  void init_power_values_die
  (
    Floorplan*            floorplan
  ) ;

/******************************************************************************/

  void insert_power_values_die
  (
    Floorplan*            floorplan,
    PowersQueue*          pvalues
  ) ;

/******************************************************************************/

  SystemMatrix fill_system_matrix_die
  (
    Die*                  die,
    Dimensions*           dimensions,
    ThermalCell*          thermalcells,
    GridDimension_t       layer_index,
    SystemMatrix          system_matrix
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
