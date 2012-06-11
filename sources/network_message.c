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

#include <string.h>

#include "macros.h"
#include "network_message.h"

/******************************************************************************/

void init_network_message (NetworkMessage_t *this)
{
    this->Memory    = (MessageWord_t *) calloc (MESSAGE_LENGTH, sizeof (MessageWord_t)) ;

    this->MaxLength = MESSAGE_LENGTH ;

    this->Length    = this->Memory ;

    this->Type      = this->Length + 1u ;

    this->Content   = this->Type   + 1u ;
}

/******************************************************************************/

void free_network_message (NetworkMessage_t *this)
{
    FREE_POINTER (free, this->Memory) ;

    this->Memory    = NULL ;
    this->MaxLength = 0 ;
    this->Length    = NULL ;
    this->Type      = NULL ;
    this->Content   = NULL ;
}

/******************************************************************************/

void increase_message_memory (NetworkMessage_t *this, Quantity_t new_size)
{
    MessageWord_t *tmp = (MessageWord_t *) calloc (new_size, sizeof(MessageWord_t)) ;

    memcpy (tmp, this->Memory, this->MaxLength * sizeof(MessageWord_t)) ;

    free (this->Memory) ;

    this->Memory    = tmp ;
    this->MaxLength = new_size ;
    this->Length    = this->Memory ;
    this->Type      = this->Length + 1u ;
    this->Content   = this->Type   + 1u ;
}

/******************************************************************************/

void build_message_head (NetworkMessage_t *this, MessageType_t type)
{
    *this->Length = (MessageWord_t) 2u ;

    *this->Type   = (MessageWord_t) type ;
}

/******************************************************************************/

void insert_message_word
(
    NetworkMessage_t *this,
    void             *word
)
{
    if (*this->Length == this->MaxLength)

        increase_message_memory (this, 2 * this->MaxLength) ;

    MessageWord_t *toinsert = this->Memory + *this->Length ;

    memcpy (toinsert, word, sizeof (MessageWord_t)) ;

    (*this->Length)++ ;
}

/******************************************************************************/

Error_t extract_message_word
(
    NetworkMessage_t *this,
    void             *word,
    Quantity_t        index
)
{
    if (index >= (this->MaxLength - 2))

        return TDICE_FAILURE ;

    memcpy (word, this->Content + index, sizeof (MessageWord_t)) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

