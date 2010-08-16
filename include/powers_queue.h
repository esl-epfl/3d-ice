/******************************************************************************
 *                                                                            *
 * Header file "include/powers_queue.h"                                        *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_POWERSQUEUE_H_
#define _TL_POWERSQUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"

#define RESIZE_FACTOR 5

/******************************************************************************
 *                               "PowersQueue"                                *
 ******************************************************************************/

  struct PowerNode
  {
    Power_t Value ;
    struct PowerNode* Next ;
  } ;

  struct PowersQueue
  {
    struct PowerNode* Head ;
    struct PowerNode* Tail ;
    int Length ;
  } ;

  typedef struct PowersQueue PowersQueue ;

/******************************************************************************/

  void init_powers_queue (PowersQueue* queue) ;

  PowersQueue* alloc_and_init_powers_queue (void) ;

  int is_empty_powers_queue (PowersQueue* queue) ;

  void free_powers_queue (PowersQueue* queue) ;

  void put_into_powers_queue (PowersQueue* queue, Power_t power) ;

  Power_t get_from_powers_queue (PowersQueue* queue) ;

  void pop_from_powers_queue (PowersQueue* queue) ;

  void print_powers_queue (FILE* stream, String_t prefix, PowersQueue* queue) ;

  void print_formatted_powers_queue (FILE* stream, PowersQueue* queue) ;

#ifdef __cplusplus
}
#endif

#endif /* _TL_POWERSQUEUE_H_ */
