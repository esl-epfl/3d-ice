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

#ifndef _3DICE_IC_ELEMENT_H_
#define _3DICE_IC_ELEMENT_H_

/*! \file ic_element.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "dimensions.h"

/******************************************************************************/

    /*! \struct ICElement
     *
     *  A rectangle that represent the surface occupied by a flooprlan element
     */

    struct ICElement
    {
        /*! The south-west X coordinate, in \f$ \mu m \f$ */

        ChipDimension_t SW_X ;

        /*! The south-west Y coordinate, in \f$ \mu m \f$ */

        ChipDimension_t SW_Y ;

        /*! The length (west <-> east) of the ic element, in \f$ \mu m \f$ */

        ChipDimension_t Length ;

        /*! The width (south <-> north) of the ic element, in \f$ \mu m \f$ */

        ChipDimension_t Width ;

        /*! The length of the ic element, in \f$ \mu m \f$,
         *  as the sum of all the lengths of the cells in the source layer
         *  covered by the ic element */

        ChipDimension_t EffectiveLength ;

        /*! The width of the ic element, in \f$ \mu m \f$,
         *  as the sum of all the widths of the cells in the source layer
         *  covered by the ic element */

        ChipDimension_t EffectiveWidth ;

        /*! The index of the row of the thermal cell where the south-west
         *  corner of the ic element is placed */

        CellIndex_t SW_Row ;

        /*! The index of the column of the thermal cell where the south-west
         *  corner of the ic element is placed */

        CellIndex_t SW_Column ;

        /*! The index of the row of the thermal cell where the north-east
         *  corner of the ic element is placed */

        CellIndex_t NE_Row ;

        /*! The index of the column of the thermal cell where the north-east
         *  corner of the ic element is placed */

        CellIndex_t NE_Column ;

        /*! pointer to collect IC elements in a linked list */

        struct ICElement *Next ;

    } ;

    /*! Definition of the type ICElement */

    typedef struct ICElement ICElement ;

/******************************************************************************/

    /*! Sets all the fields of \a icelement to a default value (zero or \c NULL ).
     *
     * \param icelement the address of the ic element to initialize
     */

    void init_ic_element (ICElement *icelement) ;



    /*! Allocates an ICElement in memory and sets its fields to their
     *   default value with #init_ic_element
     *
     * \return the pointer to a new ICElement
     * \return \c NULL if the memory allocation fails
     */

    ICElement *alloc_and_init_ic_element (void) ;



    /*! Frees the memory related to \a icelement
     *
     * The parametrer \a icelement must be a pointer previously
     *  obtained with #alloc_and_init_ic_element
     *
     * \param icelement the address of the ic element structure to free
     */

    void free_ic_element (ICElement *icelement) ;



    /*! Frees a list of ic elements
     *
     * If frees, calling \c free_ic_element, the ic element pointed by the
     * parameter \a list and all the ic elements it finds following the
     * linked list throught the field ICElement::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_ic_elements_list (ICElement *list) ;



    /*! Prints detailed information about all the fields of an ic element
     *
     * \param stream    the output stream (must be already open)
     * \param prefix    a string to be printed as prefix at the beginning of each line
     * \param icelement the ic element to print
     */

    void print_detailed_ic_element

        (FILE *stream, String_t prefix, ICElement *icelement) ;



    /*! Prints a list of detailed information about all the fields of the icelements
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first ic element in the list
     */

    void print_detailed_ic_elements_list

        (FILE *stream, String_t prefix, ICElement *list) ;



    /*! Prints the ic element as it looks in the stack file
     *
     * \param stream    the output stream (must be already open)
     * \param prefix    a string to be printed as prefix at the beginning of each line
     * \param icelement the ic element to print
     */

    void print_formatted_ic_element

        (FILE *stream, String_t prefix, ICElement *icelement) ;



    /*! Prints a list of ic elements as they look in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first ic element in the list
     */

    void print_formatted_ic_elements_list

        (FILE *stream, String_t prefix, ICElement *list) ;



    /*! Checks if two ic elements overlap
     *
     *  The control is based on the real coordinates of the ic element, i.e. the
     *  coordinates read from the floorplan file.
     *
     *  \param icelement_a the first ic element to test
     *  \param icelement_b the second ic element to test
     *
     *  \return \c true if the two surfaces oevrlap
     *  \return \c false otherwise
     */

    bool check_intersection (ICElement *icelement_a, ICElement *icelement_b) ;



    /*! Checks if the ic element is inside the IC
     *
     *  The control is based on the real coordinates of the ic element, i.e. the
     *  coordinates read from the floorplan file.
     *
     *  \param dimensions the structure storing the dimensions of the IC
     *  \param icelement the ic element to test
     *
     *  \return \c true if \a icelement is outside of the IC
     *  \return \c false otherwise
     */

    bool check_location (Dimensions *dimensions, ICElement* icelement) ;



    /*! Aligns the ic element to the mesh of the IC
     *
     *  The function translates the coordinates of the ic element as
     *  they are in the floorplan file to coordinates in terms of rows and
     *  columns to place the ic element on the surface of the active
     *  layer. It also computes the effective dimensions.
     *
     *  \param dimensions the structure storing the dimensions of the IC
     *  \param icelement the ic element to align on the source layer
     */

    void align_to_grid (Dimensions *dimensions, ICElement* icelement) ;


    /*! Fills the source vector corresponding to an ic element
     *
     *  \param sources    pointer to the location of the source vector
     *                    that corresponds to the South-West thermal cell
     *                    of the layer where the floorplan is placed
     *  \param dimensions pointer to the structure storing the dimensions
     *  \param power      the power value consumed by the floorplan element
     *  \param surface    the surface of the floorplan element
     *  \param icelement  pointer to the ic element
     */

    void fill_sources_ic_element
    (
        Source_t        *sources,
        Dimensions      *dimensions,
        Power_t          power,
        ChipDimension_t  surface,
        ICElement       *icelement
    ) ;



    /*! Returns the maximum temperature of the ic element
     *
     *  \param icelement    pointer to the ic element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a icelement
     *                      is placed
     *
     *  \return the maximum temperature among the thermal cells on the stack
     *          layer where \a icelement is placed
     */

    Temperature_t get_max_temperature_ic_element
    (
        ICElement     *icelement,
        Dimensions    *dimensions,
        Temperature_t *temperatures
    ) ;



    /*! Returns the minimum temperature of the ic element
     *
     *  \param icelement    pointer to the ic element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a icelement
     *                      is placed
     *
     *  \return the minimum temperature among the thermal cells on the stack
     *          layer where \a icelement is placed
     */

    Temperature_t get_min_temperature_ic_element
    (
        ICElement     *icelement,
        Dimensions    *dimensions,
        Temperature_t *temperatures
    ) ;



    /*! Returns the average temperature of the ic element
     *
     *  \param icelement    pointer to the ic element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where \a icelement
     *                      is placed
     *
     *  \return the average temperature among the thermal cells on the stack
     *          layer where \a icelement is placed
     */

    Temperature_t get_avg_temperature_ic_element
    (
        ICElement     *icelement,
        Dimensions    *dimensions,
        Temperature_t *temperatures
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_IC_ELEMENT_H_ */
