/******************************************************************************
 *                                                                            *
 * Header file "Include/dimensions.h"                                         *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_DIMENSIONS_H_
#define _TL_DIMENSIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

#include <stdio.h>

/******************************************************************************
 *                                "Dimensions"                                *
 ******************************************************************************/

  struct CellDimensions
  {
    CellDimension_t FirstWallLength ;
    CellDimension_t LastWallLength ;
    CellDimension_t WallLength ;
    CellDimension_t ChannelLength ;
    CellDimension_t Width ;
  } ;

  typedef struct CellDimensions CellDimensions;

/******************************************************************************/

  struct GridDimensions
  {
    GridDimension_t NLayers ;
    GridDimension_t NRows ;
    GridDimension_t NColumns ;
    GridDimension_t NCells ;
    GridDimension_t NNz ;

  } ;

  typedef struct GridDimensions GridDimensions;

/******************************************************************************/

  struct ChipDimensions
  {
    ChipDimension_t Length ;
    ChipDimension_t Width ;
  } ;

  typedef struct ChipDimensions ChipDimensions;

/******************************************************************************/

  struct Dimensions
  {
    CellDimensions Cell;
    GridDimensions Grid;
    ChipDimensions Chip;

    int StackHasChannel ;
  } ;

  typedef struct Dimensions Dimensions ;

/******************************************************************************/

  void init_dimensions (Dimensions* dimensions) ;

  Dimensions* alloc_and_init_dimensions (void) ;

  void free_dimensions (Dimensions* dimensions) ;

  void print_dimensions (FILE* stream, String_t prefix, Dimensions* dimensions) ;

/******************************************************************************/

  CellDimension_t
  get_cell_length (Dimensions *dimensions, ColumnIndex_t column) ;

  CellDimension_t
  get_cell_width  (Dimensions *dimensions) ;

/******************************************************************************/

  GridDimension_t get_number_of_layers     (Dimensions *dimensions) ;

  GridDimension_t get_number_of_rows       (Dimensions *dimensions) ;

  GridDimension_t get_number_of_columns    (Dimensions *dimensions) ;

  GridDimension_t get_number_of_cells      (Dimensions *dimensions) ;

  GridDimension_t get_number_of_non_zeroes (Dimensions *dimensions) ;

/******************************************************************************/

  CellDimension_t
  get_cell_top_surface    (Dimensions *dimensions, int column) ;

  CellDimension_t
  get_cell_bottom_surface (Dimensions *dimensions, int column) ;

  CellDimension_t
  get_cell_east_surface   (Dimensions *dimensions, double height) ;

  CellDimension_t
  get_cell_west_surface   (Dimensions *dimensions, double height) ;

  CellDimension_t
  get_cell_north_surface  (Dimensions *dimensions, double height, int column) ;

  CellDimension_t
  get_cell_south_surface  (Dimensions *dimensions, double height, int column) ;

/******************************************************************************/

  int
  get_layer_area           (Dimensions* dimensions) ;

  int
  get_cell_offset_in_layer (Dimensions* dimensions, int row, int column) ;

  int
  get_cell_offset_in_stack (
                            Dimensions* dimensions,
                            int layer,
                            int row,
                            int column
                           ) ;

/******************************************************************************/

  ChipDimension_t
  get_chip_length          (Dimensions* dimensions) ;

  ChipDimension_t
  get_chip_width           (Dimensions* dimensions) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_DIMENSIONS_H_ */
