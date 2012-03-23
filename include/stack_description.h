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

#include "analysis.h"
#include "channel.h"
#include "conventional_heat_sink.h"
#include "die.h"
#include "dimensions.h"
#include "floorplan_element.h"
#include "material.h"
#include "powers_queue.h"
#include "stack_element.h"
#include "system_matrix.h"
#include "thermal_cell.h"

/******************************************************************************/

    /*! \struct StackDescription
     *
     * \brief Structure containing all the informations related to the 3d stack
     */

    struct StackDescription
    {
        /*! The name of the file used to fill the stack description */

        String_t FileName ;

        /*! The list of materials componing the layers and channel walls */

        Material *MaterialsList ;

        /*! Information about the heat dissipation throught the top surface */

        ConventionalHeatSink *ConventionalHeatSink ;

        /*! Information about the (unique) type of channel used in the 3d stack */

        Channel *Channel ;

        /*! The list of dies available to compose the 3d stack */

        Die *DiesList ;

        /*! Pointer to the top-most stack elements componing the 3Dstack */

        StackElement *TopStackElement ;

        /*! Pointer to the bottom-most stack elements componing the 3Dstack */

        StackElement *BottomStackElement ;

        /*! Collection of all the dimensions (chip, grid of cells, cell) */

        Dimensions *Dimensions ;
    } ;



    /*! Definition of the type StackDescription */

    typedef struct StackDescription StackDescription ;

/******************************************************************************/



    /*! Sets all the fields of \a stkd to a default value (zero or \c NULL ).
     *
     * \param stkd the address of the StackDescription to initialize
     */

    void init_stack_description (StackDescription* stkd) ;



    /*! Fills the \a stkd and \a analysis structures with the content
     *  of a stack file
     *
     * \param stkd the address of the StackDescription structure to fill
     * \param analysis the address of the Analysis structure to fill
     * \param filename the path of the stack file
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the parsing
     *                  of the stack description fails
     * \return \c TDICE_FAILURE otherwise
     */

    Error_t fill_stack_description

        (StackDescription* stkd, Analysis *analysis, String_t filename) ;



    /*! Frees the memory related to \a stkd
     *
     * The parametrer \a stkd must be the address of a static variable
     *
     * \param stkd the address of the StackDescription structure to free
     */

    void free_stack_description (StackDescription* stkd) ;



    /*! Prints the stack descritpion as it looks in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param stkd   the pointer to the StackDescription to print
     */

    void print_formatted_stack_description

        (FILE *stream, String_t prefix, StackDescription *stkd) ;



    /*! Prints a list of detailed information about the stack descritpion
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param stkd   the pointer to the StackDescription to print
     */

    void print_detailed_stack_description

        (FILE *stream, String_t prefix, StackDescription* stkd) ;



    /*! Prints the floorplans (detilaed info) used in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param stkd   the pointer to the StackDescription
     */

    void print_floorplans

        (FILE *stream, String_t prefix, StackDescription *stkd) ;



    /*! Fills the 3d grid of thermal cells
     *
     *  The function fills all the thermal cells representing the 3d volume
     *  of the IC stack, starting from the bottom-most stack element. If the
     *  heat sink is used, it adapts the top-most layer to connect it to
     *  the enviroment and enable heat dissipation. If the simulation typed
     *  is steady state it resets the capacitance of each thermal cell.
     *
     *  \param thermal_cells pointer to the first thermal cell in the 3d grid
     *  \param analysis      pointer to the steructure containing info about the type of analysis
     *  \param stkd          pointer to the stack descritpion structure
     */

    void fill_thermal_cell_stack_description
    (
        ThermalCell      *thermal_cells,
        Analysis         *analysis,
        StackDescription *stkd
    ) ;



    /*! Fills the source vector
     *
     *  The function resets the source vector. If the heat sink is used,
     *  it inserts power values into the thermal cells in the top-most layer.
     *  Then, it extracts power traces from each floorplan elements and power
     *  related to the inlet of the channels.
     *
     *  \param sources       pointer to the first element in the source vector
     *  \param thermal_cells pointer to the first thermal cell in the 3d grid
     *  \param stkd          pointer to the stack descritpion structure
     *
     *  \return \c TDICE_SUCCESS if the source vector has been filled correctly
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t fill_sources_stack_description
    (
        Source_t         *sources,
        ThermalCell      *thermal_cells,
        StackDescription *stkd
    ) ;



    /*! Fills the system matrix
     *
     *  The function fills, stack element by stack element, all the columns
     *  of the system matrix. If the heat sink is used, the coefficients
     *  related to the last layer are updated.
     *
     *  \param system_matrix copy of the system matrix structure
     *  \param thermal_cells pointer to the first thermal cell in the 3d grid
     *  \param stkd          pointer to the stack descritpion structure
     */

    void fill_system_matrix_stack_description
    (
        SystemMatrix      system_matrix,
        ThermalCell      *thermal_cells,
        StackDescription *stkd
    ) ;



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

        (StackDescription* stkd, String_t stack_element_id) ;



    /*! Returns the total number of floorplan elements in the whole 3d stack
     *
     * \param stkd address of the StackDescription structure
     *
     * \return the total nyumber of floorplan elements in the 3d stack
     */

    Quantity_t get_total_number_of_floorplan_elements (StackDescription *stkd) ;



    /*! Returns a pointer to a floorplan element in the stack
     *
     * \param stkd address of the StackDescription structure
     * \param stack_element_id id of the stack element as in the stack file
     * \param floorplan_element_id id of the floorplan element as in the
     *                             floorplan file
     *
     * \return \c NULL if \a stack_element_id does not exist in the stack
     * \return \c NULL if \a stack_element_id is not a die stack element
     * \return \c NULL if \a floorplan_element_id does not exist in the floorplan
     * \return the pointer to the floorplan element \a floorplan_element_id
     *         that belongs to the floorplan on the die stack element
     *         \a stack_element_id
     */

    FloorplanElement *get_floorplan_element

        (StackDescription *stkd,
         String_t stack_element_id, String_t floorplan_element_id) ;



    /*! Inserts one power values from \a pvaluse into each floorplan element
     *  in the stack
     *
     *  The queue \a pvalues must contain at least as many power values as
     *  the number of floorplan elements in the entire stack. \a pvalues is a
     *  FIFO queue and, whithin the stack, elements are considered from the
     *  bottom (the last element declared in the stack section of the
     *  stack description file). If the stack has two dies \c A (bottom) and
     *  \c B (top) and A has a floorplan with \c n elements while \c B has \c m
     *  elements, then \a pvalues must contain at least \c nm elements. The
     *  first \c n power values will be given to \c A and the remaining will
     *  be assigned to \c B
     *
     *  \param stkd address of the StackDescription structure
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues does not contain enough
     *                           power values
     *  \return \c TDICE_SUCCESS otherwise
     */

    Error_t insert_power_values (StackDescription *stkd, PowersQueue *pvalues) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_DESCRIPTION_H_ */
