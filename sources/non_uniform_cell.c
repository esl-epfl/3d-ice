#include <stdlib.h> // For the memory functions malloc/free

#include "non_uniform_cell.h"
#include "macros.h"

void non_uniform_cell_init (Non_uniform_cell_t *non_uniform_cell)
{
    non_uniform_cell->layer_info=0;
    non_uniform_cell->left_x=0;
    non_uniform_cell->left_y=0;
    non_uniform_cell->length=0;
    non_uniform_cell->width=0;
    non_uniform_cell->isChannel=0;
}

void non_uniform_cell_destroy (Non_uniform_cell_t *non_uniform_cell)
{
  non_uniform_cell_init (non_uniform_cell) ;
}

void non_uniform_cell_free (Non_uniform_cell_t *non_uniform_cell)
{
    if (non_uniform_cell == NULL)

        return ;

    non_uniform_cell_destroy (non_uniform_cell) ;

    free (non_uniform_cell) ;
}

void non_uniform_cell_copy (Non_uniform_cell_t *dst, Non_uniform_cell_t *src)
{
  non_uniform_cell_init (dst) ;

  dst->layer_info = src->layer_info;
  dst->left_x = src->left_x;
  dst->left_y = src->left_y;
  dst->length = src->length;
  dst->width = src->width;
  dst->isChannel = src->isChannel;

}

bool non_uniform_cell_equal (Non_uniform_cell_t *non_uniform_cell, Non_uniform_cell_t *other)
{
  return  non_uniform_cell->layer_info == other->layer_info &&
         non_uniform_cell->left_x == other->left_x &&
         non_uniform_cell->left_y == other->left_y &&
         non_uniform_cell->length == other->length &&
         non_uniform_cell->width == other->width &&
         non_uniform_cell->isChannel == other->isChannel;
}

void non_uniform_cell_print (Non_uniform_cell_t *non_uniform_cell, FILE *stream, String_t prefix)
{
  fprintf(stream, "%sLayer info: %d\n", prefix, non_uniform_cell->layer_info) ;
}
