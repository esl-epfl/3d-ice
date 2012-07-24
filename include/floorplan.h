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
#include "floorplan_matrix.h"
#include "floorplan_element_list.h"
#include "powers_queue.h"

/******************************************************************************/

    /*! \struct Floorplan_t
     *
     *  \brief The floorplan representing the IC as a set of floorplan elements
     */

    struct Floorplan_t
    {
        /*! The name of the file scanned to fill the floorplan */

        String_t FileName ;

        /*! The number of floorplan elements in the floorplan  */

        Quantity_t NElements ;

        /*! Pointer to the list of floorplan elements */

        FloorplanElementList_t ElementsList ;

        /*! Matrix that stores the surface coefficients to transform
            power traces of each floorplan element into a source vector */

        FloorplanMatrix_t SurfaceCoefficients ;

        /*! Power vector to perform the mv-multiplication
             to get the source vector */

        Power_t *Bpowers ;
    } ;

    /*! Definition of the type Floorplan_t */

    typedef struct Floorplan_t Floorplan_t ;



/******************************************************************************/



    /*! Inits the fields of the \a floorplan structure with default values
     *
     * \param floorplan the address of the structure to initalize
     */

    void floorplan_init (Floorplan_t *floorplan) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void floorplan_copy (Floorplan_t *dst, Floorplan_t *src) ;



    /*! Destroys the content of the fields of the structure \a floorplan
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a florplan_init .
     *
     * \param floorplan the address of the structure to destroy
     */

    void floorplan_destroy (Floorplan_t *floorplan) ;



    /*! Allocates memory for a structure of type Floorplan_t
     *
     * The content of the new structure is set to default values
     * calling \a floorplan_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    Floorplan_t *floorplan_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a floorplan
     *
     * \param floorplan the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a floorplan is \c NULL
     */

    Floorplan_t *floorplan_clone (Floorplan_t *floorplan) ;



    /*! Frees the memory space pointed by \a floorplan
     *
     * The function destroys the structure \a floorplan and then frees
     * its memory. The pointer \a floorplan must have been returned by
     * a previous call to \a floorplan_calloc or \a floorplan_clone .
     *
     * If \a floorplan is \c NULL, no operation is performed.
     *
     * \param floorplan the pointer to free
     */

    void floorplan_free (Floorplan_t *floorplan) ;



    /*! Prints the structure of the floorplan as it looks in the floorplan file
     *
     * \param floorplan the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void floorplan_print (Floorplan_t *floorplan, FILE *stream, String_t prefix) ;



    /*! Parses the floorplan file and fills the floorplan structure
     *
     * \param floorplan       the floorplan structure to fill
     * \param dimensions pointer to the structure storing the dimensions of the stack
     * \param file_name  path to the floorplan file to parse
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the parsing
     *                  of the floorplan fails
     * \return \c TDICE_FAILURE otherwise
     */

    Error_t fill_floorplan

        (Floorplan_t *floorplan, Dimensions_t *dimensions, String_t file_name) ;



    /*! Fills the source vector corresponding to a floorplan
     *
     *  \param floorplan    pointer to the floorplan placed on the source layer
     *  \param sources pointer to the location of the source vector
     *                 that corresponds to the South-West thermal cell
     *                 of the layer where the floorplan is placed
     *
     *  \return \c TDICE_SUCCESS if the source vector has been filled correctly
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t fill_sources_floorplan (Floorplan_t *floorplan, Source_t *sources) ;



    /*! Returns the total number of floorplan elements in the floorplan
     *
     * \param floorplan address of the Floorplan structure
     *
     * \return the total nyumber of floorplan elements in \a floorplan
     */

    Quantity_t get_number_of_floorplan_elements_floorplan

        (Floorplan_t *floorplan) ;



    /*! Returns a pointer to a floorplan element in the floorplan
     *
     * \param floorplan address of the floorplan
     * \param floorplan_element_id id of the floorplan element as in the
     *                             floorplan file
     *
     * \return \c NULL if \a floorplan_element_id does not exist
     * \return the pointer to the floorplan element \a floorplan_element_id
     */

    FloorplanElement_t *get_floorplan_element

        (Floorplan_t *floorplan, String_t floorplan_element_id) ;



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

        (Floorplan_t *floorplan, PowersQueue_t *pvalues) ;



    /*! Returns the maximum temperature of each floorplan element
     *  in the given floorplan
     *
     *  \param floorplan pointer to the floorplan
     *  \param dimensions pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a floorplan is placed
     *  \param n_floorplan_elements (\c OUT) the number of floorplan elements
     *  \param max_temperatures (\c IN/OUT) addres to the memory where
     *                      temperatures will be written into.
     *
     *  \return the same address as \a max_temperatures if \a max_temperatures
     *          is not \c NULL.
     *  \return the address of a memory containing \a n_floorplan_elements
     *          temperatures if \a max_temperatures is \c NULL
     */

    Temperature_t *get_all_max_temperatures_floorplan
    (
        Floorplan_t   *floorplan,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures,
        Quantity_t    *n_floorplan_elements,
        Temperature_t *max_temperatures
    ) ;



    /*! Returns the minimum temperature of each floorplan element
     *  in the given floorplan
     *
     *  \param floorplan pointer to the floorplan
     *  \param dimensions pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a floorplan is placed
     *  \param n_floorplan_elements (\c OUT) the number of floorplan elements
     *  \param min_temperatures (\c IN/OUT) addres to the memory where
s     *                      temperatures will be written into.
     *
     *  \return the same address as \a min_temperatures if \a min_temperatures
     *          is not \c NULL.
     *  \return the address of a memory containing \a n_floorplan_elements
     *          temperatures if \a min_temperatures is \c NULL
     */

    Temperature_t *get_all_min_temperatures_floorplan
    (
        Floorplan_t   *floorplan,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures,
        Quantity_t    *n_floorplan_elements,
        Temperature_t *min_temperatures
    ) ;



    /*! Returns the average temperature of each floorplan element
     *  in the given floorplan
     *
     *  \param floorplan pointer to the floorplan
     *  \param dimensions pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a floorplan is placed
     *  \param n_floorplan_elements (\c OUT) the number of floorplan elements
     *  \param avg_temperatures (\c IN/OUT) addres to the memory where
     *                      temperatures will be written into.
     *
     *  \return the same address as \a avg_temperatures if \a avg_temperatures
     *          is not \c NULL.
     *  \return the address of a memory containing \a n_floorplan_elements
     *          temperatures if \a avg_temperatures is \c NULL
     */

    Temperature_t *get_all_avg_temperatures_floorplan
    (
        Floorplan_t   *floorplan,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures,
        Quantity_t    *n_floorplan_elements,
        Temperature_t *avg_temperatures
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_H_ */
