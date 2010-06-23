/******************************************************************************
 *                                                                            *
 * Header file "Include/add_rows_and_columns.h"                               *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_ADD_ROWS_AND_COLUMNS_
#define _TL_ADD_ROWS_AND_COLUMNS_

#include "stack_description.h"


  int add_ccs_solid_column
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    struct Dimensions *dim,
    struct Conductances *conductances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *columns,
    int *rows,
    double *values
  ) ;

  int add_crs_solid_row
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    struct Dimensions *dim,
    struct Conductances *conductances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *rows,
    int *columns,
    double *values
  ) ;

  int add_ccs_liquid_column
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    struct Dimensions *dim,
    struct Conductances *conductances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *columns,
    int *rows,
    double *values
  ) ;

  int add_crs_liquid_row
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    struct Dimensions *dim,
    struct Conductances *conductances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *rows,
    int *columns,
    double *values
  ) ;

/******************************************************************************/

#endif /* _TL_ADD_ROWS_AND_COLUMNS_ */
