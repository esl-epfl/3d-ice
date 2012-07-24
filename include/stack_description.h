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

#ifndef _3DICE_STACK_DESCRIPTION_H_
#define _3DICE_STACK_DESCRIPTION_H_

/*! \file stack_description.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "channel.h"
#include "heat_sink.h"
#include "die_list.h"
#include "layer_list.h"
#include "dimensions.h"
#include "material_list.h"
#include "stack_element_list.h"

/******************************************************************************/

    /*! \struct StackDescription_t
     *
     * \brief Structure containing all the informations related to the 3d stack
     */

    struct StackDescription_t
    {
        /*! The name of the file used to fill the stack description */

        String_t FileName ;

        /*! The list of materials componing the layers and channel walls */

        MaterialList_t Materials ;

        /*! Information about the heat dissipation throught the top surface */

        HeatSink_t *HeatSink ;

        /*! Information about the type of channel used in the 3d stack */

        Channel_t *Channel ;

        /*! The list of layers available to compose the 3d stack */

        LayerList_t Layers ;

        /*! The list of dies available to compose the 3d stack */

        DieList_t Dies ;

        /*! Collection of all the dimensions (chip, grid of cells, cell) */

        Dimensions_t *Dimensions ;

        /*! The list of stack elements composing the 3Dstack. The list
         *  stores the stack elements in such a way that the head/begin/first
         *  of the list points to the top most stack element while the
         *  tail/end/last points to the bottom most one. Crossing the list
         *  in a reverse order is necessary to get stack elements from bottom
         *  to top. */

        StackElementList_t StackElements ;
    } ;



    /*! Definition of the type StackDescription_t */

    typedef struct StackDescription_t StackDescription_t ;



/******************************************************************************/



    /*! Inits the fields of the \a skd structure with default values
     *
     * \param stkd the address of the structure to initalize
     */

    void stack_description_init (StackDescription_t* stkd) ;



    /*! Destroys the content of the fields of the structure \a stkd
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a stack_description_init .
     *
     * \param stkd the address of the structure to destroy
     */

    void stack_description_destroy (StackDescription_t *stkd) ;



    /*! Prints the sack descritpion as it looks in the stack file
     *
     * \param stkd the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void stack_description_print

        (StackDescription_t *stkd, FILE *stream, String_t prefix) ;



    /*! Returns the number of floorplan elements in a stack element
     *
     * The parameter \a stack_element_id must refer to the ID given to a stack
     * element (of type die) in the stack file used to to fill \a stkd
     *
     * \param stkd address of the StackDescription structure
     * \param stack_element_id the id of the stack element
     *
     * \return \c 0 if stack_element_id does not exist in the stack or if it
     *              does not refer to a die.
     * \return the number of floorplan elements in \a stack_element_id
     */

    Quantity_t get_number_of_floorplan_elements

        (StackDescription_t *stkd, String_t stack_element_id) ;



    /*! Returns the total number of floorplan elements in the whole 3d stack
     *
     * \param stkd address of the StackDescription structure
     *
     * \return the total nyumber of floorplan elements in the 3d stack
     */

    Quantity_t get_total_number_of_floorplan_elements

        (StackDescription_t *stkd) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_DESCRIPTION_H_ */
