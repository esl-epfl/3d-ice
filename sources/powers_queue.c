/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdlib.h>

#include "powers_queue.h"
#include "macros.h"

/******************************************************************************/

void init_powers_queue (PowersQueue *powers_queue)
{
    powers_queue->Head   = NULL ;
    powers_queue->Tail   = NULL ;
    powers_queue->Length = 0u ;
}

/******************************************************************************/

PowersQueue *alloc_and_init_powers_queue (void)
{
    PowersQueue *powers_queue = (PowersQueue *) malloc (sizeof(PowersQueue)) ;

    if (powers_queue != NULL)

        init_powers_queue (powers_queue) ;

    return powers_queue ;
}

/******************************************************************************/

void free_powers_queue (PowersQueue *powers_queue)
{
    while (! is_empty_powers_queue(powers_queue))

        pop_from_powers_queue(powers_queue) ;

    FREE_POINTER (free, powers_queue) ;
}

/******************************************************************************/

void print_detailed_powers_queue
(
    FILE        *stream,
    String_t     prefix,
    PowersQueue *powers_queue
)
{
    fprintf(stream, "%s [%d] ", prefix, powers_queue->Length) ;

    PowerNode *tmp = NULL ;
    for (tmp = powers_queue->Head ; tmp != NULL ; tmp = tmp->Next)

        fprintf(stream, "%6.4f ", tmp->Value) ;

    fprintf(stream, "\n") ;
}

/******************************************************************************/

void print_formatted_powers_queue (FILE *stream, PowersQueue *powers_queue)
{
    PowerNode *tmp = NULL ;
    for (tmp = powers_queue->Head ; tmp != NULL ; tmp = tmp->Next)

        fprintf(stream, "%6.4f ", tmp->Value) ;
}

/******************************************************************************/

bool is_empty_powers_queue (PowersQueue *powers_queue)
{
    return (powers_queue->Length == 0) ;
}

/******************************************************************************/

void put_into_powers_queue (PowersQueue *powers_queue, Power_t power)
{
    PowerNode *tmp = powers_queue->Tail ;

    powers_queue->Tail = malloc (sizeof(PowerNode)) ;

    if ( powers_queue->Tail == NULL )
    {
        fprintf (stderr, "malloc power node error !!\n") ;
        return ;
    }

    powers_queue->Tail->Value = power ;
    powers_queue->Tail->Next  = NULL ;

    if (powers_queue->Head == NULL)

        powers_queue->Head = powers_queue->Tail ;

    else

        tmp->Next = powers_queue->Tail ;

    powers_queue->Length++;
}

/******************************************************************************/

Power_t get_from_powers_queue (PowersQueue *powers_queue)
{
    return powers_queue->Head->Value ;
}

/******************************************************************************/

void pop_from_powers_queue (PowersQueue *powers_queue)
{
    PowerNode *tmp = powers_queue->Head->Next ;

    FREE_POINTER (free, powers_queue->Head) ;

    powers_queue->Head = tmp ;

    powers_queue->Length--;
}

/******************************************************************************/
