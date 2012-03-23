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
#include "ic_element.h"
#include "powers_queue.h"

/******************************************************************************/

    /*! \struct FloorplanElement
     *  \brief  Structure containing information about a floorplan element
     *
     *  Floorplan elements represent ...
     */

    struct FloorplanElement
    {
        /*! The id (string) to identifiy the floorplan element */

        String_t Id ;

        /*! The number of IC elements that defines the surface of the floorplan element */

        Quantity_t NICElements ;

        /*! The list of IC elements that defines the surface of the floorplan element */

        ICElement *ICElementsList ;

        /*! The area of the floorplan element, in \f$ \mu m^2 \f$,
         *  computed as the sum of the effective area of each ic element */

        ChipDimension_t EffectiveSurface ;

        /*! The list of power values representing the power consumption of
         *  the floorplan element during the thermal simulation */

        PowersQueue *PowerValues ;

        /*! Pointer to collect floorplan elements in a linked list */

        struct FloorplanElement *Next ;

    } ;

    /*! Definition of the type FloorplanElement */

    typedef struct FloorplanElement FloorplanElement ;

/******************************************************************************/



    /*! Sets all the fields of \a floorplan_element to a default value (zero or \c NULL ).
     *
     * \param floorplan_element the address of the flooprlan element to initialize
     */

    void init_floorplan_element (FloorplanElement *floorplan_element) ;



    /*! Allocates a FloorplanElement in memory and sets its fields to their
     *   default value with #init_floorplan_element
     *
     * \return the pointer to a new FloorplanElement
     * \return \c NULL if the memory allocation fails
     */

    FloorplanElement *alloc_and_init_floorplan_element (void) ;



    /*! Frees the memory related to \a floorplan_element
     *
     * The parametrer \a floorplan_element must be a pointer previously
     *  obtained with #alloc_and_init_floorplan_element
     *
     * \param floorplan_element the address of the floorplan element structure to free
     */

    void free_floorplan_element (FloorplanElement *floorplan_element) ;



    /*! Frees a list of floorplan elements
     *
     * If frees, calling #free_floorplan_element, the floorplan element pointed
     * by the parameter \a list and all the floorplan elements it finds following
     * the linked list throught the field FloorplanElement::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_floorplan_elements_list (FloorplanElement *list) ;



    /*! Searches for a StackElement in a linked list of floorplan elements.
     *
     * Id based search of a StackElement structure in a list.
     *
     * \param list the pointer to the list
     * \param id   the identifier of the floorplan element to be found
     *
     * \return the address of a StackElement, if founded
     * \return \c NULL if the search fails
     */

    FloorplanElement *find_floorplan_element_in_list

        (FloorplanElement *list, String_t id) ;



    /*! Prints the floorplan element as it looks in the stack file
     *
     * \param stream   the output stream (must be already open)
     * \param prefix   a string to be printed as prefix at the beginning of each line
     * \param floorplan_element the floorplan element to print
     */

    void print_formatted_floorplan_element

        (FILE *stream, String_t prefix, FloorplanElement *floorplan_element) ;



    /*! Prints a list of floorplan elements as they look in the stack file
     *
     * \param stream  the output stream (must be already open)
     * \param prefix  a string to be printed as prefix at the beginning of each line
     * \param list    the pointer to the first floorplan element in the list
     */

    void print_formatted_floorplan_elements_list

        (FILE *stream, String_t prefix, FloorplanElement *list) ;



    /*! Prints detailed information about all the fields of a floorplan element
     *
     * \param stream   the output stream (must be already open)
     * \param prefix   a string to be printed as prefix at the beginning of each line
     * \param floorplan_element the floorplan element to print
     */

    void print_detailed_floorplan_element

        (FILE *stream, String_t prefix, FloorplanElement *floorplan_element) ;



    /*! Prints a list of detailed information about all the fields of the flooprlan elements
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list the pointer to the first floorplan element in the list
     */

    void print_detailed_floorplan_elements_list

        (FILE *stream, String_t prefix, FloorplanElement *list) ;



    /*! Fills the source vector corresponding to a floorplan element
     *
     *  \param sources           pointer to the location of the source vector
     *                           that corresponds to the South-West thermal cell
     *                           of the layer where the floorplan is placed
     *  \param dimensions        pointer to the structure storing the dimensions
     *  \param floorplan_element pointer to the floorplan element
     *
     *  \return \c TDICE_SUCCESS if the source vector has been filled correctly
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t fill_sources_floorplan_element
    (
        Source_t         *sources,
        Dimensions       *dimensions,
        FloorplanElement *floorplan_element
    ) ;



    /*! Returns the maximum temperature of the floorplan element
     *
     *  \param floorplan_element pointer to the floorplan element
     *  \param dimensions        pointer to the structure storing the dimensions
     *  \param temperatures      pointer to the temperature of the first thermal
     *                           cell in the layer where \a floorplan_element
     *                           is placed
     *
     *  \return the maximum temperature among the thermal cells on the stack
     *          layer where \a floorplan_element is placed
     */

    Temperature_t get_max_temperature_floorplan_element
    (
        FloorplanElement *floorplan_element,
        Dimensions       *dimensions,
        Temperature_t    *temperatures
    ) ;



    /*! Returns the minimum temperature of the floorplan element
     *
     *  \param floorplan_element pointer to the floorplan element
     *  \param dimensions        pointer to the structure storing the dimensions
     *  \param temperatures      pointer to the temperature of the first thermal
     *                           cell in the layer where \a floorplan_element
     *                           is placed
     *
     *  \return the minimum temperature among the thermal cells on the stack
     *          layer where \a floorplan_element is placed
     */

    Temperature_t get_min_temperature_floorplan_element
    (
        FloorplanElement *floorplan_element,
        Dimensions       *dimensions,
        Temperature_t    *temperatures
    ) ;



    /*! Returns the average temperature of the floorplan element
     *
     *  \param floorplan_element pointer to the floorplan element
     *  \param dimensions        pointer to the structure storing the dimensions
     *  \param temperatures      pointer to the temperature of the first thermal
     *                           cell in the layer where \a floorplan_element
     *                           is placed
     *
     *  \return the average temperature among the thermal cells on the stack
     *          layer where \a floorplan_element is placed
     */

    Temperature_t get_avg_temperature_floorplan_element
    (
        FloorplanElement *floorplan_element,
        Dimensions       *dimensions,
        Temperature_t    *temperatures
    ) ;



    /*! Moves one power value from \a pvalues into \a floorplan_element
     *
     *  The queue \a pvalues must contain at least one power value
     *
     *  \param floorplan_element pointer to the floorplan element
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues is empty
     *  \return \c TDICE_SUCCESS otherwise
     */

    Error_t insert_power_values_floorplan_element

        (FloorplanElement *floorplan_element, PowersQueue *pvalues) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_ELEMENT_H_ */
