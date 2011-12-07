/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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

#include <string.h>
#include <stdlib.h>

#include "floorplan_element.h"
#include "macros.h"

/******************************************************************************/

void init_floorplan_element (FloorplanElement *floorplan_element)
{
    floorplan_element->Id              = NULL ;

    floorplan_element->SW_X            = 0.0 ;
    floorplan_element->SW_Y            = 0.0 ;

    floorplan_element->Length          = 0.0 ;
    floorplan_element->Width           = 0.0 ;

    floorplan_element->EffectiveLength  = 0.0 ;
    floorplan_element->EffectiveWidth   = 0.0 ;
    floorplan_element->EffectiveSurface = 0.0 ;

    floorplan_element->SW_Row          = 0u ;
    floorplan_element->SW_Column       = 0u ;
    floorplan_element->NE_Row          = 0u ;
    floorplan_element->NE_Column       = 0u ;

    floorplan_element->PowerValues     = NULL ;

    floorplan_element->Next            = NULL ;
}

/******************************************************************************/

FloorplanElement *alloc_and_init_floorplan_element (void)
{
    FloorplanElement *floorplan_element = (FloorplanElement *)

        malloc (sizeof(FloorplanElement));

    if (floorplan_element != NULL)

        init_floorplan_element(floorplan_element) ;

    return floorplan_element ;
}

/*****************************************************************************/

void free_floorplan_element (FloorplanElement *floorplan_element)
{
    FREE_POINTER (free_powers_queue, floorplan_element->PowerValues) ;
    FREE_POINTER (free,              floorplan_element->Id) ;
    FREE_POINTER (free,              floorplan_element) ;
}

/******************************************************************************/

void free_floorplan_elements_list (FloorplanElement *list)
{
    FREE_LIST (FloorplanElement, list, free_floorplan_element) ;
}

/******************************************************************************/

void print_detailed_floorplan_element
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *floorplan_element
)
{
    fprintf (stream,
        "%sFloorplan Element %s ( %.1f , %.1f ) %.1f x %.1f \n",
        prefix, floorplan_element->Id,
        floorplan_element->SW_X, floorplan_element->SW_Y,
        floorplan_element->Length, floorplan_element->Width) ;

    fprintf (stream,
        "%s  Rows (%d - %d) Columns (%d - %d)",
        prefix, floorplan_element->SW_Row, floorplan_element->NE_Row,
        floorplan_element->SW_Column, floorplan_element->NE_Column) ;

    fprintf (stream,
        "%s  Power values ",
        prefix) ;

    print_detailed_powers_queue (stream, prefix, floorplan_element->PowerValues) ;
}

/******************************************************************************/

void print_detailed_floorplan_elements_list
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)

        print_detailed_floorplan_element (stream, prefix, flp_el) ;
}

/******************************************************************************/

void print_formatted_floorplan_element
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *floorplan_element
)
{
    fprintf (stream,
        "%s%s:\n",
        prefix, floorplan_element->Id) ;

    fprintf (stream,
        "%s\tposition %.1f, %.1f ;\n",
        prefix, floorplan_element->SW_X, floorplan_element->SW_Y) ;

    fprintf (stream,
        "%s\tdimension %.1f, %.1f ;\n",
        prefix, floorplan_element->Length, floorplan_element->Width) ;

    fprintf (stream,
        "%s   power values ",
        prefix) ;

    print_formatted_powers_queue (stream, floorplan_element->PowerValues) ;

    fprintf (stream, "\n") ;
}

/******************************************************************************/

void print_formatted_floorplan_elements_list
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)
    {
        print_formatted_floorplan_element (stream, prefix, flp_el) ;
        fprintf(stream, "\n") ;
    }
}

/******************************************************************************/

Error_t fill_sources_floorplan_element
(
    Source_t         *sources,
    Dimensions       *dimensions,
    FloorplanElement *floorplan_element
)
{
    if (is_empty_powers_queue (floorplan_element->PowerValues) == true)

        return TDICE_FAILURE ;

    Power_t power = get_from_powers_queue (floorplan_element->PowerValues);

    ChipDimension_t flp_el_surface = floorplan_element->EffectiveSurface ;

    // Here we ADD the power value to the source vector. It works as long as
    // the source vector is set to zero every time. This way the vale is added
    // in case this is the top most layer and the heatsink is used

    FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
    {
        FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
        {
            sources [get_cell_offset_in_layer (dimensions, row_index, column_index)]

            += (power * get_cell_length (dimensions, column_index)
                      * get_cell_width (dimensions, row_index)
               )
               /  flp_el_surface ;

#ifdef PRINT_SOURCES
            fprintf (stderr,
                "solid  cell  | r %4d c %4d | l %6.1f w %6.1f " \
                            " | %.5e [source] += ( %.4e [W] * l * w) / %4.1f | %s\n",
                row_index, column_index,
                get_cell_length (dimensions, column_index), get_cell_width (dimensions, row_index),
                sources [get_cell_offset_in_layer (dimensions, row_index, column_index)],
                power, flp_el_surface, floorplan_element->Id) ;
#endif

        } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
    } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

    pop_from_powers_queue (floorplan_element->PowerValues) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t insert_power_values_floorplan_element
(
    FloorplanElement *floorplan_element,
    PowersQueue      *pvalues
)
{
    if (is_empty_powers_queue(pvalues) == true)

        return TDICE_FAILURE ;

    Power_t power = get_from_powers_queue (pvalues) ;

    put_into_powers_queue (floorplan_element->PowerValues, power);

    pop_from_powers_queue (pvalues) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

FloorplanElement *find_floorplan_element_in_list
(
    FloorplanElement *list,
    String_t          id
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)
    {
        if (strcmp (flp_el->Id, id) == 0) break ;
    }
    return flp_el ;
}

/******************************************************************************/

bool check_intersection
(
    FloorplanElement *floorplan_element_a,
    FloorplanElement *floorplan_element_b
)
{
    if (   (floorplan_element_a->SW_X + floorplan_element_a->Length)
           <= floorplan_element_b->SW_X
        || floorplan_element_a->SW_X
           >= (floorplan_element_b->SW_X + floorplan_element_b->Length))

        return false ;

    if (   (floorplan_element_a->SW_Y + floorplan_element_a->Width)
           <= floorplan_element_b->SW_Y
        || floorplan_element_a->SW_Y
           >= (floorplan_element_b->SW_Y + floorplan_element_b->Width))

        return false ;

    return true ;
}

/******************************************************************************/

FloorplanElement *find_intersection_in_list
(
    FloorplanElement *list,
    FloorplanElement *floorplan_element
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)

        if (check_intersection(flp_el, floorplan_element) == true)

            return flp_el ;

    return NULL ;
}
/******************************************************************************/

bool check_location
(
    Dimensions       *dimensions,
    FloorplanElement *floorplan_element
)
{
    return (   (floorplan_element->SW_X <  0)

               || (floorplan_element->SW_X + floorplan_element->Length
                  > get_chip_length (dimensions))

            || (floorplan_element->SW_Y <  0)

               || (floorplan_element->SW_Y + floorplan_element->Width
                  > get_chip_width (dimensions)) ) ;
}

/******************************************************************************/

void align_to_grid
(
    Dimensions       *dimensions,
    FloorplanElement *floorplan_element
)
{
    ChipDimension_t cx = 0.0 ;
    ChipDimension_t cy = 0.0 ;
    CellIndex_t column_index = 0u ;
    CellIndex_t row_index    = 0u ;

    /* West side */

    cx = get_cell_length (dimensions, 0) / 2.0 ;

    while (cx < floorplan_element->SW_X)
    {
        cx += get_cell_length (dimensions, column_index    ) / 2.0 ;
        cx += get_cell_length (dimensions, column_index + 1) / 2.0 ;
        column_index++ ;
    }

    floorplan_element->SW_Column = column_index ;

    /* East side */

    while (cx < floorplan_element->SW_X + floorplan_element->Length)
    {
        cx += get_cell_length (dimensions, column_index    ) / 2.0 ;
        cx += get_cell_length (dimensions, column_index + 1) / 2.0 ;
        column_index++ ;
    }

    floorplan_element->NE_Column = column_index - 1 ;

    /* Effective length */

    FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (tmp_column_index, floorplan_element)
    {
        floorplan_element->EffectiveLength +=

            get_cell_length (dimensions, tmp_column_index) ;
    }

    /* South side */

    cy  = (get_cell_width (dimensions, 0) / 2.0) ;

    while (cy < floorplan_element->SW_Y)
    {
        cy += get_cell_width (dimensions, row_index) ;  // CHECKME
        row_index++ ;
    }

    floorplan_element->SW_Row = row_index ;

    /* North side */

    while (cy < floorplan_element->SW_Y + floorplan_element->Width)
    {
        cy += get_cell_width (dimensions, row_index) ;  // CHECKME
        row_index++ ;
    }

    floorplan_element->NE_Row = row_index - 1 ;

    /* Effective width */

    FOR_EVERY_FLOORPLAN_ELEMENT_ROW (tmp_row_index, floorplan_element)
    {
        floorplan_element->EffectiveWidth +=

            get_cell_width (dimensions, tmp_row_index) ;  // CHECKME
    }

    floorplan_element->EffectiveSurface =

        floorplan_element->EffectiveLength * floorplan_element->EffectiveWidth ;

//    if (floorplan_element->NE_Row - floorplan_element->SW_Row == 0
//        && floorplan_element->NE_Column - floorplan_element->SW_Column == 0)
//    {
//        fprintf (stderr,  FIXME
//        "%s: no cells belong to floorplan element %s.\n",
//        floorplan->FileName, floorplan_element->Id) ;
//    }
}

/******************************************************************************/

Temperature_t get_max_temperature_floorplan_element
(
    FloorplanElement *floorplan_element,
    Dimensions       *dimensions,
    Temperature_t    *temperatures
)
{
    CellIndex_t first_row    = FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(floorplan_element) ;
    CellIndex_t first_column = FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(floorplan_element) ;

    Temperature_t max_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
    {
        FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
        {

            max_temperature = MAX
            (
                max_temperature,
                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index))
            ) ;

        } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
    } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

    return max_temperature ;
}

/******************************************************************************/

Temperature_t get_min_temperature_floorplan_element
(
    FloorplanElement *floorplan_element,
    Dimensions       *dimensions,
    Temperature_t    *temperatures
)
{
    CellIndex_t first_row    = FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(floorplan_element) ;
    CellIndex_t first_column = FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(floorplan_element) ;

    Temperature_t min_temperature =

        *(temperatures + get_cell_offset_in_layer(dimensions, first_row, first_column)) ;

    FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
    {
        FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
        {

            min_temperature = MIN
            (
                min_temperature,
                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index))
            ) ;

        } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
    } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

    return min_temperature ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_floorplan_element
(
    FloorplanElement *floorplan_element,
    Dimensions       *dimensions,
    Temperature_t    *temperatures
)
{
    CellIndex_t counter = 0u ;

    Temperature_t avg_temperature = 0.0 ;

    FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, floorplan_element)
    {
        FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, floorplan_element)
        {

            avg_temperature +=

                *(temperatures + get_cell_offset_in_layer (dimensions, row_index, column_index)) ;

            counter++ ;

        } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
    } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

    return avg_temperature / (Temperature_t) counter ;
}

/******************************************************************************/
