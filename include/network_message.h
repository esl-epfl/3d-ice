/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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

#ifndef _3DICE_NETWORK_MESSAGE_H_
#define _3DICE_NETWORK_MESSAGE_H_

/*! \file network_message.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"

    /*! \def MAX_LENGTH
     *
     *  The maximum number of words that a message can store (header included)
     */

#   define MAX_LENGTH 256

/******************************************************************************/

    /*! \struct NetworkMessage
     *
     *  \brief Structure used to store messages to be sent over network
     *
     *  The Maximum Length of the message, as well as the content of the
     *  Length field in te message is mesured in number of words.
     */

    struct NetworkMessage   
    {
        /*! The memory used to store the message */

        MessageWord_t Memory [MAX_LENGTH] ;

        /*! The maximum length of the message */

        Quantity_t MaxLength ;

        /*! Offset to access the actual length of the message */

        MessageWord_t *Length ;

        /*! Offset to access the type of the message */

        MessageWord_t *Type ;

        /*! Offset to access the content of the message */

        MessageWord_t *Content ;

    } ;

    /*! Definition of the type NetworkMessage */

    typedef struct NetworkMessage NetworkMessage ;


    /*! Initializes a NetworkMessage
     *
     * \param message   the address of the NetworkMessage to initialize
     */

    void init_network_message (NetworkMessage *message) ;



    /*! Builds the head of a message (sets its type)
     *
     *  The function sets the content of the first two words of the message
     *  (i.e. the length snd the type)
     *
     * \param message the address of the message to build
     * \param type    the type of the request
     */

    void build_message_head (NetworkMessage *message, MessageType_t type) ;



    /*! Inserts a single word to the content of a message
     *
     * The value pointed by word will be insert at the end of the message
     * (as a suffix) and the length of the message is incresed conseguently
     *
     * \param message the address of the message to build
     * \param word    (in) the address of the word to add
     *
     * \return \c TDICE_SUCCESS if the operation succeeded
     * \return \c TDICE_FAILURE if there is not enough memory left in the message
     */

    Error_t insert_message_word (NetworkMessage *message, void *word) ;



    /*! Extracts the index-th word from the content of a message
     *
     * The function will not change the status/content of the message
     *
     * \param message the address of the message to build
     * \param word    (out) the address of the word to extrcact
     * \param index   the index of the word (starting from the beginning of the content)
     *
     * \return \c TDICE_SUCCESS if the operation succeeded
     * \return \c TDICE_FAILURE if there index specifies a word out of the message
     */

    Error_t extract_message_word

        (NetworkMessage *message, void *word, Quantity_t index) ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_NETWORK_MESSAGE_H_ */
