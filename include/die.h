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

#ifndef _3DICE_DIE_H_
#define _3DICE_DIE_H_

/*! \file die.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "floorplan.h"
#include "layer.h"
#include "system_matrix.h"
#include "thermal_cell.h"

/******************************************************************************/

    /*! \struct Die
     *
     *  \brief Structure used to store data about the dies that compose the 2D/3D stack.
     *
     *  Dies are one of the elements that can be used to build a 3d stack
     */

    struct Die
    {
        /*! The id (string) of the die */

        String_t Id ;

        /*! To know, after the parsing of a stack file, if a
         *  die has been declared but never used in the stack */

        Quantity_t Used ;

        /*! The number of layer composing the die */

        CellIndex_t NLayers ;

        /*! The offset (\# layers) of the source layer within the die */

        CellIndex_t SourceLayerOffset ;

        /*! Pointer to the top-most layer */

        Layer *TopLayer ;

        /*! Pointer to the source layer */

        Layer *SourceLayer ;

        /*! Pointer to the bottom-most layer */

        Layer *BottomLayer ;

        /*! To collect dies in a linked list */

        struct Die *Next ;

    } ;

    /*! Definition of the type Die */

    typedef struct Die Die ;

/******************************************************************************/



    /*! Sets all the fields of \a die to a default value (zero or \c NULL ).
     *
     * \param die the address of the die to initialize
     */

    void init_die (Die *die) ;



    /*! Allocates a die in memory and sets its fields to their default
     *  value with \c init_die
     *
     * \return the pointer to a new Die
     * \return \c NULL if the memory allocation fails
     */

    Die *alloc_and_init_die (void) ;



    /*! Frees the memory related to \a die
     *
     * The parametrer \a die must be a pointer previously obtained with
     * \c alloc_and_init_die
     *
     * \param die the address of the die structure to free
     */

    void free_die (Die *die) ;



    /*! Frees a list of dies
     *
     * If frees, calling \c free_die, the die pointed by the
     * parameter \a list and all the dies it finds following the
     * linked list throught the field Die::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_dies_list (Die *list) ;



    /*! Searches for a Die in a linked list of dies.
     *
     * Id based search of a Die structure in a list.
     *
     * \param list the pointer to the list
     * \param id   the identifier of the die to be found
     *
     * \return the address of a Die, if founded
     * \return \c NULL if the search fails
     */

    Die *find_die_in_list (Die *list, String_t id) ;



    /*! Prints the die as it looks in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param die    the die to print
     */

    void print_formatted_die (FILE *stream, String_t prefix, Die *die) ;



    /*! Prints a list of dies as they look in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first die in the list
     */

    void print_formatted_dies_list (FILE *stream, String_t prefix, Die *list) ;



    /*! Prints detailed information about all the fields of a die
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param die    the die to print
     */

    void print_detailed_die (FILE *stream, String_t prefix, Die *die) ;



    /*! Prints a list of detailed information about all the fields of the dies
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first die in the list
     */

    void print_detailed_dies_list (FILE *stream, String_t prefix, Die *list) ;



    /*! Fills the thermal cells corresponding to a die
     *
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param delta_time    the time resolution of the thermal simulation
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param layer_index   offset (\# layers) of the die within the stack
     *  \param die           pointer to the die
     */

    void fill_thermal_cell_die
    (
        ThermalCell *thermal_cells,
        Time_t       delta_time,
        Dimensions  *dimensions,
        CellIndex_t  layer_index,
        Die         *die
    ) ;



    /*! Fills the source vector corresponding to a die
     *
     *  \param sources     pointer to the first element in the source vector
     *  \param dimensions  pointer to the structure storing the dimensions
     *  \param layer_index offset (\# layers) of the die within the stack
     *  \param floorplan   pointer to the floorplan placed on the source layer
     *  \param die         pointer to the die
     *
     *  \return \c TDICE_SUCCESS if the source vector has been filled correctly
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t fill_sources_die
    (
        Source_t   *sources,
        Dimensions *dimensions,
        CellIndex_t layer_index,
        Floorplan  *floorplan,
        Die        *die
    ) ;



    /*! Fills the system matrix
     *
     *  \param die           pointer to the die
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   offset (\# layers) of the die within the stack
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix fill_system_matrix_die
    (
        Die          *die,
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,
        CellIndex_t   layer_index,
        SystemMatrix  system_matrix
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
