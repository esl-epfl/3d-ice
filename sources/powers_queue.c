/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
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

Bool_t is_empty_powers_queue (PowersQueue* queue)
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
  PowerNode* tmp = queue->Tail ;

  queue->Tail = (PowerNode*) malloc ( sizeof(PowerNode) ) ;

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
  return queue->Head->Value ;
}

/******************************************************************************/

void pop_from_powers_queue (PowersQueue* queue)
{
  PowerNode* tmp = queue->Head->Next ;

  free(queue->Head) ;

  queue->Head = tmp ;

  queue->Length--;
}

/******************************************************************************/

void print_powers_queue (FILE* stream, String_t prefix, PowersQueue* queue)
{
  PowerNode* tmp;
  fprintf(stream, "%s [%d] ", prefix, queue->Length);
  for (tmp = queue->Head ; tmp != NULL ; tmp = tmp->Next)
    fprintf(stream, "%6.4f ", tmp->Value) ;
  fprintf(stream, "\n");
}

/******************************************************************************/

void print_formatted_powers_queue (FILE* stream, PowersQueue* queue)
{
  PowerNode* tmp;
  for (tmp = queue->Head ; tmp != NULL ; tmp = tmp->Next)
    fprintf(stream, "%6.4f ", tmp->Value) ;
}

/******************************************************************************/
