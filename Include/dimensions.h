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

#include <stdio.h>

/******************************************************************************
 *                                                                            *
 * "Dimensions" : all the dimensions used in the library (cell, grid, chip)   *
 *                                                                            *
 ******************************************************************************/

  struct Dimensions
  {
    struct Cell
    {
      double FirstLength ,     /* The length of the cell if it belongs to the */
                               /* first column in a layer (column 0).         */
             LastLength ,      /* The length of the cell if it belongs to the */
                               /* last column in a layer (column #columns-1). */
             Length  ,         /* The length of the cell if it belongs to the */
                               /* remaining columns.                          */
             Width ;           /* The width of every cell.                    */

    } Cell ;

    struct Grid
    {
      int NLayers ,
          NRows ,
          NColumns ,
          NCells ,
          NNz ;

    } Grid ;

    struct Chip
    {
      double Length ,
             Width ;

    } Chip ;

  } ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_dimensions           (struct Dimensions *dimensions) ;

  struct Dimensions*
  alloc_and_init_dimensions (void) ;

  void
  free_dimensions           (struct Dimensions *dimensions) ;

  void
  print_dimensions          (
                             FILE *stream,
                             char *prefix,
                             struct Dimensions *dimensions
                            ) ;

/******************************************************************************/

  double
  get_cell_length (struct Dimensions *dimensions, int column) ;

  double
  get_cell_width  (struct Dimensions *dimensions) ;

/******************************************************************************/

  int
  get_number_of_layers     (struct Dimensions *dimensions) ;

  int
  get_number_of_rows       (struct Dimensions *dimensions) ;

  int
  get_number_of_columns    (struct Dimensions *dimensions) ;

  int
  get_number_of_cells      (struct Dimensions *dimensions) ;

  int
  get_number_of_non_zeroes (struct Dimensions *dimensions) ;

/******************************************************************************/

  double
  get_cell_top_surface    (struct Dimensions *dimensions, int column) ;

  double
  get_cell_bottom_surface (struct Dimensions *dimensions, int column) ;

  double
  get_cell_east_surface   (struct Dimensions *dimensions, double height) ;

  double
  get_cell_west_surface   (struct Dimensions *dimensions, double height) ;

  double
  get_cell_north_surface  (struct Dimensions *dimensions, double height, int column) ;

  double
  get_cell_south_surface  (struct Dimensions *dimensions, double height, int column) ;

/******************************************************************************/

  int
  get_layer_area           (struct Dimensions *dimensions) ;

  int
  get_cell_offset_in_layer (struct Dimensions *dimensions, int row, int column) ;

  int
  get_cell_offset_in_stack (
                            struct Dimensions *dimensions,
                            int layer,
                            int row,
                            int column
                           ) ;

/******************************************************************************/

  double
  get_chip_length          (struct Dimensions *dimensions) ;

  double
  get_chip_width           (struct Dimensions *dimensions) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#endif /* _TL_DIMENSIONS_H_ */
