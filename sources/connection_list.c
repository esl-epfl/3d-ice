#include "connection_list.h"

#define ListType Connection
#define ListName connection

#define node_data_init      connection_init
#define node_data_destroy   connection_destroy
#define node_data_copy      connection_copy
#define node_data_equal     connection_equal
#define node_data_print     connection_print

#include "list_template.c"

#undef ListType
#undef ListName

/******************************************************************************/
