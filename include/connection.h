#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/*! \file connection.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
#include <stdio.h> // For the file type FILE
#include "types.h"
#include "string_t.h"

/******************************************************************************/
  
struct Connection_t
{
    int node1;
    int node2;
};

typedef struct Connection_t Connection_t;

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
      
  void connection_init (Connection_t *connection) ;
  void connection_destroy (Connection_t *connection) ;
  void connection_free (Connection_t *connection);
  void connection_copy (Connection_t *dst, Connection_t *src) ;  
  bool connection_equal (Connection_t *connection, Connection_t *other) ;
  void connection_print (Connection_t *connection, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _CONNECTION_H_ */
