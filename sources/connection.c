#include <stdlib.h> // For the memory functions malloc/free

#include "connection.h"
#include "macros.h"

void connection_init (Connection_t *connection)
{
    connection->node1=0;
    connection->node2=0;
}

void connection_destroy (Connection_t *connection)
{
  connection_init (connection) ;
}

void connection_free (Connection_t *connection)
{
    if (connection == NULL)

        return ;

    connection_destroy (connection) ;

    free (connection) ;
}

void connection_copy (Connection_t *dst, Connection_t *src)
{
  connection_destroy (dst) ;

  dst->node1 = src->node1;
  dst->node2 = src->node2;
}

bool connection_equal (Connection_t *connection, Connection_t *other)
{
  return  connection->node1 == other->node1 &&
         connection->node2 == other->node2 ;
}

void connection_print (Connection_t *connection, FILE *stream, String_t prefix)
{
  fprintf(stream, "%sNode1: %d, Node2: %d\n", prefix, connection->node1, connection->node2) ;
}
