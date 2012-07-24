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

#ifndef _3DICE_FLOORPLAN_ELEMENT_H_
#define _3DICE_FLOORPLAN_ELEMENT_H_

/*! \file floorplan_element.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "ic_element_list.h"
#include "powers_queue.h"

/******************************************************************************/

    /*! \struct FloorplanElement_t
     *  \brief  Structure containing information about a floorplan element
     *
     *  Floorplan elements represent ...
     */

    struct FloorplanElement_t
    {
        /*! The id (string) to identifiy the floorplan element */

        String_t Id ;

        /*! The number of IC elements that defines the surface of
         *  the floorplan element */

        Quantity_t NICElements ;

        /*! The list of IC elements that defines the surface of
         *  the floorplan element */

        ICElementList_t ICElements ;

        /*! The area of the floorplan element, in \f$ \mu m^2 \f$,
         *  computed as the sum of the effective area of each ic element */

        ChipDimension_t Area ;

        /*! The list of power values representing the power consumption of
         *  the floorplan element during the thermal simulation */

        PowersQueue_t *PowerValues ;
    } ;

    /*! Definition of the type FloorplanElement_t */

    typedef struct FloorplanElement_t FloorplanElement_t ;



/******************************************************************************/



    /*! Inits the fields of the \a flpel structure with default values
     *
     * \param flpel the address of the structure to initalize
     */

    void floorplan_element_init (FloorplanElement_t *flpel) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void floorplan_element_copy

        (FloorplanElement_t *dst, FloorplanElement_t *src) ;



    /*! Destroys the content of the fields of the structure \a flpel
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a floorplan_element_init .
     *
     * \param flpel the address of the structure to destroy
     */

    void floorplan_element_destroy (FloorplanElement_t *flpel) ;



    /*! Allocates memory for a structure of type FloorplanElement_t
     *
     * The content of the new structure is set to default values
     * calling \a floorplan_element_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    FloorplanElement_t *floorplan_element_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a flpel
     *
     * \param flpel the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a flpel is \c NULL
     */

    FloorplanElement_t *floorplan_element_clone (FloorplanElement_t *flpel) ;



    /*! Frees the memory space pointed by \a flpel
     *
     * The function destroys the structure \a flpel and then frees
     * its memory. The pointer \a flpel must have been returned by
     * a previous call to \a floorplan_element_calloc or
     * \a floorplan_element_clone .
     *
     * If \a flpel is \c NULL, no operation is performed.
     *
     * \param flpel the pointer to free
     */

    void floorplan_element_free (FloorplanElement_t *flpel) ;



    /*! Tests if two floorplan elements have the same Id
     *
     * \param flpel the first floorplan element
     * \param other the second floorplan element
     *
     * \return \c TRUE if \a flpel and \a other have the same Id
     * \return \c FALSE otherwise
     */

    bool floorplan_element_same_id

        (FloorplanElement_t *flpel, FloorplanElement_t *other) ;



    /*! Prints the floorplan element structure as it looks in the
     *  floorplan file
     *
     * \param flpel the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void floorplan_element_print

        (FloorplanElement_t *flpel, FILE *stream, String_t prefix) ;



    /*! Returns the maximum temperature of the floorplan element
     *
     *  \param flpel         pointer to the floorplan element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where the floorplan element
     *                      is placed
     *
     *  \return the maximum temperature among the thermal cells on the stack
     *          layer where \a flpel is placed
     */

    Temperature_t get_max_temperature_floorplan_element
    (
        FloorplanElement_t *flpel,
        Dimensions_t       *dimensions,
        Temperature_t      *temperatures
    ) ;



    /*! Returns the minimum temperature of the floorplan element
     *
     *  \param flpel         pointer to the floorplan element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where the floorplan element
     *                      is placed
     *
     *  \return the minimum temperature among the thermal cells on the stack
     *          layer where \a flpel is placed
     */

    Temperature_t get_min_temperature_floorplan_element
    (
        FloorplanElement_t *flpel,
        Dimensions_t       *dimensions,
        Temperature_t      *temperatures
    ) ;



    /*! Returns the average temperature of the floorplan element
     *
     *  \param flpel         pointer to the floorplan element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a flpel
     *                      is placed
     *
     *  \return the average temperature among the thermal cells on the stack
     *          layer where \a flpel is placed
     */

    Temperature_t get_avg_temperature_floorplan_element
    (
        FloorplanElement_t *flpel,
        Dimensions_t       *dimensions,
        Temperature_t      *temperatures
    ) ;



    /*! Moves one power value from \a pvalues into \a flpel
     *
     *  The queue \a pvalues must contain at least one power value
     *
     *  \param flpel    pointer to the floorplan element
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues is empty
     *  \return \c TDICE_SUCCESS otherwise
     */

    Error_t insert_power_values_floorplan_element

        (FloorplanElement_t *flpel, PowersQueue_t *pvalues) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_ELEMENT_H_ */
