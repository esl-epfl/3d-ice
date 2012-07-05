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

#include <stdlib.h>
#include <string.h>

#include "inspection_point.h"
#include "macros.h"

/******************************************************************************/

void init_tcell (Tcell_t *tcell)
{
    tcell->Xval        = 0.0 ;
    tcell->ActualXval  = 0.0 ;
    tcell->Yval        = 0.0 ;
    tcell->ActualYval  = 0.0 ;
    tcell->RowIndex    = 0u ;
    tcell->ColumnIndex = 0u ;
}

/******************************************************************************/

Tcell_t *calloc_tcell (void)
{
    Tcell_t *tcell = (Tcell_t *) malloc (sizeof(Tcell_t)) ;

    if (tcell != NULL)

        init_tcell (tcell) ;

    return tcell ;
}

/******************************************************************************/

void free_tcell (Tcell_t *tcell)
{
    if (tcell != NULL)

        FREE_POINTER (free, tcell) ;
}

/******************************************************************************/

void align_tcell
(
    Tcell_t         *tcell,
    ChipDimension_t  xval,
    ChipDimension_t  yval,
    Dimensions_t    *dimensions
)
{
    FOR_EVERY_ROW (row_index, dimensions)
    {
        tcell->RowIndex   = row_index ;

        if (   yval >= get_cell_location_y (dimensions, row_index)
            && yval <  get_cell_location_y (dimensions, row_index + 1))
        {
            tcell->ActualYval = get_cell_location_y (dimensions, row_index) ;
            break ;
        }
    }

    tcell->Yval = yval ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        tcell->ColumnIndex = column_index ;

        if (   xval >= get_cell_location_x (dimensions, column_index)
            && xval <  get_cell_location_x (dimensions, column_index + 1))
        {
            tcell->ActualXval = get_cell_location_x (dimensions, column_index) ;
            break ;
        }
    }

    tcell->Xval = xval ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tflp (Tflp_t *tflp)
{
    tflp->Quantity = TDICE_OUTPUT_QUANTITY_NONE ;
}

/******************************************************************************/

Tflp_t *calloc_tflp (void)
{
    Tflp_t *tflp = (Tflp_t *) malloc (sizeof(Tflp_t)) ;

    if (tflp != NULL)

        init_tflp (tflp) ;

    return tflp ;
}

/******************************************************************************/

void free_tflp (Tflp_t *tflp)
{
    if (tflp != NULL)

        FREE_POINTER(free, tflp) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tflpel (Tflpel_t *tflpel)
{
    tflpel->FloorplanElement = NULL ;
    tflpel->Quantity         = TDICE_OUTPUT_QUANTITY_NONE ;
}

/******************************************************************************/

Tflpel_t *calloc_tflpel (void)
{
    Tflpel_t *tflpel = (Tflpel_t *) malloc (sizeof(Tflpel_t)) ;

    if (tflpel != NULL)

        init_tflpel(tflpel) ;

    return tflpel ;
}

/******************************************************************************/

void free_tflpel (Tflpel_t *tflpel)
{
    if (tflpel != NULL)

        FREE_POINTER(free, tflpel) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tcoolant (Tcoolant_t *tcoolant)
{
    tcoolant->Quantity = TDICE_OUTPUT_QUANTITY_NONE ;
}

/******************************************************************************/

Tcoolant_t *calloc_tcoolant (void)
{
    Tcoolant_t *tcoolant = (Tcoolant_t *) malloc (sizeof(Tcoolant_t)) ;

    if (tcoolant != NULL)

        init_tcoolant (tcoolant) ;

    return tcoolant ;
}

/******************************************************************************/

void free_tcoolant (Tcoolant_t *tcoolant)
{
    if (tcoolant != NULL)

        FREE_POINTER (free, tcoolant) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_inspection_point (InspectionPoint_t *ipoint)
{
    ipoint->Type             = TDICE_OUTPUT_TYPE_NONE ;
    ipoint->Instant          = TDICE_OUTPUT_INSTANT_NONE ;
    ipoint->FileName         = NULL ;
    ipoint->Pointer.Tcell    = NULL ;
    ipoint->Pointer.Tflp     = NULL ;
    ipoint->Pointer.Tflpel   = NULL ;
    ipoint->Pointer.Tcoolant = NULL ;
    ipoint->StackElement     = NULL ;
    ipoint->Next             = NULL ;
}

/******************************************************************************/

InspectionPoint_t *calloc_inspection_point (void)
{
    InspectionPoint_t *inspection_point = (InspectionPoint_t *) malloc (sizeof(InspectionPoint_t)) ;

    if (inspection_point != NULL)

        init_inspection_point (inspection_point) ;

    return inspection_point ;
}

/******************************************************************************/

void free_inspection_point (InspectionPoint_t *ipoint)
{
    if (   ipoint->Type == TDICE_OUTPUT_TYPE_TCELL
        && ipoint->Pointer.Tcell != NULL)

        FREE_POINTER (free_tcell, ipoint->Pointer.Tcell) ;

    else if (   ipoint->Type == TDICE_OUTPUT_TYPE_TFLP
             && ipoint->Pointer.Tflp != NULL)

        FREE_POINTER (free_tflp, ipoint->Pointer.Tflp) ;

    else if (   ipoint->Type == TDICE_OUTPUT_TYPE_TFLPEL
             && ipoint->Pointer.Tflpel != NULL)

        FREE_POINTER (free_tflpel, ipoint->Pointer.Tflpel) ;

    else if (   ipoint->Type == TDICE_OUTPUT_TYPE_TCOOLANT
             && ipoint->Pointer.Tcoolant != NULL)

        FREE_POINTER (free_tcoolant, ipoint->Pointer.Tcoolant) ;

    ipoint->StackElement = NULL ;

    FREE_POINTER (free, ipoint->FileName) ;
    FREE_POINTER (free, ipoint) ;
}

/******************************************************************************/

void free_inspection_point_list (InspectionPoint_t *list)
{
    FREE_LIST (InspectionPoint_t, list, free_inspection_point) ;
}

/******************************************************************************/

void print_inspection_point_list
(
    InspectionPoint_t *list,
    FILE              *stream,
    String_t           prefix
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, inspection_point, list)
    {
        switch (inspection_point->Type)
        {
            case TDICE_OUTPUT_TYPE_TCELL :

                fprintf (stream, "%sT        (%s, %.1f, %.1f, \"%s\", ",
                    prefix, inspection_point->StackElement->Id,
                    inspection_point->Pointer.Tcell->Xval,
                    inspection_point->Pointer.Tcell->Yval,
                    inspection_point->FileName) ;

                break ;

            case TDICE_OUTPUT_TYPE_TFLP :

                fprintf (stream, "%sTflp     (%s, \"%s\", ",
                    prefix, inspection_point->StackElement->Id,
                    inspection_point->FileName) ;

                if (inspection_point->Pointer.Tflp->Quantity==TDICE_OUTPUT_QUANTITY_MAXIMUM)

                    fprintf(stream, "maximum, ");

                else if (inspection_point->Pointer.Tflp->Quantity==TDICE_OUTPUT_QUANTITY_MINIMUM)

                    fprintf(stream, "minimum, ");

                else

                    fprintf(stream, "average, ");

                break ;

            case TDICE_OUTPUT_TYPE_TFLPEL :

                fprintf (stream, "%sTflpel   (%s.%s, \"%s\", ",
                    prefix, inspection_point->StackElement->Id,
                    inspection_point->Pointer.Tflpel->FloorplanElement->Id,
                    inspection_point->FileName) ;

                if (inspection_point->Pointer.Tflpel->Quantity==TDICE_OUTPUT_QUANTITY_MAXIMUM)

                    fprintf(stream, "maximum, ");

                else if (inspection_point->Pointer.Tflpel->Quantity==TDICE_OUTPUT_QUANTITY_MINIMUM)

                    fprintf(stream, "minimum, ");

                else

                    fprintf(stream, "average, ");

                break ;

            case TDICE_OUTPUT_TYPE_TMAP :

                fprintf (stream, "%sTmap   (%s, \"%s\", ",
                    prefix, inspection_point->StackElement->Id,
                    inspection_point->FileName) ;

                break ;

            case TDICE_OUTPUT_TYPE_PMAP :

                fprintf (stream, "%sPmap   (%s, \"%s\", ",
                    prefix, inspection_point->StackElement->Id,
                    inspection_point->FileName) ;

                break ;

            case TDICE_OUTPUT_TYPE_TCOOLANT :

                fprintf (stream, "%sTcoolant (%s, \"%s\", ",
                    prefix, inspection_point->StackElement->Id,
                    inspection_point->FileName) ;

                if (inspection_point->Pointer.Tcoolant->Quantity==TDICE_OUTPUT_QUANTITY_MAXIMUM)

                    fprintf(stream, "maximum, ");

                else if (inspection_point->Pointer.Tcoolant->Quantity==TDICE_OUTPUT_QUANTITY_MINIMUM)

                    fprintf(stream, "minimum, ");

                else

                    fprintf(stream, "average, ");

                break ;

            default :

                fprintf (stderr, "Undefined inspection point command type %d\n", inspection_point->Type) ;
                break ;
        }

        if (inspection_point->Instant == TDICE_OUTPUT_INSTANT_SLOT)

            fprintf(stream, "slot );\n");

        else if (inspection_point->Instant == TDICE_OUTPUT_INSTANT_STEP)

            fprintf(stream, "step );\n");

        else

            fprintf(stream, "final );\n");

    } // FOR_EVERY_ELEMENT_IN_LIST
}

/******************************************************************************/

bool is_inspection_point
(
    InspectionPoint_t *ipoint,
    OutputType_t       type,
    OutputQuantity_t   quantity
)
{
    if (ipoint->Type != type)

        return false ;

    switch (type)
    {
        case TDICE_OUTPUT_TYPE_NONE :

            return false ;

        case TDICE_OUTPUT_TYPE_TCELL :
        case TDICE_OUTPUT_TYPE_TMAP :
        case TDICE_OUTPUT_TYPE_PMAP :

            return true ;

        case TDICE_OUTPUT_TYPE_TFLP :

            return ipoint->Pointer.Tflp->Quantity == quantity ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            return ipoint->Pointer.Tflpel->Quantity == quantity ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            return ipoint->Pointer.Tcoolant->Quantity == quantity ;

        default :

            fprintf (stderr, "Error: output type %d not supported\n", type) ;

            return false ;
    }
}

/******************************************************************************/

Error_t generate_inspection_point_header
(
    InspectionPoint_t *ipoint,
    Dimensions_t      *dimensions,
    String_t           prefix
)
{
    FILE *output_stream = fopen (ipoint->FileName, "w") ;

    if (output_stream == NULL)
    {
        fprintf (stderr,
            "Inspection Point: Cannot open output file %s\n",
            ipoint->FileName);

        return TDICE_FAILURE ;
    }

    switch (ipoint->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :

            fprintf (output_stream,
                "%sCell temperature for the location [%s, x=%5.3f,y=%5.3f]\n",
                prefix,
                ipoint->StackElement->Id,
                ipoint->Pointer.Tcell->Xval,
                ipoint->Pointer.Tcell->Yval) ;

            fprintf (output_stream,
                "%sNearest [column, row] indices found= [%d,%d] (locations [x=%5.3f,y=%5.3f])\n",
                prefix,
                ipoint->Pointer.Tcell->ColumnIndex,
                ipoint->Pointer.Tcell->RowIndex,
                ipoint->Pointer.Tcell->ActualXval,
                ipoint->Pointer.Tcell->ActualYval);

            fprintf (output_stream,
                "%sTime(s) \t Temperature(K)\n", prefix);

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%s", prefix) ;

            if (ipoint->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflp\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the floorplan of the die %s\n",
                ipoint->StackElement->Id) ;

            if (ipoint->StackElement->Floorplan == NULL)
            {
                fprintf (stderr,
                    "Inspection Point: Error reading floorplan for the die %s\n",
                    ipoint->StackElement->Id) ;

                goto header_error ;
            }

            fprintf (output_stream, "%sTime(s) \t ", prefix);

            FOR_EVERY_ELEMENT_IN_LIST_NEXT

            (FloorplanElement_t, flp_el, ipoint->StackElement->Floorplan->ElementsList)

                fprintf (output_stream, "%s(K) \t ", flp_el->Id);

            fprintf(output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            fprintf (output_stream, "%s", prefix) ;

            if (ipoint->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflpel\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the floorplan element %s of the die %s\n",
                ipoint->Pointer.Tflpel->FloorplanElement->Id,
                ipoint->StackElement->Id) ;

            fprintf (output_stream,
                "%sTime(s) \t %s.%s(K)\n",
                prefix,
                ipoint->StackElement->Id,
                ipoint->Pointer.Tflpel->FloorplanElement->Id) ;

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :

            fprintf (output_stream,
                "%sThermal map for layer %s (please find axis information in \"xaxis.txt\" and \"yaxis.txt\")\n",
                prefix, ipoint->StackElement->Id);

            print_axes (dimensions) ;

            break ;

        case TDICE_OUTPUT_TYPE_PMAP :

            fprintf (output_stream,
                "%sPower map for layer %s (please find axis information in \"xaxis.txt\" and \"yaxis.txt\")\n",
                prefix, ipoint->StackElement->Id);

            print_axes (dimensions) ;

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            fprintf (output_stream, "%s", prefix) ;

            if (ipoint->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tcoolant\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the outlet of the channel %s\n",
                ipoint->StackElement->Id) ;

            fprintf (output_stream, "%sTime(s) \t Temperature(K)\n", prefix) ;

            break ;

        default :

            fprintf (stderr, "Error reading inspection point instruction\n") ;

            goto header_error ;
    }

    fclose (output_stream) ;

    return TDICE_SUCCESS ;

header_error :

    fclose (output_stream) ;

    return TDICE_FAILURE ;
}

/******************************************************************************/

Error_t generate_inspection_point_output
(
    InspectionPoint_t *ipoint,
    Dimensions_t      *dimensions,
    Temperature_t     *temperatures,
    Source_t          *sources,
    Time_t             current_time
)
{
    Quantity_t index, n_flp_el ;
    Temperature_t temperature, *result ;

    FILE *output_stream = fopen (ipoint->FileName, "a") ;

    if (output_stream == NULL)
    {
        fprintf (stderr,
            "Inspection Point: Cannot open output file %s\n",
            ipoint->FileName);

        return TDICE_FAILURE ;
    }

    switch (ipoint->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :

            index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement),
                 ipoint->Pointer.Tcell->RowIndex,
                 ipoint->Pointer.Tcell->ColumnIndex) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, *(temperatures + index)) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%5.3f \t ", current_time) ;

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            result = NULL ;

            if (ipoint->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                result = get_all_max_temperatures_floorplan

                    (ipoint->StackElement->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else if (ipoint->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                result = get_all_min_temperatures_floorplan

                    (ipoint->StackElement->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else

                result = get_all_avg_temperatures_floorplan

                    (ipoint->StackElement->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            for (index = 0 ; index != n_flp_el ; index++)

                fprintf (output_stream, "%5.3f \t ", result [index]) ;

            fprintf (output_stream, "\n") ;

            FREE_POINTER (free, result) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            if (ipoint->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_floorplan_element

                    (ipoint->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            else if (ipoint->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (ipoint->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            else

                temperature = get_avg_temperature_floorplan_element

                    (ipoint->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, temperature) ;

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :

            print_thermal_map_stack_element

                (ipoint->StackElement, dimensions,
                 temperatures, output_stream) ;

            fprintf (output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_PMAP :

            print_power_map_stack_element

                (ipoint->StackElement, dimensions,
                 sources, output_stream) ;

            fprintf (output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            if (ipoint->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                 temperature = get_max_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else if (ipoint->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                 temperature = get_min_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else

                 temperature = get_avg_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, temperature) ;

            break ;

        default :

            fprintf (stderr, "Error reading inspection point instruction\n") ;

            goto output_error ;
    }

    fclose (output_stream) ;

    return TDICE_SUCCESS ;

output_error :

    fclose (output_stream) ;

    return TDICE_FAILURE ;
}

/******************************************************************************/

void fill_message_inspection_point
(
    InspectionPoint_t *ipoint,
    OutputQuantity_t   output_quantity,
    Dimensions_t      *dimensions,
    Temperature_t     *temperatures,
    Source_t          *sources,
    NetworkMessage_t  *message
)
{
    switch (ipoint->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :
        {
            Quantity_t index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement),
                 ipoint->Pointer.Tcell->RowIndex,
                 ipoint->Pointer.Tcell->ColumnIndex) ;

            float temperature = *(temperatures + index) ;

            insert_message_word (message, &temperature) ;

            break ;
        }

        case TDICE_OUTPUT_TYPE_TFLP :
        {
            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            Temperature_t *tmp ;
            Quantity_t nflp, index ;

            if (output_quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                tmp = get_all_max_temperatures_floorplan

                    (ipoint->StackElement->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                tmp = get_all_min_temperatures_floorplan

                    (ipoint->StackElement->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            else

                tmp = get_all_avg_temperatures_floorplan

                    (ipoint->StackElement->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            insert_message_word (message, &nflp) ;

            for (index = 0 ; index != nflp ; index++)
            {
                Temperature_t t = tmp [ index ] ;

                insert_message_word (message, &t) ;
            }

            free (tmp) ;

            break ;
        }

        case TDICE_OUTPUT_TYPE_TFLPEL :
        {
            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            float temperature ;

            if (output_quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_floorplan_element

                    (ipoint->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (ipoint->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            else

                temperature = get_avg_temperature_floorplan_element

                    (ipoint->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            insert_message_word (message, &temperature) ;

            break ;
        }

        case TDICE_OUTPUT_TYPE_TMAP :
        {
            CellIndex_t n ;

            n = get_number_of_rows (dimensions) ;

            insert_message_word (message, &n) ;

            n = get_number_of_columns (dimensions) ;

            insert_message_word (message, &n) ;

            Quantity_t index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement),
                 0, 0) ;

            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    float temperature = *(temperatures + index++) ;

                    insert_message_word (message, &temperature) ;
                }
            }

            break ;
        }

        case TDICE_OUTPUT_TYPE_PMAP :
        {
            CellIndex_t n ;

            n = get_number_of_rows (dimensions) ;

            insert_message_word (message, &n) ;

            n = get_number_of_columns (dimensions) ;

            insert_message_word (message, &n) ;

            Quantity_t index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement),
                 0, 0) ;

            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    float source = *(sources + index++) ;

                    insert_message_word (message, &source) ;
                }
            }

            break ;
        }

        case TDICE_OUTPUT_TYPE_TCOOLANT :
        {
            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            float temperature ;

            if (output_quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            else

                temperature = get_avg_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            insert_message_word (message, &temperature) ;

            break ;
        }

        default :

            fprintf (stderr, "Error reading inspection point instruction\n") ;
   }
}

/******************************************************************************/
