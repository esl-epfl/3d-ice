#include "non_uniform_cell_list.h"

#define ListType Non_uniform_cell
#define ListName non_uniform_cell

#define node_data_init      non_uniform_cell_init
#define node_data_destroy   non_uniform_cell_destroy
#define node_data_copy      non_uniform_cell_copy
#define node_data_equal     non_uniform_cell_equal
#define node_data_print     non_uniform_cell_print

#include "list_template.c"

#undef ListType
#undef ListName

/******************************************************************************/
