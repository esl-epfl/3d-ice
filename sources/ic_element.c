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
    this->SW_X   = 0.0 ;
    this->SW_Y   = 0.0 ;
    this->Length = 0.0 ;
    this->Width  = 0.0 ;

    this->EffectiveLength  = 0.0 ;
    this->EffectiveWidth   = 0.0 ;

    this->SW_Row    = 0u ;
    this->SW_Column = 0u ;
    this->NE_Row    = 0u ;
    this->NE_Column = 0u ;

    this->Next = NULL ;
}

/******************************************************************************/

ICElement_t *alloc_and_init_ic_element (void)
{
    ICElement_t *ic_element = (ICElement_t *) malloc (sizeof(ICElement_t));

    if (ic_element != NULL)

        init_ic_element (ic_element) ;

    return ic_element ;
}

/*****************************************************************************/

void free_ic_element (ICElement_t *this)
{
    FREE_POINTER (free, this) ;
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
             "%s    EffectiveLength         = %.3f\n",
             prefix, this->EffectiveLength) ;

    fprintf (stream,
             "%s    EffectiveWidth          = %.3f\n",
             prefix, this->EffectiveWidth) ;

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
    ChipDimension_t cx = 0.0 ;
    ChipDimension_t cy = 0.0 ;

    CellIndex_t column_index = 0u ;
    CellIndex_t row_index    = 0u ;

    /* West side */

    cx = get_cell_length (dimensions, 0) / 2.0 ;

    while (cx < this->SW_X)
    {
        cx += get_cell_length (dimensions, column_index    ) / 2.0 ;
        cx += get_cell_length (dimensions, column_index + 1) / 2.0 ;
        column_index++ ;
    }

    this->SW_Column = column_index ;

    /* East side */

    while (cx < this->SW_X + this->Length)
    {
        cx += get_cell_length (dimensions, column_index    ) / 2.0 ;
        cx += get_cell_length (dimensions, column_index + 1) / 2.0 ;
        column_index++ ;
    }

    if (column_index > 0)

        this->NE_Column = column_index - 1 ;

    else

        this->NE_Column = column_index ;

    /* Effective length */

    FOR_EVERY_IC_ELEMENT_COLUMN (tmp_column_index, this)
    {
        this->EffectiveLength +=

            get_cell_length (dimensions, tmp_column_index) ;
    }

    /* South side */

    cy  = (get_cell_width (dimensions, 0) / 2.0) ;

    while (cy < this->SW_Y)
    {
        cy += get_cell_width (dimensions, row_index) ;  // CHECKME
        row_index++ ;
    }

    this->SW_Row = row_index ;

    /* North side */

    while (cy < this->SW_Y + this->Width)
    {
        cy += get_cell_width (dimensions, row_index) ;  // CHECKME
        row_index++ ;
    }

    if (row_index > 0) 

        this->NE_Row = row_index - 1 ;

    else

        this->NE_Row = row_index ;

    /* Effective width */

    FOR_EVERY_IC_ELEMENT_ROW (tmp_row_index, this)
    {
        this->EffectiveWidth +=

            get_cell_width (dimensions, tmp_row_index) ;  // CHECKME
    }

//    if (icelement->NE_Row - icelement->SW_Row == 0
//        && icelement->NE_Column - icelement->SW_Column == 0)
//    {
//        fprintf (stderr,  FIXME
//        "%s: no cells belong to floorplan element %s.\n",
//        floorplan->FileName, icelement->Id) ;
//    }
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
