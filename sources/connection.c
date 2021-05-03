#include <stdlib.h> // For the memory functions malloc/free

#include "connection.h"
#include "macros.h"

void connection_init (Connection_t *connection)
{
    connection->node1=0;
    connection->node2=0;
    connection->node1_layer=0;
    connection->node2_layer=0;
    connection->direction = 0;
    connection->value=0;
}

void connection_destroy (Connection_t *connection)
{
    if (connection == NULL)

        return ;

    free (connection) ;
}

void connection_copy (Connection_t *dst, Connection_t *src)
{
  connection_init (dst) ;

  dst->node1 = src->node1;
  dst->node2 = src->node2;
  dst->node1_layer = src->node1_layer;
  dst->node2_layer = src->node2_layer;
  dst->direction = src->direction;
  dst->value = src->value;
}

bool connection_equal (Connection_t *connection, Connection_t *other)
{
  return  connection->node1 == other->node1 &&
         connection->node2 == other->node2 ;
}

void connection_print (Connection_t *connection, FILE *stream, String_t prefix)
{
  fprintf(stream, "%sNode%d in layer %d <-> Node%d in layer %d\n", prefix, connection->node1, connection->node1_layer, connection->node2, connection->node2_layer) ;
  fprintf(stream, "%sDirection %d, Value %f\n", prefix, connection->direction, connection->value) ;
}
