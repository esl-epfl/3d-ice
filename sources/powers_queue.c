/******************************************************************************
 *                                                                            *
 * Source file "sources/powers_queue.c"                                       *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "powers_queue.h"

/******************************************************************************/

void init_powers_queue (PowersQueue* queue)
{
  queue->Head = NULL ;
  queue->Tail = NULL ;
  queue->Length = 0 ;
}

/******************************************************************************/

PowersQueue* alloc_and_init_powers_queue (void)
{
  PowersQueue* queue = (PowersQueue*) malloc (sizeof(PowersQueue)) ;

  if (queue != NULL)  init_powers_queue (queue) ;

  return queue ;
}

/******************************************************************************/

int is_empty_powers_queue (PowersQueue* queue)
{
  return (queue->Length == 0) ;
}

/******************************************************************************/

void free_powers_queue (PowersQueue* queue)
{
  while (! is_empty_powers_queue(queue) )

    pop_from_powers_queue(queue) ;

  free (queue) ;
}

/******************************************************************************/

void put_into_powers_queue (PowersQueue* queue, Power_t power)
{
  struct PowerNode* tmp = queue->Tail ;

  queue->Tail = (struct PowerNode*) malloc ( sizeof(struct PowerNode) ) ;

  if ( queue->Tail == NULL )
  {
    fprintf (stderr, "malloc power node error !!\n") ;
    return ;
  }

  queue->Tail->Value = power ;
  queue->Tail->Next  = NULL ;

  if (queue->Head == NULL)

    queue->Head = queue->Tail ;

  else

    tmp->Next = queue->Tail ;

  queue->Length++;
}

/******************************************************************************/

Power_t get_from_powers_queue (PowersQueue* queue)
{
  return queue->Head != NULL ? queue->Head->Value : (Power_t) 0 ; // FIXME
}

/******************************************************************************/

void pop_from_powers_queue (PowersQueue* queue)
{
  struct PowerNode* tmp = queue->Head->Next ;

  free(queue->Head) ;

  queue->Head = tmp ;

  queue->Length--;
}

/******************************************************************************/

void print_powers_queue (FILE* stream, String_t prefix, PowersQueue* queue)
{
  struct PowerNode* tmp;
  fprintf(stream, "%s [%d] ", prefix, queue->Length);
  for (tmp = queue->Head ; tmp != NULL ; tmp = tmp->Next)
    fprintf(stream, "%6.4f ", tmp->Value) ;
  fprintf(stream, "\n");
}

/******************************************************************************/

void print_formatted_powers_queue (FILE* stream, PowersQueue* queue)
{
  struct PowerNode* tmp;
  for (tmp = queue->Head ; tmp != NULL ; tmp = tmp->Next)
    fprintf(stream, "%6.4f ", tmp->Value) ;
}

