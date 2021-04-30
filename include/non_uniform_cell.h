#ifndef _NON_UNIFORM_CELL_H_
#define _NON_UNIFORM_CELL_H_

/*! \file non_uniform_cell.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
#include <stdio.h> // For the file type FILE
#include "types.h"
#include "string_t.h"

/******************************************************************************/
  
struct Non_uniform_cell_t
{
    CellIndex_t layer_info;
    ChipDimension_t left_x;
    ChipDimension_t left_y;
    ChipDimension_t length;
    ChipDimension_t width;
};

typedef struct Non_uniform_cell_t Non_uniform_cell_t;

/******************************************************************************/
  /** WARNING: We need to define these functions. If we want to change the name,
      we can do it in person_list.c
      The mandatory functions are
      TTT_data_init
      TTT_data_destroy
      TTT_data_copy
      TTT_data_equal
      TTT_data_print
  **/
      
  void non_uniform_cell_init (Non_uniform_cell_t *non_uniform_cell) ;
  void non_uniform_cell_destroy (Non_uniform_cell_t *non_uniform_cell) ;
  void non_uniform_cell_free (Non_uniform_cell_t *non_uniform_cell);
  void non_uniform_cell_copy (Non_uniform_cell_t *dst, Non_uniform_cell_t *src) ;  
  bool non_uniform_cell_equal (Non_uniform_cell_t *non_uniform_cell, Non_uniform_cell_t *other) ;
  void non_uniform_cell_print (Non_uniform_cell_t *non_uniform_cell, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _NON_UNIFORM_CELL_H_ */
