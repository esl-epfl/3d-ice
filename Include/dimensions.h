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
 * "Dimensions" : all the dimensions used in the library (cell, grid, chip)   *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    struct
    {
      double FirstLength ;     /* The length of the cell if it belongs to the */
                               /* first column in a layer (column 0).         */

      double LastLength ;      /* The length of the cell if it belongs to the */
                               /* last column in a layer (column #columns-1). */

      double Length  ;         /* The length of the cell if it belongs to the */
                               /* remaining columns.                          */

      double Width ;           /* The width of every cell.                    */

    } Cell ;

    struct
    {
      int NLayers ;
      int NRows ;
      int NColumns ;
      int NCells ;
      int NNz ;

    } Grid ;

    struct
    {
      double Length ;
      double Width ;

    } Chip ;

  } Dimensions ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_dimensions           (Dimensions *dimensions) ;

  Dimensions*
  alloc_and_init_dimensions (void) ;

  void
  free_dimensions           (Dimensions *dimensions) ;

  void
  print_dimensions          (
                             FILE *stream,
                             char *prefix,
                             Dimensions *dimensions
                            ) ;

/******************************************************************************/

  double
  get_cell_length (Dimensions *dimensions, int column) ;

  double
  get_cell_width  (Dimensions *dimensions) ;

/******************************************************************************/

  int
  get_number_of_layers     (Dimensions *dimensions) ;

  int
  get_number_of_rows       (Dimensions *dimensions) ;

  int
  get_number_of_columns    (Dimensions *dimensions) ;

  int
  get_number_of_cells      (Dimensions *dimensions) ;

  int
  get_number_of_non_zeroes (Dimensions *dimensions) ;

/******************************************************************************/

  int
  get_cell_top_surface    (Dimensions *dimensions, int column) ;

  int
  get_cell_bottom_surface (Dimensions *dimensions, int column) ;

  int
  get_cell_east_surface   (Dimensions *dimensions, double height) ;

  int
  get_cell_west_surface   (Dimensions *dimensions, double height) ;

  int
  get_cell_north_surface  (Dimensions *dimensions, double height, int column) ;

  int
  get_cell_south_surface  (Dimensions *dimensions, double height, int column) ;

/******************************************************************************/

  int
  get_layer_area           (Dimensions *dimensions) ;

  int
  get_cell_offset_in_layer (Dimensions *dimensions, int row, int column) ;

  int
  get_cell_offset_in_stack (
                            Dimensions *dimensions,
                            int layer,
                            int row,
                            int column
                           ) ;

/******************************************************************************/

  double
  get_chip_length          (Dimensions *dimensions) ;

  double
  get_chip_width           (Dimensions *dimensions) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_DIMENSIONS_H_ */
