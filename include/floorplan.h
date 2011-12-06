/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_FLOORPLAN_H_
#define _3DICE_FLOORPLAN_H_

/*! \file floorplan.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "floorplan_element.h"
#include "powers_queue.h"

/******************************************************************************/

    /*! \struct Floorplan
     *
     *  \brief The floorplan representing the IC as a set of floorplan elements
     */

    struct Floorplan
    {
        /*! The name of the file scanned to fill the floorplan */

        char *FileName ;

        /*! The number of floorplan elements in the floorplan  */

        uint8_t NElements ;

        /*! Pointer to the list of floorplan elements */

        FloorplanElement *ElementsList ;

    } ;

    /*! Definition of the type Floorplan */

    typedef struct Floorplan Floorplan ;

/******************************************************************************/



    /*! Sets all the fields of \a floorplan to a default value (zero or \c NULL ).
     *
     * \param floorplan the address of the flooprlan to initialize
     */

    void init_floorplan (Floorplan *floorplan) ;



    /*! Allocates a Floorplan in memory and sets its fields to their
     *  default value with #init_floorplan
     *
     * \return the pointer to a new Floorplan
     * \return \c NULL if the memory allocation fails
     */

    Floorplan *alloc_and_init_floorplan (void) ;



    /*! Frees the memory related to \a floorplan
     *
     * The parametrer \a floorplan must be a pointer previously
     * obtained with #alloc_and_init_floorplan
     *
     * \param floorplan the address of the floorplan structure to free
     */

    void free_floorplan (Floorplan *floorplan) ;



    /*! Prints detailed information about all the fields of a floorplan
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param floorplan the floorplan    to print
     */

    void print_detailed_floorplan (FILE *stream, char *prefix, Floorplan *floorplan) ;



    /*! Parses the file pointed by Floorplan::FileName and fills the \a floorplan structure
     *
     * \param floorplan  the floorplan structure to fill
     * \param dimensions pointer to the structure storing the dimensions of the stack
     * \param file_name  path to the floorplan file to parse
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the parsing
     *                  of the floorplan fails
     * \return \c TDICE_FAILURE otherwise
     */

    Error_t fill_floorplan

        (Floorplan *floorplan, Dimensions *dimensions, char *file_name) ;



    /*! Fills the source vector corresponding to a floorplan
     *
     *  \param sources     pointer to the location of the source vector
     *                     that corresponds to the South-West thermal cell
     *                     of the layer where the floorplan is placed
     *  \param dimensions  pointer to the structure storing the dimensions
     *  \param floorplan   pointer to the floorplan placed on the source layer
     *
     *  \return \c TDICE_SUCCESS if the source vector has been filled correctly
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t fill_sources_floorplan
    (
        double     *sources,
        Dimensions *dimensions,
        Floorplan  *floorplan
    ) ;



    /*! Returns the total number of floorplan elements in \a floorplan
     *
     * \param floorplan address of the Floorplan structure
     *
     * \return the total nyumber of floorplan elements in \a floorplan
     */

    uint32_t get_number_of_floorplan_elements_floorplan (Floorplan *floorplan) ;



    /*! Inserts power values from \a pvaluse into each floorplan element
     *
     *  The queue \a pvalues must contain at least as many power values
     *  as floorplan elements in \a floorplan . Floorplan elements are
     *  considered in the same order as they appear in the floorplan file: the
     *  first element popped from \a pvalues (a fifo queue) goes to the first
     *  floorplan element in the flp file, the second to the second, etc ...
     *
     *  \param floorplan pointer to the floorplan
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues does not contain enough
     *                           power values
     *  \return \c TDICE_SUCCESS otherwise
     */

    Error_t insert_power_values_floorplan

        (Floorplan *floorplan, PowersQueue *pvalues) ;



    int get_max_temperature_floorplan
    (
        Floorplan  *floorplan,
        char       *floorplan_element_id,
        Dimensions *dimensions,
        double     *temperatures,
        double     *max_temperature
    ) ;

    int get_min_temperature_floorplan
    (
        Floorplan  *floorplan,
        char       *floorplan_element_id,
        Dimensions *dimensions,
        double     *temperatures,
        double     *min_temperature
    ) ;

    int get_avg_temperature_floorplan
    (
        Floorplan  *floorplan,
        char       *floorplan_element_id,
        Dimensions *dimensions,
        double     *temperatures,
        double     *avg_temperature
    ) ;

    int get_all_max_temperatures_floorplan
    (
        Floorplan  *floorplan,
        Dimensions *dimensions,
        double     *temperatures,
        double     *max_temperature
    ) ;

    void print_all_max_temperatures_floorplan
    (
        Floorplan  *floorplan,
        Dimensions *dimensions,
        double     *temperatures,
        FILE       *stream
    ) ;

    int get_all_min_temperatures_floorplan
    (
        Floorplan  *floorplan,
        Dimensions *dimensions,
        double     *temperatures,
        double     *min_temperature
    ) ;

    void print_all_min_temperatures_floorplan
    (
        Floorplan  *floorplan,
        Dimensions *dimensions,
        double     *temperatures,
        FILE       *stream
    ) ;

    int get_all_avg_temperatures_floorplan
    (
        Floorplan  *floorplan,
        Dimensions *dimensions,
        double     *temperatures,
        double     *avg_temperature
    ) ;

    void print_all_avg_temperatures_floorplan
    (
        Floorplan  *floorplan,
        Dimensions *dimensions,
        double     *temperatures,
        FILE       *stream
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_H_ */
