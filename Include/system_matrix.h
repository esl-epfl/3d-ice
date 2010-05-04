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

  void fill_system_matrix (SystemMatrix *matrix,
                           StackDescription *stkd,
                           double time_delta) ;

  void free_system_matrix (SystemMatrix *matrix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_SYSTEM_MATRIX_ */
