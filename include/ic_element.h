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

    /*! \struct ICElement_t
     *
     *  A rectangle that represent the surface occupied by a flooprlan element
     */

    struct ICElement_t
    {
        /*! The south-west X coordinate, in \f$ \mu m \f$ */

        ChipDimension_t SW_X ;

        /*! The south-west Y coordinate, in \f$ \mu m \f$ */

        ChipDimension_t SW_Y ;

        /*! The length (west <-> east) of the ic element, in \f$ \mu m \f$ */

        ChipDimension_t Length ;

        /*! The width (south <-> north) of the ic element, in \f$ \mu m \f$ */

        ChipDimension_t Width ;

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
    } ;

    /*! Definition of the type ICElement_t */

    typedef struct ICElement_t ICElement_t ;

/******************************************************************************/

    void ic_element_init (ICElement_t *icel) ;

    void ic_element_copy (ICElement_t *dst, ICElement_t *src) ;

    void ic_element_destroy (ICElement_t *icel) ;

    ICElement_t *ic_element_calloc ( void ) ;

    ICElement_t *ic_element_clone (ICElement_t *icel) ;

    void ic_element_free (ICElement_t *icel) ;

    bool ic_element_equal (ICElement_t *icel, ICElement_t *other) ;

    void ic_element_print (ICElement_t *icel, FILE *stream, String_t prefix) ;



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

    bool check_intersection (ICElement_t *icelement_a, ICElement_t *icelement_b) ;



    /*! Checks if the ic element is inside the IC
     *
     *  The control is based on the real coordinates of the ic element, i.e. the
     *  coordinates read from the floorplan file.
     *
     *  \param icel       the ic element to test
     *  \param dimensions the structure storing the dimensions of the IC
     *
     *  \return \c true if \a icel is outside of the IC
     *  \return \c false otherwise
     */

    bool check_location (ICElement_t* icel, Dimensions_t *dimensions) ;



    /*! Aligns the ic element to the mesh of the IC
     *
     *  The function translates the coordinates of the ic element as
     *  they are in the floorplan file to coordinates in terms of rows and
     *  columns to place the ic element on the surface of the active
     *  layer. It also computes the effective dimensions.
     *
     *  \param icel       the ic element to align on the source layer
     *  \param dimensions the structure storing the dimensions of the IC
     */

    void align_to_grid (ICElement_t* icel, Dimensions_t *dimensions) ;



    /*! Returns the maximum temperature of the ic element
     *
     *  \param icel         pointer to the ic element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where the IC element is placed
     *
     *  \return the maximum temperature among the thermal cells on the stack
     *          layer where \a icel is placed
     */

    Temperature_t get_max_temperature_ic_element
    (
        ICElement_t   *icel,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures
    ) ;



    /*! Returns the minimum temperature of the ic element
     *
     *  \param icel         pointer to the ic element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where the IC element is placed
     *
     *  \return the minimum temperature among the thermal cells on the stack
     *          layer where \a icel is placed
     */

    Temperature_t get_min_temperature_ic_element
    (
        ICElement_t   *icel,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures
    ) ;



    /*! Returns the average temperature of the ic element
     *
     *  \param icel         pointer to the ic element
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param temperatures pointer to the temperature of the first thermal
     *                      cell in the layer where the IC element is placed
     *
     *  \return the average temperature among the thermal cells on the stack
     *          layer where \a icel is placed
     */

    Temperature_t get_avg_temperature_ic_element
    (
        ICElement_t   *icel,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_IC_ELEMENT_H_ */
