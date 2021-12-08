/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#include <stdlib.h> // For the memory function calloc
#include <string.h> // For the memory function memcpy

#include "network_message.h"

/******************************************************************************/

void network_message_init (NetworkMessage_t *message)
{
    message->Memory    = (MessageWord_t *) calloc (MESSAGE_LENGTH, sizeof (MessageWord_t)) ;

    message->MaxLength = MESSAGE_LENGTH ;

    message->Length    = message->Memory ;

    message->MType     = message->Length + 1u ;

    message->Content   = message->MType  + 1u ;
}

/******************************************************************************/

void network_message_destroy (NetworkMessage_t *message)
{
    if (message->Memory != NULL)

        free (message->Memory) ;

    message->Memory    = NULL ;
    message->MaxLength = 0 ;
    message->Length    = NULL ;
    message->MType     = NULL ;
    message->Content   = NULL ;
}

/******************************************************************************/

void increase_message_memory (NetworkMessage_t *message, Quantity_t new_size)
{
    MessageWord_t *tmp = (MessageWord_t *) calloc (new_size, sizeof(MessageWord_t)) ;

    memcpy (tmp, message->Memory, message->MaxLength * sizeof(MessageWord_t)) ;

    free (message->Memory) ;

    message->Memory    = tmp ;
    message->MaxLength = new_size ;
    message->Length    = message->Memory ;
    message->MType     = message->Length + 1u ;
    message->Content   = message->MType  + 1u ;
}

/******************************************************************************/

void build_message_head (NetworkMessage_t *message, MessageType_t type)
{
    *message->Length = (MessageWord_t) 2u ;

    *message->MType  = (MessageWord_t) type ;
}

/******************************************************************************/

void insert_message_word
(
    NetworkMessage_t *message,
    void             *word
)
{
    if (*message->Length == message->MaxLength)

        increase_message_memory (message, 2 * message->MaxLength) ;

    MessageWord_t *toinsert = message->Memory + *message->Length ;

    memcpy (toinsert, word, sizeof (MessageWord_t)) ;

    (*message->Length)++ ;
}

/******************************************************************************/

Error_t extract_message_word
(
    NetworkMessage_t *message,
    void             *word,
    Quantity_t        index
)
{
    if (index >= (message->MaxLength - 2))

        return TDICE_FAILURE ;

    memcpy (word, message->Content + index, sizeof (MessageWord_t)) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

