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
#include "die.h"
#include "layer.h"
#include "dimensions.h"
#include "material.h"
#include "stack_element.h"

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

        Material_t *MaterialsList ;

        /*! Information about the heat dissipation throught the top surface */

        HeatSink_t *HeatSink ;

        /*! Information about the (unique) type of channel used in the 3d stack */

        Channel_t *Channel ;

        /*! The list of layers available to compose the 3d stack */

        Layer_t *LayersList ;

        /*! The list of dies available to compose the 3d stack */

        Die_t *DiesList ;

        /*! Collection of all the dimensions (chip, grid of cells, cell) */

        Dimensions_t *Dimensions ;

        /*! Pointer to the top-most stack elements componing the 3Dstack */

        StackElement_t *TopStackElement ;

        /*! Pointer to the bottom-most stack elements componing the 3Dstack */

        StackElement_t *BottomStackElement ;
    } ;



    /*! Definition of the type StackDescription_t */

    typedef struct StackDescription_t StackDescription_t ;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param stkd the address of the StackDescription to initialize
     */

    void init_stack_description (StackDescription_t* stkd) ;



    /*! Frees the memory related to \a stkd
     *
     * The parametrer \a stkd must be the address of a static variable
     *
     * \param stkd the address of the StackDescription structure to free
     */

    void destroy_stack_description (StackDescription_t *stkd) ;



    /*! Prints the stack descritpion as it looks in the stack file
     *
     * \param stkd   the pointer to the StackDescription to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_stack_description

        (StackDescription_t *v, FILE *stream, String_t prefix) ;



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

    Quantity_t get_total_number_of_floorplan_elements (StackDescription_t *stkd) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_DESCRIPTION_H_ */
