/******************************************************************************
 *                                                                            *
 * Header file "Include/system_matrix.h"                                      *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_SYSTEM_MATRIX_
#define _TL_SYSTEM_MATRIX_

#include "stack_description.h"

/******************************************************************************
 *                                                                            *
 * "SystemVector" : the representation of the A matrix.                       *
 *                                                                            *
 ******************************************************************************/

  enum MatrixStorage_t
  {
    TL_CCS_MATRIX = 0, TL_CRS_MATRIX

  } ;

  struct SystemMatrix
  {
    int    *Columns ;
    int    *Rows ;
    double *Values ;

    int Size ;
    int NNz ;

    int columns_size ;
    int rows_size ;

    enum MatrixStorage_t Storage ;

  } ;

/******************************************************************************/

  int alloc_system_matrix (
                           struct SystemMatrix *matrix,
                           enum MatrixStorage_t storage,
                           int nvalues,
                           int nnz
                          ) ;

  void fill_system_matrix (struct StackDescription *stkd,
                           struct SystemMatrix     *matrix,
                           struct Conductances     *conductances,
                           double           *capacities) ;

  void free_system_matrix (struct SystemMatrix *matrix) ;

  void print_system_matrix_columns (struct SystemMatrix *matrix, char const *file_name) ;
  void print_system_matrix_rows    (struct SystemMatrix *matrix, char const *file_name) ;
  void print_system_matrix_values  (struct SystemMatrix *matrix, char const *file_name) ;

  void print_system_matrix (struct SystemMatrix *matrix, char *file_name) ;

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

  int add_crs_solid_column
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

  int add_crs_liquid_column
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

#endif /* _TL_SYSTEM_MATRIX_ */
