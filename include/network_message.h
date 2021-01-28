/******************************************************************************
 * This file is part of 3D-ICE, version 3.0.0 .                               *
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

    /*! \def MESSAGE_LENGTH
     *
     *  The initial number of words that a message can store (header included)
     */

#   define MESSAGE_LENGTH 256

/******************************************************************************/

    /*! \struct NetworkMessage_t
     *
     *  \brief Structure used to store messages to be sent over network
     *
     *  The Maximum Length of the message, as well as the content of the
     *  Length field in te message is mesured in number of words.
     */

    struct NetworkMessage_t
    {
        /*! The memory used to store the message */

        MessageWord_t *Memory ;

        /*! The maximum length of the message */

        Quantity_t MaxLength ;

        /*! Offset to access the actual length of the message */

        MessageWord_t *Length ;

        /*! Offset to access the type of the message */

        MessageWord_t *MType ;

        /*! Offset to access the content of the message */

        MessageWord_t *Content ;

    } ;

    /*! Definition of the type NetworkMessage_t */

    typedef struct NetworkMessage_t NetworkMessage_t ;



/******************************************************************************/



    /*! Inits the fields of the \a message structure with default values
     *
     * The function reserves memory to store \c MESSAGE_LENGTH words
     *
     * \param message the address of the structure to initalize
     */

    void network_message_init (NetworkMessage_t *message) ;



    /*! Destroys the content of the fields of the structure \a message
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a network_message_init .
     *
     * \param message the address of the structure to destroy
     */

    void network_message_destroy (NetworkMessage_t *message) ;



    /*! Changes the amount of memory available to store the message
     *
     * \param message     the address of the message
     * \param new_size the new size (number of words)
     *
     */

    void increase_message_memory

        (NetworkMessage_t *message, Quantity_t new_size) ;



    /*! Builds the head of a message (sets its type)
     *
     *  The function sets the content of the first two words of the message
     *  (i.e. the length snd the type)
     *
     * \param message the address of the message to build
     * \param type the type of the request
     */

    void build_message_head (NetworkMessage_t *message, MessageType_t type) ;



    /*! Inserts a single word to the content of a message
     *
     * The value pointed by word will be insert at the end of the message
     * (as a suffix) and the length of the message is incresed conseguently
     *
     * \param message the address of the message to build
     * \param word (in) the address of the word to add
     */

    void insert_message_word (NetworkMessage_t *message, void *word) ;



    /*! Extracts the index-th word from the content of a message
     *
     * The function will not change the status/content of the message. If
     * \a index indicates a position out of the message payload the result is
     * undetermined.
     *
     * \param message  the address of the message to access
     * \param word  (out) the address of the word to extrcact
     * \param index the index of the word (starting from the beginning
     *              of the content)
     *
     * \return \c TDICE_SUCCESS if the operation succeeded
     * \return \c TDICE_FAILURE if there index specifies a word out
     *                          of the message
     */

    Error_t extract_message_word

        (NetworkMessage_t *message, void *word, Quantity_t index) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_NETWORK_MESSAGE_H_ */
