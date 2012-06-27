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

void init_powers_queue (PowersQueue_t *this)
{
    this->Capacity = (Quantity_t) 0u ;
    this->Memory   = NULL ;
    this->Size     = (Quantity_t) 0u ;
    this->End      = (Quantity_t) 0u ;
    this->Start    = (Quantity_t) 0u ;
}

/******************************************************************************/

void build_powers_queue (PowersQueue_t *this, Quantity_t capacity)
{
    if (this == NULL)    return ;

    destroy_powers_queue (this) ;

    this->Memory = (Power_t *) calloc (capacity, sizeof (Power_t)) ;

    if (this->Memory == NULL)
    {
        fprintf (stderr, "Malloc power queue error\n") ;

        return ;
    }

    this->Capacity = capacity ;
}

/******************************************************************************/

void destroy_powers_queue (PowersQueue_t *this)
{
    if (this == NULL)    return ;

    FREE_POINTER (free, this->Memory) ;

    init_powers_queue (this) ;
}

/******************************************************************************/

void copy_powers_queue (PowersQueue_t *dst, PowersQueue_t *src)
{
    if (dst->Capacity < src->Capacity || dst->Memory == NULL)
    {
        build_powers_queue (dst, src->Capacity) ;
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

PowersQueue_t *calloc_powers_queue ( void )
{
    PowersQueue_t *powers_queue = (PowersQueue_t *) malloc (sizeof(PowersQueue_t)) ;

    if (powers_queue != NULL)

        init_powers_queue (powers_queue) ;

    return powers_queue ;
}

/******************************************************************************/

void free_powers_queue (PowersQueue_t *this)
{
    if (this == NULL)    return ;

    destroy_powers_queue (this) ;

    FREE_POINTER (free, this) ;
}

/******************************************************************************/

void print_powers_queue
(
    PowersQueue_t *this,
    FILE          *stream,
    String_t       prefix
)
{
    fprintf (stream, "%s", prefix) ;

    Quantity_t index = this->Start ;

    if (index == this->End)
    {
        fprintf (stream, "%.3f ", this->Memory [index]) ;

        index = (index + 1) % this->Capacity ;
    }

    while (index != this->End)
    {
        fprintf (stream, "%.3f ", this->Memory [index]) ;

        index = (index + 1) % this->Capacity ;
    }
}

/******************************************************************************/

bool is_empty_powers_queue (PowersQueue_t *this)
{
    return (this->Size == 0) ;
}

/******************************************************************************/

bool is_full_powers_queue (PowersQueue_t *this)
{
    return (this->Size == this->Capacity) ;
}

/******************************************************************************/

void put_into_powers_queue (PowersQueue_t *this, Power_t power)
{
    if (this->Memory == NULL)
    {
        fprintf (stderr, "ERROR: put into not-built powers queue\n") ;

        return ;
    }

    if (is_full_powers_queue (this))
    {
        PowersQueue_t pq ;

        init_powers_queue (&pq) ;

        build_powers_queue (&pq, this->Capacity * (Quantity_t) 2) ;

        copy_powers_queue (&pq, this) ;

        Power_t *tmp = pq.Memory ;
           pq.Memory = this->Memory ;
        this->Memory = tmp ;

        this->Capacity = pq.Capacity ;
        this->Size     = pq.Size ;
        this->Start    = pq.Start ;
        this->End      = pq.End ;

        destroy_powers_queue (&pq) ;
    }

    this->Memory [this->End] = power ;

    this->End = (this->End + 1) % this->Capacity ;

    this->Size++ ;
}

/******************************************************************************/

Power_t get_from_powers_queue (PowersQueue_t *this)
{
    if (this->Memory == NULL)
    {
        fprintf (stderr, "ERROR: get from not-built powers queue\n") ;

        return 0.0 ;
    }

    if ( is_empty_powers_queue(this) )
    {
        fprintf (stderr, "ERROR: get from empty power queue\n") ;

        return 0.0 ;
    }

    Power_t power = this->Memory [this->Start] ;

    this->Start = (this->Start + 1) % this->Capacity ;

    this->Size-- ;

    return power ;
}

/******************************************************************************/
