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
#include <stdio.h>

#include "messages_queue.h"

void init_messagess_queue (MessagesQueue* queue)
{
  queue->Head   = NULL ;
  queue->Tail   = NULL ;
  queue->Length = QUANTITY_I ;
}

MessagesQueue* alloc_and_init_messages_queue (void)
{
  MessagesQueue* queue = malloc (sizeof(MessagesQueue)) ;

  if (queue != NULL)  init_messagess_queue (queue) ;

  return queue ;
}

Bool_t is_empty_messages_queue (MessagesQueue* queue)
{
  return (queue->Length == 0) ;
}

void free_messages_queue (MessagesQueue* queue)
{
  while (! is_empty_messages_queue(queue) )

    pop_from_messages_queue(queue) ;

  FREE_POINTER (free, queue) ;
}

void put_into_messages_queue (MessagesQueue* queue, NetworkMessage messages)
{
  MessageNode* tmp = queue->Tail ;

  queue->Tail = malloc (sizeof(MessageNode)) ;

  if ( queue->Tail == NULL )
  {
    fprintf (stderr, "malloc messages node error !!\n") ;
    return ;
  }

  queue->Tail->Message = messages ;
  queue->Tail->Next  = NULL ;

  if (queue->Head == NULL)

    queue->Head = queue->Tail ;

  else

    tmp->Next = queue->Tail ;

  queue->Length++;
}

void pop_from_messages_queue (MessagesQueue* queue)
{
  MessageNode* tmp = queue->Head->Next ;

  FREE_POINTER (free, queue->Head) ;

  queue->Head = tmp ;

  queue->Length--;
}


