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

#ifndef _3DICE_STACK_ELEMENT_H_
#define _3DICE_STACK_ELEMENT_H_

/*! \file stack_element.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "channel.h"
#include "heat_sink.h"
#include "die.h"
#include "dimensions.h"
#include "floorplan.h"
#include "layer.h"
#include "powers_queue.h"

/******************************************************************************/

    /*! \union StackElement_p
     *
     *  \brief A union of pointers to types that can be an instance of a StackElement
     */

    union StackElement_p
    {
        Layer_t    *Layer ;     /*!< Pointer to a Layer      */
        Die_t      *Die ;       /*!< Pointer to a Die        */
        Channel_t  *Channel ;   /*!< Pointer to the Channel  */
        HeatSink_t *HeatSink ;  /*!< Pointer to the Heatsink */
    } ;

    /*! Definition of the type StackElement_p */

    typedef union StackElement_p StackElement_p ;

/******************************************************************************/

    /*! \struct StackElement_t
     *
     *  \brief Structure used to store data about the stack element that compose the 2D/3D stack.
     *
     *  A stack element is the main componenent of a 3d stack. It can be a layer,
     *  a die or a channel.
     */

    struct StackElement_t
    {
        /*! The id (string) of the stack element */

        String_t Id ;

        /*! The type of the stack element (Layer, Die or Channel) */

        StackElementType_t Type ;

        /*! Pointer to a data structure representing the type of a StackElement.
         *  This pointer must be casted depending on the value stored in
         *  StackElement::Type */

        StackElement_p Pointer ;

        /*! The number of layers composing the stack element */

        CellIndex_t NLayers ;

        /*! A pointer to a Floorplan. This field is
         *  used only if StaclElement::Type is \c TDICE_STACK_ELEMENT_DIE */

        Floorplan_t *Floorplan ;

        /*! The offset (\# layers) of the first layer of the
         *  stack element, counting from the first layer in the stack */

        CellIndex_t Offset ;

        /*! Pointer to the 'next' stack element (towards the top of the stack),
         *  to collect stack elements in a double linked list */

        struct StackElement_t *Next ;

        /*! Pointer to the 'previous' stack element (towards the bottom of the stack),
         *  to collect stack elements in a double linked list */

        struct StackElement_t *Prev ;

    } ;

    /*! Definition of the type StackElement_t */

    typedef struct StackElement_t StackElement_t ;

/******************************************************************************/

    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the stack element to initialize
     */

    void init_stack_element (StackElement_t *this) ;



    /*! Allocates a stack element in memory and sets its fields to their default
     *  value with #init_stack_element
     *
     * \return the pointer to a new StackElement
     * \return \c NULL if the memory allocation fails
     */

    StackElement_t *alloc_and_init_stack_element (void) ;



    /*! Frees the memory related to \a stack \a element
     *
     * The parametrer \a this must be a pointer previously obtained with
     * #alloc_and_init_stack_element
     *
     * \param this the address of the stack_element structure to free
     */

    void free_stack_element (StackElement_t *this) ;



    /*! Frees a list of stack elements
     *
     * If frees, calling #free_stack_element, the stack element pointed by the
     * parameter \a list and all the stack elements it finds following the
     * linked list throught the field StackElement::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_stack_elements_list (StackElement_t *list) ;



    /*! Searches for a StackElement in a linked list of stack elements.
     *
     * Id based search of a StackElement structure in a list.
     *
     * \param list the pointer to the list
     * \param id   the identifier of the stack element to be found
     *
     * \return the address of a StackElement, if founded
     * \return \c NULL if the search fails
     */

    StackElement_t *find_stack_element_in_list (StackElement_t *list, String_t id) ;


    // FIXME : what about print_formatted_stack_element ??
    // FIXME : what about print_detailed_stack_element  ??

    /*! Prints a list of stack elements as they look in the stack file
     *
     * \param list   the pointer to the first stack element in the list
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_formatted_stack_elements_list

        (StackElement_t *list, FILE *stream, String_t prefix) ;



    /*! Prints a list of detailed information about all the fields of the stack elements
     *
     * \param list   the pointer to the first stack element in the list
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_detailed_stack_elements_list

        (StackElement_t *list, FILE *stream, String_t prefix) ;



    /*! Returns the offset of the source layer of the stack element within the stack
     *
     *  \param this pointer to the stack element
     *
     *  \return the offset (\# layers) of the source layer within the stack
     */

    CellIndex_t get_source_layer_offset (StackElement_t *this) ;



    /*! Prints a matrix of temperatures as the thermal map of the stack element
     *
     * \param this          pointer to the stack element to print
     * \param dimensions    pointer to the structure storing the dimensions
     * \param temperatures  pointer to the first element of the temparature array
     * \param stream        the reference to the (already opened) stream
     */

    void print_thermal_map_stack_element
    (
        StackElement_t  *this,
        Dimensions_t    *dimensions,
        Temperature_t   *temperatures,
        FILE            *stream
    ) ;



    /*! Prints a matrix of source values as the power map of the stack element
     *
     * \param this          pointer to the stack element to print
     * \param dimensions    pointer to the structure storing the dimensions
     * \param sources       pointer to the first element of the source array
     * \param stream        the reference to the (already opened) stream
     */

    void print_power_map_stack_element
    (
        StackElement_t  *this,
        Dimensions_t    *dimensions,
        Source_t        *sources,
        FILE            *stream
    ) ;



    /*! Returns the total number of floorplan elements
     *
     * \param this address of the StackElement structure
     *
     * \return the total nyumber of floorplan elements in \a this
     */

    Quantity_t get_number_of_floorplan_elements_stack_element

        (StackElement_t *this) ;



    /*! Returns a pointer to a floorplan element in the stack element
     *
     * \param this address of the stack_element
     * \param floorplan_element_id id of the floorplan element as in the
     *                             floorplan file
     *
     * \return \c NULL if \a this is not a die stack element
     * \return \c NULL if \a floorplan_element_id does not exist in the floorplan
     * \return the pointer to the floorplan element \a floorplan_element_id
     *         that belongs to the floorplan on the die stack element
     *         \a this
     */

    FloorplanElement_t *get_floorplan_element_stack_element

        (StackElement_t *this, String_t floorplan_element_id) ;



    /*! Moves power values from \a pvaluse into a stack element
     *
     *  The function works only if \a this is a die. Otherwise, the
     *  queue \a pvalues remains unchanged. The queue \a pvalues must contain
     *  at least as many power values as floorplan elements in the floorplan
     *  of the stack element.
     *
     *  \param this address of the StackElement structure
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues does not contain enough
     *                           power values
     *  \return \c TDICE_SUCCESS if the power values have been copied or if
     *                           \a this is not a die
     */

    Error_t insert_power_values_stack_element

        (StackElement_t *this, PowersQueue_t *pvalues) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_ELEMENT_H_ */
