/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE, version 1.0 .                                 *
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
 * Bstiment ELG, ELG 130                                                      *
 * Station 11                                           3d-ice@listes.epfl.ch *
 * 1015 Lausanne, Switzerland                  http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_DIMENSIONS_H_
#define _3DICE_DIMENSIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

#include <stdio.h>

  typedef struct
  {
    CellDimension_t FirstWallLength ;
    CellDimension_t WallLength ;
    CellDimension_t LastWallLength ;

    CellDimension_t ChannelLength ;

    CellDimension_t Width ;
    CellDimension_t Length ;

  } CellDimensions ;

/******************************************************************************/

  typedef struct
  {
    GridDimension_t NLayers ;
    GridDimension_t NRows ;
    GridDimension_t NColumns ;
    GridDimension_t NCells ;
    GridDimension_t NNz ;

  } GridDimensions ;

/******************************************************************************/

  typedef struct
  {
    ChipDimension_t Length ;
    ChipDimension_t Width ;

  } ChipDimensions ;

/******************************************************************************/

  typedef struct
  {
    CellDimensions Cell;
    GridDimensions Grid;
    ChipDimensions Chip;

    Bool_t StackHasChannel ;

  } Dimensions ;

/******************************************************************************/

  void init_dimensions (Dimensions* dimensions) ;

/******************************************************************************/

  Dimensions* alloc_and_init_dimensions (void) ;

/******************************************************************************/

  void free_dimensions (Dimensions* dimensions) ;

/******************************************************************************/

  void
  print_dimensions (FILE* stream, String_t prefix, Dimensions* dimensions) ;

/******************************************************************************/

  CellDimension_t
  get_cell_length (Dimensions* dimensions, ColumnIndex_t column) ;

  CellDimension_t
  get_cell_width  (Dimensions* dimensions) ;

/******************************************************************************/

  GridDimension_t get_number_of_layers     (Dimensions* dimensions) ;

  GridDimension_t get_number_of_rows       (Dimensions* dimensions) ;

  GridDimension_t get_number_of_columns    (Dimensions* dimensions) ;

  GridDimension_t get_number_of_cells      (Dimensions* dimensions) ;

  GridDimension_t get_number_of_non_zeroes (Dimensions* dimensions) ;

/******************************************************************************/

  Quantity_t get_layer_area (Dimensions* dimensions) ;

  Quantity_t get_cell_offset_in_layer
  (
    Dimensions*   dimensions,
    RowIndex_t    row,
    ColumnIndex_t column
  ) ;

  Quantity_t get_cell_offset_in_stack
  (
    Dimensions*   dimensions,
    LayerIndex_t  layer,
    RowIndex_t    row,
    ColumnIndex_t column
  ) ;

/******************************************************************************/

  ChipDimension_t get_chip_length (Dimensions* dimensions) ;

  ChipDimension_t get_chip_width (Dimensions* dimensions) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIMENSIONS_H_ */
