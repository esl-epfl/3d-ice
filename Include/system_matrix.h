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

  typedef struct
  {
    int    *Columns ;
    int    *Rows ;
    double *Values ;

    int Size ;
    int NNz ;

  } SystemMatrix ;

/******************************************************************************/

  int alloc_system_matrix (SystemMatrix *matrix, int nvalues, int nnz) ;

  void fill_system_matrix (StackDescription *stkd,
                           SystemMatrix *matrix,
                           Resistances *resistances,
                           double *capacities) ;

  void free_system_matrix (SystemMatrix *matrix) ;

  int add_solid_column
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    Dimensions *dim,
    Resistances *resistances,
    double *capacities,
    int current_layer,
    int current_row,
    int current_column,
    int *columns,
    int *rows,
    double *values
  ) ;

  int add_liquid_column
  (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
    FILE *debug,
#endif
    Dimensions *dim,
    Resistances *resistances,
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
