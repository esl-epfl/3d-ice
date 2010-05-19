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

#ifdef __cplusplus
extern "C"
{
#endif

#include "stack_description.h"

/******************************************************************************
 *                                                                            *
 * "SystemVector" : the representation of the A matrix.                       *
 *                                                                            *
 ******************************************************************************/

  typedef enum
  {
    TL_CCS_MATRIX = 0, TL_CRS_MATRIX

  } MatrixStorage_t ;

  typedef struct
  {
    int    *Columns ;
    int    *Rows ;
    double *Values ;

    int Size ;
    int NNz ;

    int columns_size ;
    int rows_size ;

    MatrixStorage_t Storage ;

  } SystemMatrix ;

/******************************************************************************/

  int alloc_system_matrix (
                           SystemMatrix *matrix,
                           MatrixStorage_t storage,
                           int nvalues,
                           int nnz
                          ) ;

  void fill_system_matrix (StackDescription *stkd,
                           SystemMatrix     *matrix,
                           Conductances     *conductances,
                           double           *capacities) ;

  void free_system_matrix (SystemMatrix *matrix) ;

  void print_system_matrix_columns (SystemMatrix *matrix, char *file_name) ;
  void print_system_matrix_rows    (SystemMatrix *matrix, char *file_name) ;
  void print_system_matrix_values  (SystemMatrix *matrix, char *file_name) ;

  int add_ccs_solid_column
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    Dimensions *dim,
    Conductances *conductances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *columns,
    int *rows,
    double *values
  ) ;

  int add_ccs_liquid_column
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    Dimensions *dim,
    Conductances *conductances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *columns,
    int *rows,
    double *values
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_SYSTEM_MATRIX_ */
