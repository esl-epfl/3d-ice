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
    this->Head   = NULL ;
    this->Tail   = NULL ;
    this->Length = 0u ;
}

/******************************************************************************/

PowersQueue_t *alloc_and_init_powers_queue (void)
{
    PowersQueue_t *powers_queue = (PowersQueue_t *) malloc (sizeof(PowersQueue_t)) ;

    if (powers_queue != NULL)

        init_powers_queue (powers_queue) ;

    return powers_queue ;
}

/******************************************************************************/

void free_powers_queue (PowersQueue_t *this)
{
    while (! is_empty_powers_queue(this))

        pop_from_powers_queue(this) ;

    FREE_POINTER (free, this) ;
}

/******************************************************************************/

void print_detailed_powers_queue
(
    PowersQueue_t *this,
    FILE          *stream,
    String_t       prefix
)
{
    fprintf(stream, "%s [%d] ", prefix, this->Length) ;

    PowerNode_t *tmp = NULL ;
    for (tmp = this->Head ; tmp != NULL ; tmp = tmp->Next)

        fprintf(stream, "%6.4f ", tmp->Value) ;

    fprintf(stream, "\n") ;
}

/******************************************************************************/

void print_formatted_powers_queue (PowersQueue_t *this, FILE *stream)
{
    PowerNode_t *tmp = NULL ;
    for (tmp = this->Head ; tmp != NULL ; tmp = tmp->Next)

        fprintf(stream, "%6.4f ", tmp->Value) ;
}

/******************************************************************************/

bool is_empty_powers_queue (PowersQueue_t *this)
{
    return (this->Length == 0) ;
}

/******************************************************************************/

void put_into_powers_queue (PowersQueue_t *this, Power_t power)
{
    PowerNode_t *tmp = this->Tail ;

    this->Tail = (PowerNode_t *) malloc (sizeof(PowerNode_t)) ;

    if ( this->Tail == NULL )
    {
        fprintf (stderr, "malloc power node error !!\n") ;
        return ;
    }

    this->Tail->Value = power ;
    this->Tail->Next  = NULL ;

    if (this->Head == NULL)

        this->Head = this->Tail ;

    else

        tmp->Next = this->Tail ;

    this->Length++;
}

/******************************************************************************/

Power_t get_from_powers_queue (PowersQueue_t *this)
{
    return this->Head->Value ;
}

/******************************************************************************/

void pop_from_powers_queue (PowersQueue_t *this)
{
    PowerNode_t *tmp = this->Head->Next ;

    FREE_POINTER (free, this->Head) ;

    this->Head = tmp ;

    this->Length--;
}

/******************************************************************************/
