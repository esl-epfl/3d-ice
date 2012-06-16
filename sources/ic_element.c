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

#include "ic_element.h"
#include "macros.h"

/******************************************************************************/

void init_ic_element (ICElement_t *this)
{
    this->SW_X      = (ChipDimension_t) 0.0 ;
    this->SW_Y      = (ChipDimension_t) 0.0 ;
    this->Length    = (ChipDimension_t) 0.0 ;
    this->Width     = (ChipDimension_t) 0.0 ;
    this->SW_Row    = (CellIndex_t) 0u ;
    this->SW_Column = (CellIndex_t) 0u ;
    this->NE_Row    = (CellIndex_t) 0u ;
    this->NE_Column = (CellIndex_t) 0u ;
    this->Next      = NULL ;
}

/******************************************************************************/

void copy_ic_element (ICElement_t *dst, ICElement_t *src)
{
    dst->SW_X      = src->SW_X ;
    dst->SW_Y      = src->SW_Y ;
    dst->Length    = src->Length ;
    dst->Width     = src->Width ;
    dst->SW_Row    = src->SW_Row ;
    dst->SW_Column = src->SW_Column ;
    dst->NE_Row    = src->NE_Row ;
    dst->NE_Column = src->NE_Column ;
    dst->Next      = src->Next ;
}

/******************************************************************************/

ICElement_t *calloc_ic_element ( void )
{
    ICElement_t *ic_element = (ICElement_t *) malloc (sizeof(ICElement_t));

    if (ic_element != NULL)

        init_ic_element (ic_element) ;

    return ic_element ;
}

/*****************************************************************************/

ICElement_t *clone_ic_element (ICElement_t *this)
{
    if (this == NULL)

        return NULL ;

    ICElement_t *ic_element = calloc_ic_element ( ) ;

    if (ic_element != NULL)

        copy_ic_element (ic_element, this) ;

    return ic_element ;
}

/*****************************************************************************/

void free_ic_element (ICElement_t *this)
{
    if (this != NULL)

        FREE_POINTER (free, this) ;
}

/******************************************************************************/

ICElement_t *clone_ic_elements_list (ICElement_t *list)
{
    if (list == NULL)

        return NULL ;

    ICElement_t *new_list = NULL ;
    ICElement_t *prev     = NULL ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, ic_el, list)
    {
        ICElement_t *tmp = clone_ic_element (ic_el) ;

        if (tmp == NULL)
        {
            free_ic_elements_list (new_list) ;

            new_list = NULL ;

            break ;
        }

        if (new_list == NULL)    new_list   = tmp ;
        else                     prev->Next = tmp ;

        prev = tmp ;
    }

    return new_list ;
}

/******************************************************************************/

void free_ic_elements_list (ICElement_t *list)
{
    FREE_LIST (ICElement_t, list, free_ic_element) ;
}

/******************************************************************************/

void print_detailed_ic_element
(
    ICElement_t *this,
    FILE        *stream,
    String_t     prefix
)
{
    fprintf (stream,
             "%sic element                  = %p\n",
             prefix, this) ;

    fprintf (stream,
             "%s    SW_X                    = %.3f\n",
             prefix, this->SW_X) ;

    fprintf (stream,
             "%s    SW_Y                    = %.3f\n",
             prefix, this->SW_Y) ;

    fprintf (stream,
             "%s    Length                  = %.3f\n",
             prefix, this->Length) ;

    fprintf (stream,
             "%s    Width                   = %.3f\n",
             prefix, this->Width) ;

    fprintf (stream,
             "%s    SW_Row                  = %d\n",
             prefix, this->SW_Row) ;

    fprintf (stream,
             "%s    SW_Column               = %d\n",
             prefix, this->SW_Column) ;

    fprintf (stream,
             "%s    NE_Row                  = %d\n",
             prefix, this->NE_Row) ;

    fprintf (stream,
             "%s    NE_Column               = %d\n",
             prefix, this->NE_Column) ;

    fprintf (stream,
             "%s    Next                    = %p\n",
             prefix, this->Next) ;
}

/******************************************************************************/

void print_detailed_ic_elements_list (ICElement_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, icelement, list)
    {
        if (icelement->Next == NULL)

            break ;

        print_detailed_ic_element (icelement, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_ic_element (icelement, stream, prefix) ;
}

/******************************************************************************/

void print_formatted_ic_element
(
    ICElement_t *this,
    FILE        *stream,
    String_t     prefix
)
{
    fprintf (stream,
        "%s\trectangle (%.1f, %.1f, %.1f, %.1f ) ;\n",
        prefix, this->SW_X, this->SW_Y,
                this->Length, this->Width) ;
}

/******************************************************************************/

void print_formatted_ic_elements_list (ICElement_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, icelement, list)
    {
        if (icelement->Next == NULL)

            break ;

        print_formatted_ic_element (icelement, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_ic_element (icelement, stream, prefix) ;
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

bool check_location (ICElement_t *this, Dimensions_t *dimensions)
{
    return (   (this->SW_X <  0)

               || (this->SW_X + this->Length > get_chip_length (dimensions))

            || (this->SW_Y <  0)

               || (this->SW_Y + this->Width > get_chip_width (dimensions)) ) ;
}

/******************************************************************************/

void align_to_grid (ICElement_t *this, Dimensions_t *dimensions)
{
    /* We "search" for these values instead of computing them directly
       since the lengths of the cells along the length of the ic might not
       be uniform
    */

    /* West side */

    CellIndex_t column_index = 0u ;

    while (get_cell_location_x (dimensions, column_index + 1) <= this->SW_X)

        column_index++ ;

    this->SW_Column = column_index-- ;

    /* East side */

    while (get_cell_location_x (dimensions, column_index + 1) < this->SW_X + this->Length)

        column_index++ ;

    this->NE_Column = column_index ;

    /* South side */

    CellIndex_t row_index = 0u ;

    while (get_cell_location_y (dimensions, row_index + 1) <= this->SW_Y)

        row_index++ ;

    this->SW_Row = row_index-- ;

    /* North side */

    while (get_cell_location_y (dimensions, row_index + 1) < this->SW_Y + this->Width)

        row_index++ ;

    this->NE_Row = row_index ;
}

/******************************************************************************/

Temperature_t get_max_temperature_ic_element
(
    ICElement_t   *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t first_row    = FIRST_IC_ELEMENT_ROW_INDEX(this) ;
    CellIndex_t first_column = FIRST_IC_ELEMENT_COLUMN_INDEX(this) ;

    Temperature_t max_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, this)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, this)
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
    ICElement_t   *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t first_row    = FIRST_IC_ELEMENT_ROW_INDEX(this) ;
    CellIndex_t first_column = FIRST_IC_ELEMENT_COLUMN_INDEX(this) ;

    Temperature_t min_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, this)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, this)
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
    ICElement_t   *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t counter = 0u ;

    Temperature_t avg_temperature = 0.0 ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, this)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, this)
        {

            avg_temperature +=

                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index)) ;

            counter++ ;

        } // FOR_EVERY_IC_ELEMENT_COLUMN
    } // FOR_EVERY_IC_ELEMENT_ROW

    return avg_temperature / (Temperature_t) counter ;
}

/******************************************************************************/
