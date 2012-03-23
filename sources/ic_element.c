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

void init_ic_element (ICElement *icelement)
{
    icelement->SW_X   = 0.0 ;
    icelement->SW_Y   = 0.0 ;
    icelement->Length = 0.0 ;
    icelement->Width  = 0.0 ;

    icelement->EffectiveLength  = 0.0 ;
    icelement->EffectiveWidth   = 0.0 ;

    icelement->SW_Row    = 0u ;
    icelement->SW_Column = 0u ;
    icelement->NE_Row    = 0u ;
    icelement->NE_Column = 0u ;

    icelement->Next = NULL ;
}

/******************************************************************************/

ICElement *alloc_and_init_ic_element (void)
{
    ICElement *ic_element = (ICElement *) malloc (sizeof(ICElement));

    if (ic_element != NULL)

        init_ic_element (ic_element) ;

    return ic_element ;
}

/*****************************************************************************/

void free_ic_element (ICElement *icelement)
{
    FREE_POINTER (free, icelement) ;
}

/******************************************************************************/

void free_ic_elements_list (ICElement *list)
{
    FREE_LIST (ICElement, list, free_ic_element) ;
}

/******************************************************************************/

void print_detailed_ic_element
(
    FILE      *stream,
    String_t   prefix,
    ICElement *icelement
)
{
    fprintf (stream,
             "%sic element                  = %p\n",
             prefix, icelement) ;

    fprintf (stream,
             "%s    SW_X                    = %.3f\n",
             prefix, icelement->SW_X) ;

    fprintf (stream,
             "%s    SW_Y                    = %.3f\n",
             prefix, icelement->SW_Y) ;

    fprintf (stream,
             "%s    Length                  = %.3f\n",
             prefix, icelement->Length) ;

    fprintf (stream,
             "%s    Width                   = %.3f\n",
             prefix, icelement->Width) ;

    fprintf (stream,
             "%s    EffectiveLength         = %.3f\n",
             prefix, icelement->EffectiveLength) ;

    fprintf (stream,
             "%s    EffectiveWidth          = %.3f\n",
             prefix, icelement->EffectiveWidth) ;

    fprintf (stream,
             "%s    SW_Row                  = %d\n",
             prefix, icelement->SW_Row) ;

    fprintf (stream,
             "%s    SW_Column               = %d\n",
             prefix, icelement->SW_Column) ;

    fprintf (stream,
             "%s    NE_Row                  = %d\n",
             prefix, icelement->NE_Row) ;

    fprintf (stream,
             "%s    NE_Column               = %d\n",
             prefix, icelement->NE_Column) ;

    fprintf (stream,
             "%s    Next                    = %p\n",
             prefix, icelement->Next) ;
}

/******************************************************************************/

void print_detailed_ic_elements_list (FILE *stream, String_t prefix, ICElement *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement, icelement, list)
    {
        if (icelement->Next == NULL)

            break ;

        print_detailed_ic_element (stream, prefix, icelement) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_ic_element (stream, prefix, icelement) ;
}

/******************************************************************************/

void print_formatted_ic_element
(
    FILE      *stream,
    String_t   prefix,
    ICElement *icelement
)
{
    fprintf (stream,
        "%s\trectangle (%.1f, %.1f, %.1f, %.1f ) ;\n",
        prefix, icelement->SW_X, icelement->SW_Y,
                icelement->Length, icelement->Width) ;
}

/******************************************************************************/

void print_formatted_ic_elements_list (FILE *stream, String_t prefix, ICElement *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement, icelement, list)
    {
        if (icelement->Next == NULL)

            break ;

        print_formatted_ic_element (stream, prefix, icelement) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_ic_element (stream, prefix, icelement) ;
}

/******************************************************************************/

bool check_intersection
(
    ICElement *icelement_a,
    ICElement *icelement_b
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

bool check_location (Dimensions *dimensions, ICElement *icelement)
{
    return (   (icelement->SW_X <  0)

               || (icelement->SW_X + icelement->Length > get_chip_length (dimensions))

            || (icelement->SW_Y <  0)

               || (icelement->SW_Y + icelement->Width > get_chip_width (dimensions)) ) ;
}

/******************************************************************************/

void align_to_grid (Dimensions *dimensions, ICElement *icelement)
{
    ChipDimension_t cx = 0.0 ;
    ChipDimension_t cy = 0.0 ;

    CellIndex_t column_index = 0u ;
    CellIndex_t row_index    = 0u ;

    /* West side */

    cx = get_cell_length (dimensions, 0) / 2.0 ;

    while (cx < icelement->SW_X)
    {
        cx += get_cell_length (dimensions, column_index    ) / 2.0 ;
        cx += get_cell_length (dimensions, column_index + 1) / 2.0 ;
        column_index++ ;
    }

    icelement->SW_Column = column_index ;

    /* East side */

    while (cx < icelement->SW_X + icelement->Length)
    {
        cx += get_cell_length (dimensions, column_index    ) / 2.0 ;
        cx += get_cell_length (dimensions, column_index + 1) / 2.0 ;
        column_index++ ;
    }

    icelement->NE_Column = column_index - 1 ;

    /* Effective length */

    FOR_EVERY_IC_ELEMENT_COLUMN (tmp_column_index, icelement)
    {
        icelement->EffectiveLength +=

            get_cell_length (dimensions, tmp_column_index) ;
    }

    /* South side */

    cy  = (get_cell_width (dimensions, 0) / 2.0) ;

    while (cy < icelement->SW_Y)
    {
        cy += get_cell_width (dimensions, row_index) ;  // CHECKME
        row_index++ ;
    }

    icelement->SW_Row = row_index ;

    /* North side */

    while (cy < icelement->SW_Y + icelement->Width)
    {
        cy += get_cell_width (dimensions, row_index) ;  // CHECKME
        row_index++ ;
    }

    icelement->NE_Row = row_index - 1 ;

    /* Effective width */

    FOR_EVERY_IC_ELEMENT_ROW (tmp_row_index, icelement)
    {
        icelement->EffectiveWidth +=

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

void fill_sources_ic_element
(
    Source_t        *sources,
    Dimensions      *dimensions,
    Power_t          power,
    ChipDimension_t  surface,
    ICElement       *icelement
)
{
    // Here we ADD the power value to the source vector. It works as long as
    // the source vector is set to zero every time. This way the vale is added
    // in case this is the top most layer and the heatsink is used

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icelement)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icelement)
        {
            sources [get_cell_offset_in_layer (dimensions, row_index, column_index)]

            += (power * get_cell_length (dimensions, column_index)
                      * get_cell_width (dimensions, row_index)
               )
               /  surface ;

#ifdef PRINT_SOURCES
            fprintf (stderr,
                "solid  cell  | r %4d c %4d | l %6.1f w %6.1f " \
                            " | %.5e [source] += ( %.4e [W] * l * w) / %4.1f\n",
                row_index, column_index,
                get_cell_length (dimensions, column_index), get_cell_width (dimensions, row_index),
                sources [get_cell_offset_in_layer (dimensions, row_index, column_index)],
                power, surface) ;
#endif

        } // FOR_EVERY_IC_ELEMENT_COLUMN
    } // FOR_EVERY_IC_ELEMENT_ROW

    return ;
}

/******************************************************************************/

Temperature_t get_max_temperature_ic_element
(
    ICElement     *icelement,
    Dimensions    *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t first_row    = FIRST_IC_ELEMENT_ROW_INDEX(icelement) ;
    CellIndex_t first_column = FIRST_IC_ELEMENT_COLUMN_INDEX(icelement) ;

    Temperature_t max_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icelement)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icelement)
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
    ICElement     *icelement,
    Dimensions    *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t first_row    = FIRST_IC_ELEMENT_ROW_INDEX(icelement) ;
    CellIndex_t first_column = FIRST_IC_ELEMENT_COLUMN_INDEX(icelement) ;

    Temperature_t min_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icelement)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icelement)
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
    ICElement     *icelement,
    Dimensions    *dimensions,
    Temperature_t *temperatures
)
{
    CellIndex_t counter = 0u ;

    Temperature_t avg_temperature = 0.0 ;

    FOR_EVERY_IC_ELEMENT_ROW (row_index, icelement)
    {
        FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icelement)
        {

            avg_temperature +=

                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index)) ;

            counter++ ;

        } // FOR_EVERY_IC_ELEMENT_COLUMN
    } // FOR_EVERY_IC_ELEMENT_ROW

    return avg_temperature / (Temperature_t) counter ;
}

/******************************************************************************/
