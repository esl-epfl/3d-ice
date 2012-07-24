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
#include "layer.h"

/******************************************************************************/

    /*! \union StackElement_p
     *
     *  \brief A union of pointers to types that can be an instance
     *         of a StackElement
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
     *  \brief Structure used to store data about the stack element
     *         that compose the 2D/3D stack.
     *
     *  A stack element is the main componenent of a 3d stack. It can
     *  be a layer, a die or a channel.
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

        /*! The offset (\# layers) of the first layer of the
         *  stack element, counting from the first layer in the stack */

        CellIndex_t Offset ;
    } ;

    /*! Definition of the type StackElement_t */

    typedef struct StackElement_t StackElement_t ;



/******************************************************************************/



    /*! Inits the fields of the \a stkel structure with default values
     *
     * \param stkel the address of the structure to initalize
     */

    void stack_element_init (StackElement_t *stkel) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void stack_element_copy (StackElement_t *dst, StackElement_t *src) ;



    /*! Destroys the content of the fields of the structure \a stkel
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a stac_element_init .
     *
     * \param stkel the address of the structure to destroy
     */

    void stack_element_destroy (StackElement_t *stkel) ;



    /*! Allocates memory for a structure of type StackElement_t
     *
     * The content of the new structure is set to default values
     * calling \a stack_element_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    StackElement_t *stack_element_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a stkel
     *
     * \param stkel the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a stkel is \c NULL
     */

    StackElement_t *stack_element_clone (StackElement_t *stkel) ;



    /*! Frees the memory space pointed by \a stkel
     *
     * The function destroys the structure \a stkel and then frees
     * its memory. The pointer \a stkel must have been returned by
     * a previous call to \a stack_element_calloc or
     * \a stack_element_clone .
     *
     * If \a stkel is \c NULL, no operation is performed.
     *
     * \param stkel the pointer to free
     */

    void stack_element_free (StackElement_t *stkel) ;



    /*! Tests if two stack elements have the same Id
     *
     * \param stkel the first stack element
     * \param other the second stack element
     *
     * \return \c TRUE if \a stkel and \a other have the same Id
     * \return \c FALSE otherwise
     */

    bool stack_element_same_id (StackElement_t *stkel, StackElement_t *other) ;



    /*! Prints the stack element declaration as it looks in the stack file
     *
     * \param stkel the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void stack_element_print

        (StackElement_t *stkel, FILE *stream, String_t prefix) ;



    /*! Returns the offset of the source layer of the stack element
     *  within the stack
     *
     *  \param stkel pointer to the stack element
     *
     *  \return the offset (\# layers) of the source layer within the stack
     */

    CellIndex_t get_source_layer_offset (StackElement_t *stkel) ;



    /*! Prints a matrix of temperatures as the thermal map of the stack element
     *
     * \param stkel          pointer to the stack element to print
     * \param dimensions    pointer to the structure storing the dimensions
     * \param temperatures  pointer to the first element of the temparature array
     * \param stream        the reference to the (already opened) stream
     */

    void stack_element_print_thermal_map
    (
        StackElement_t  *stkel,
        Dimensions_t    *dimensions,
        Temperature_t   *temperatures,
        FILE            *stream
    ) ;



    /*! Prints a matrix of source values as the power map of the stack element
     *
     * \param stkel          pointer to the stack element to print
     * \param dimensions    pointer to the structure storing the dimensions
     * \param sources       pointer to the first element of the source array
     * \param stream        the reference to the (already opened) stream
     */

    void stack_element_print_power_map
    (
        StackElement_t  *stkel,
        Dimensions_t    *dimensions,
        Source_t        *sources,
        FILE            *stream
    ) ;



    /*! Returns the total number of floorplan elements
     *
     * \param stkel address of the StackElement structure
     *
     * \return the total nyumber of floorplan elements in \a stkel
     */

    Quantity_t get_number_of_floorplan_elements_stack_element

        (StackElement_t *stkel) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_ELEMENT_H_ */
