/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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

void powers_queue_init (PowersQueue_t *pqueue)
{
    pqueue->Capacity = (Quantity_t) 0u ;
    pqueue->Memory   = NULL ;
    pqueue->Size     = (Quantity_t) 0u ;
    pqueue->End      = (Quantity_t) 0u ;
    pqueue->Start    = (Quantity_t) 0u ;
}

/******************************************************************************/

void powers_queue_build (PowersQueue_t *pqueue, Quantity_t capacity)
{
    if (pqueue == NULL)    return ;

    powers_queue_destroy (pqueue) ;

    pqueue->Memory = (Power_t *) calloc (capacity, sizeof (Power_t)) ;

    if (pqueue->Memory == NULL)
    {
        fprintf (stderr, "Malloc power queue error\n") ;

        return ;
    }

    pqueue->Capacity = capacity ;
}

/******************************************************************************/

void powers_queue_destroy (PowersQueue_t *pqueue)
{
    if (pqueue->Memory != NULL)

        free (pqueue->Memory) ;

    powers_queue_init (pqueue) ;
}

/******************************************************************************/

void powers_queue_copy (PowersQueue_t *dst, PowersQueue_t *src)
{
    if (dst->Capacity < src->Capacity || dst->Memory == NULL)
    {
        powers_queue_build (dst, src->Capacity) ;
    }
    else
    {
        dst->Size  = (Quantity_t) 0u ;
        dst->Start = (Quantity_t) 0u ;
        dst->End   = (Quantity_t) 0u ;
    }


    Quantity_t index = src->Start ;

    if (index == src->End)
    {
        put_into_powers_queue (dst, src->Memory [index]) ;

        index = (index + 1) % src->Capacity ;
    }

    while (index != src->End)
    {
        put_into_powers_queue (dst, src->Memory [index]) ;

        index = (index + 1) % src->Capacity ;
    }
}

/******************************************************************************/

PowersQueue_t *powers_queue_calloc  ( void )
{
    PowersQueue_t *pqueue = (PowersQueue_t *) malloc (sizeof(PowersQueue_t)) ;

    if (pqueue != NULL)

        powers_queue_init (pqueue) ;

    return pqueue ;
}

/******************************************************************************/

PowersQueue_t *powers_queue_clone (PowersQueue_t *pqueue)
{
    if (pqueue == NULL)

        return NULL ;

    PowersQueue_t *newpq = powers_queue_calloc  ( ) ;

    if (newpq != NULL)

        powers_queue_copy (newpq, pqueue) ;

    return newpq ;
}

/******************************************************************************/

void powers_queue_free (PowersQueue_t *pqueue)
{
    if (pqueue == NULL)

        return ;

    powers_queue_destroy (pqueue) ;

    free (pqueue) ;
}

/******************************************************************************/

void powers_queue_print
(
    PowersQueue_t *pqueue,
    FILE          *stream,
    String_t       prefix
)
{
    fprintf (stream, "%s", prefix) ;

    Quantity_t index = pqueue->Start ;

    if (index == pqueue->End)
    {
        fprintf (stream, "%.3f ", pqueue->Memory [index]) ;

        index = (index + 1) % pqueue->Capacity ;
    }

    while (index != pqueue->End)
    {
        fprintf (stream, "%.3f ", pqueue->Memory [index]) ;

        index = (index + 1) % pqueue->Capacity ;
    }
}

/******************************************************************************/

bool is_empty_powers_queue (PowersQueue_t *pqueue)
{
    return (pqueue->Size == 0) ;
}

/******************************************************************************/

bool is_full_powers_queue (PowersQueue_t *pqueue)
{
    return (pqueue->Size == pqueue->Capacity) ;
}

/******************************************************************************/

void put_into_powers_queue (PowersQueue_t *pqueue, Power_t power)
{
    if (pqueue->Memory == NULL)
    {
        fprintf (stderr, "ERROR: put into not-built powers queue\n") ;

        return ;
    }

    if (is_full_powers_queue (pqueue))
    {
        PowersQueue_t pq ;

        powers_queue_init (&pq) ;

        powers_queue_build (&pq, pqueue->Capacity * (Quantity_t) 2) ;

        powers_queue_copy (&pq, pqueue) ;

        Power_t *tmp = pq.Memory ;
           pq.Memory = pqueue->Memory ;
        pqueue->Memory = tmp ;

        pqueue->Capacity = pq.Capacity ;
        pqueue->Size     = pq.Size ;
        pqueue->Start    = pq.Start ;
        pqueue->End      = pq.End ;

        powers_queue_destroy (&pq) ;
    }

    pqueue->Memory [pqueue->End] = power ;

    pqueue->End = (pqueue->End + 1) % pqueue->Capacity ;

    pqueue->Size++ ;
}

/******************************************************************************/

Power_t get_from_powers_queue (PowersQueue_t *pqueue)
{
    if (pqueue->Memory == NULL)
    {
        fprintf (stderr, "ERROR: get from not-built powers queue\n") ;

        return 0.0 ;
    }

    if ( is_empty_powers_queue(pqueue) )
    {
        fprintf (stderr, "ERROR: get from empty power queue\n") ;

        return 0.0 ;
    }

    Power_t power = pqueue->Memory [pqueue->Start] ;

    pqueue->Start = (pqueue->Start + 1) % pqueue->Capacity ;

    pqueue->Size-- ;

    return power ;
}

/******************************************************************************/
