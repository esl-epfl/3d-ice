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

#include <stdio.h>

/******************************************************************************
 *                                                                            *
 * "CellDimensions" : the dimensions of a individual cell.                    *
 *                                                                            *
 * To describe the dimension of a single cell we store only its length(s)     *
 * and its width. We don't store the height since it coincides with the       *
 * height of the layer it belongs to. Indeed, every layer composing a         *
 * 3DChip is divided vertically into a single cell.                           *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double FirstLength ;  /* The length of the cell if it belongs to the */
                          /* first column in a layer (column 0).         */

    double LastLength ;   /* The length of the cell if it belongs to the */
                          /* last column in a layer (column #columns-1). */

    double Length  ;      /* The length of the cell if it belongs to the */
                          /* remaining columns.                          */

    double Width ;        /* The width of every cell.                    */

  } CellDimensions ;

/******************************************************************************/

  void init_cells_dimensions  (CellDimensions *cd) ;
  void print_cells_dimensions (FILE *stream, char *prefix, CellDimensions *cd) ;

/******************************************************************************
 *                                                                            *
 * "GridDimensions" : the dimensions of a 3DGrid of cells.                    *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    int NLayers ;
    int NRows ;
    int NColumns ;
    int NCells ;
    int NNz ;

  } GridDimensions ;

/******************************************************************************/

  void init_grid_dimensions  (GridDimensions *gd) ;
  void print_grid_dimensions (FILE *stream, char *prefix, GridDimensions *gd) ;

/******************************************************************************
 *                                                                            *
 * "ChipDimensions" : the dimensions of a 3DChip.                             *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double Length ;
    double Width ;

  } ChipDimensions ;

/******************************************************************************/

  void init_chip_dimensions  (ChipDimensions *cd) ;
  void print_chip_dimensions (FILE *stream, char *prefix, ChipDimensions *gd) ;

/******************************************************************************
 *                                                                            *
 * "Dimensions" : all the dimensions used in the library.                     *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    CellDimensions Cell ;
    GridDimensions Grid ;
    ChipDimensions Chip ;

  } Dimensions ;

/******************************************************************************/

  void init_dimensions (Dimensions *dim) ;

  Dimensions* alloc_and_init_dimensions (void) ;

  void free_dimensions (Dimensions *dim) ;

  void  print_dimensions (FILE *stream, char *prefix, Dimensions *dim) ;

  double
  get_cell_width           (Dimensions *dimensions) ;

  double
  get_cell_length          (Dimensions *dimensions, int column) ;

  int
  get_number_of_layers     (Dimensions *dimensions) ;

  int
  get_number_of_rows       (Dimensions *dimensions) ;

  int
  get_number_of_columns    (Dimensions *dimensions) ;

  int
  get_cell_top_surface     (Dimensions *dimensions, int column) ;

  int
  get_cell_offset_in_layer (Dimensions *dimensions, int row, int column) ;

  int
  get_layer_area           (Dimensions *dimensions) ;

  int
  get_cell_offset_in_stack (
                            Dimensions *dimensions,
                            int layer,
                            int row,
                            int column
                           ) ;
  double
  get_chip_length          (Dimensions *dimensions) ;

  double
  get_chip_width           (Dimensions *dimensions) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_DIMENSIONS_H_ */
