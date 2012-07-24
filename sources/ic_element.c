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

#include <stdlib.h>
#include <string.h>

#include "ic_element.h"
#include "macros.h"

/******************************************************************************/

void ic_element_init (ICElement_t *icel)
{
    icel->SW_X      = (ChipDimension_t) 0.0 ;
    icel->SW_Y      = (ChipDimension_t) 0.0 ;
    icel->Length    = (ChipDimension_t) 0.0 ;
    icel->Width     = (ChipDimension_t) 0.0 ;
    icel->SW_Row    = (CellIndex_t) 0u ;
    icel->SW_Column = (CellIndex_t) 0u ;
    icel->NE_Row    = (CellIndex_t) 0u ;
    icel->NE_Column = (CellIndex_t) 0u ;
}

/******************************************************************************/

void ic_element_copy (ICElement_t *dst, ICElement_t *src)
{
    ic_element_destroy (dst) ;

    dst->SW_X      = src->SW_X ;
    dst->SW_Y      = src->SW_Y ;
    dst->Length    = src->Length ;
    dst->Width     = src->Width ;
    dst->SW_Row    = src->SW_Row ;
    dst->SW_Column = src->SW_Column ;
    dst->NE_Row    = src->NE_Row ;
    dst->NE_Column = src->NE_Column ;
}

/******************************************************************************/

void ic_element_destroy (ICElement_t *icel)
{
    // Nothing to do ...

    ic_element_init (icel) ;
}

/******************************************************************************/

ICElement_t *ic_element_calloc ( void )
{
    ICElement_t *icel = (ICElement_t *) malloc (sizeof(ICElement_t));

    if (icel != NULL)

        ic_element_init (icel) ;

    return icel ;
}

/*****************************************************************************/

ICElement_t *ic_element_clone (ICElement_t *icel)
{
    if (icel == NULL)

        return NULL ;

    ICElement_t *newi = ic_element_calloc ( ) ;

    if (newi != NULL)

        ic_element_copy (newi, icel) ;

    return newi ;
}

/*****************************************************************************/

void ic_element_free (ICElement_t *icel)
{
    if (icel == NULL)

        return ;

    ic_element_destroy (icel) ;

    free (icel) ;
}

/******************************************************************************/

bool ic_element_equal (ICElement_t *icel, ICElement_t *other)
{
    return    icel->SW_X   == other->SW_X
           && icel->SW_Y   == other->SW_Y
           && icel->Length == other->Length
           && icel->Width  == other->Width ;
}

/******************************************************************************/

void ic_element_print
(
    ICElement_t *icel,
    FILE        *stream,
    String_t     prefix
)
{
    fprintf (stream,
        "%s\trectangle (%.1f, %.1f, %.1f, %.1f ) ;\n",
        prefix, icel->SW_X, icel->SW_Y,
                icel->Length, icel->Width) ;
}

/******************************************************************************/

bool check_intersection
(
    ICElement_t *icelement_a,
    ICElement_t *icelement_b
)
{
    if (icelement_a == icelement_b)

        return false ;

    if (   (icelement_a->SW_X + icelement_a->Length)
           <= icelement_b->SW_X
        || icelement_a->SW_X
           >= (icelement_b->SW_X + icelement_b->Length))

        return false ;

    if (   (icelement_a->SW_Y + icelement_a->Width)
           <= icelement_b->SW_Y
        || icelement_a->SW_Y
           >= (icelement_b->SW_Y + icelement_b->Width))

        return false ;

    return true ;
}

/******************************************************************************/

bool check_location (ICElement_t *icel, Dimensions_t *dimensions)
{
    return (   (icel->SW_X <  0)

               || (icel->SW_X + icel->Length > get_chip_length (dimensions))

            || (icel->SW_Y <  0)

               || (icel->SW_Y + icel->Width > get_chip_width (dimensions)) ) ;
}

/******************************************************************************/

void align_to_grid (ICElement_t *icel, Dimensions_t *dimensions)
{
    /* We "search" for these values instead of computing them directly
       since the lengths of the cells along the length of the ic might not
       be uniform
    */

    /* West side */

    CellIndex_t column_index = 0u ;

    while (get_cell_location_x (dimensions, column_index + 1) <= icel->SW_X)

        column_index++ ;

    icel->SW_Column = column_index-- ;

    /* East side */

    while (get_cell_location_x (dimensions, column_index + 1) < icel->SW_X + icel->Length)

        column_index++ ;

    icel->NE_Column = column_index ;

    /* South side */

    CellIndex_t row_index = 0u ;

    while (get_cell_location_y (dimensions, row_index + 1) <= icel->SW_Y)

        row_index++ ;

    icel->SW_Row = row_index-- ;

    /* North side */

    while (get_cell_location_y (dimensions, row_index + 1) < icel->SW_Y + icel->Width)

        row_index++ ;

    icel->NE_Row = row_index ;
}

/******************************************************************************/

Temperature_t get_max_temperature_ic_element
(
    ICElement_t   *icel,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t first_row    = FIRST_IC_ELEMENT_ROW_INDEX(icel) ;
    CellIndex_t first_column = FIRST_IC_ELEMENT_COLUMN_INDEX(icel) ;

    Temperature_t max_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icel)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icel)
        {

            max_temperature = MAX
            (
                max_temperature,
                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index))
            ) ;

        } // FOR_EVERY_IC_ELEMENT_COLUMN
    } // FOR_EVERY_IC_ELEMENT_ROW

    return max_temperature ;
}

/******************************************************************************/

Temperature_t get_min_temperature_ic_element
(
    ICElement_t   *icel,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t first_row    = FIRST_IC_ELEMENT_ROW_INDEX(icel) ;
    CellIndex_t first_column = FIRST_IC_ELEMENT_COLUMN_INDEX(icel) ;

    Temperature_t min_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icel)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icel)
        {

            min_temperature = MIN
            (
                min_temperature,
                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index))
            ) ;

        } // FOR_EVERY_IC_ELEMENT_COLUMN
    } // FOR_EVERY_IC_ELEMENT_ROW

    return min_temperature ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_ic_element
(
    ICElement_t   *icel,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t counter = 0u ;

    Temperature_t avg_temperature = 0.0 ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icel)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icel)
        {

            avg_temperature +=

                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index)) ;

            counter++ ;

        } // FOR_EVERY_IC_ELEMENT_COLUMN
    } // FOR_EVERY_IC_ELEMENT_ROW

    return avg_temperature / (Temperature_t) counter ;
}

/******************************************************************************/
