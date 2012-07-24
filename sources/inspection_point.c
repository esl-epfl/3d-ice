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

#include "inspection_point.h"
#include "macros.h"

/******************************************************************************/

void inspection_point_init (InspectionPoint_t *ipoint)
{
    ipoint->FileName         = NULL ;
    ipoint->Instant          = (OutputInstant_t)  TDICE_OUTPUT_INSTANT_NONE ;
    ipoint->Type             = (OutputType_t)     TDICE_OUTPUT_TYPE_NONE ;
    ipoint->Quantity         = (OutputQuantity_t) TDICE_OUTPUT_QUANTITY_NONE ;
    ipoint->Xval             = (ChipDimension_t) 0.0 ;
    ipoint->ActualXval       = (ChipDimension_t) 0.0 ;
    ipoint->Yval             = (ChipDimension_t) 0.0 ;
    ipoint->ActualYval       = (ChipDimension_t) 0.0 ;
    ipoint->RowIndex         = (CellIndex_t) 0u ;
    ipoint->ColumnIndex      = (CellIndex_t) 0u ;
    ipoint->StackElement     = NULL ;
    ipoint->FloorplanElement = NULL ;
}

/******************************************************************************/

void inspection_point_copy (InspectionPoint_t *dst, InspectionPoint_t *src)
{
    inspection_point_destroy (dst) ;

    dst->Instant          = src->Instant ;
    dst->Type             = src->Type ;
    dst->Quantity         = src->Quantity ;
    dst->Xval             = src->Xval ;
    dst->ActualXval       = src->ActualXval ;
    dst->Yval             = src->Yval ;
    dst->ActualYval       = src->ActualYval ;
    dst->RowIndex         = src->RowIndex ;
    dst->ColumnIndex      = src->ColumnIndex ;
    dst->StackElement     = src->StackElement ;
    dst->FloorplanElement = src->FloorplanElement ;

    dst->FileName = (src->FileName == NULL) ? NULL : strdup (src->FileName) ;
}

/******************************************************************************/

void inspection_point_destroy (InspectionPoint_t *ipoint)
{
    if (ipoint->FileName != NULL)

        free (ipoint->FileName) ;

    inspection_point_init (ipoint) ;
}

/******************************************************************************/

InspectionPoint_t *inspection_point_calloc (void)
{
    InspectionPoint_t *ipoint =

        (InspectionPoint_t *) malloc (sizeof(InspectionPoint_t)) ;

    if (ipoint != NULL)

        inspection_point_init (ipoint) ;

    return ipoint ;
}

/******************************************************************************/

InspectionPoint_t *inspection_point_clone (InspectionPoint_t *ipoint)
{
    if (ipoint == NULL)

        return NULL ;

    InspectionPoint_t *newi = inspection_point_calloc ( ) ;

    if (newi != NULL)

        inspection_point_copy (newi, ipoint) ;

    return newi ;
}

/******************************************************************************/

void inspection_point_free (InspectionPoint_t *ipoint)
{
    if (ipoint == NULL)

        return ;

    inspection_point_destroy (ipoint) ;

    free (ipoint) ;
}

/******************************************************************************/

bool inspection_point_same_filename
(
    InspectionPoint_t *ipoint,
    InspectionPoint_t *other
)
{
    return strcmp (ipoint->FileName, other->FileName) == 0 ? true : false ;
}

/******************************************************************************/

void inspection_point_print
(
    InspectionPoint_t *ipoint,
    FILE              *stream,
    String_t           prefix
)
{
    switch (ipoint->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :

            fprintf (stream, "%sT        (%s, %.1f, %.1f, \"%s\", ",
                prefix,     ipoint->StackElement->Id,
                ipoint->Xval, ipoint->Yval,
                ipoint->FileName) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (stream, "%sTflp     (%s, \"%s\", ",
                prefix, ipoint->StackElement->Id,
                ipoint->FileName) ;

            if (ipoint->Quantity==TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf(stream, "maximum, ");

            else if (ipoint->Quantity==TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf(stream, "minimum, ");

            else

                fprintf(stream, "average, ");

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            fprintf (stream, "%sTflpel   (%s.%s, \"%s\", ",
                prefix, ipoint->StackElement->Id,
                ipoint->FloorplanElement->Id,
                ipoint->FileName) ;

            if (ipoint->Quantity==TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf(stream, "maximum, ");

            else if (ipoint->Quantity==TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf(stream, "minimum, ");

            else

                fprintf(stream, "average, ");

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :

            fprintf (stream, "%sTmap     (%s, \"%s\", ",
                prefix, ipoint->StackElement->Id,
                ipoint->FileName) ;

            break ;

        case TDICE_OUTPUT_TYPE_PMAP :

            fprintf (stream, "%sPmap     (%s, \"%s\", ",
                prefix, ipoint->StackElement->Id,
                ipoint->FileName) ;

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            fprintf (stream, "%sTcoolant (%s, \"%s\", ",
                prefix, ipoint->StackElement->Id,
                ipoint->FileName) ;

            if (ipoint->Quantity==TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf(stream, "maximum, ");

            else if (ipoint->Quantity==TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf(stream, "minimum, ");

            else

                fprintf(stream, "average, ");

            break ;

        default :

            fprintf (stderr, "Undefined inspection point command type %d\n", ipoint->Type) ;
            break ;
    }

    if (ipoint->Instant == TDICE_OUTPUT_INSTANT_SLOT)

        fprintf(stream, "slot );\n");

    else if (ipoint->Instant == TDICE_OUTPUT_INSTANT_STEP)

        fprintf(stream, "step );\n");

    else

        fprintf(stream, "final );\n");
}

/******************************************************************************/

void align_tcell
(
    InspectionPoint_t  *ipoint,
    ChipDimension_t  xval,
    ChipDimension_t  yval,
    Dimensions_t    *dimensions
)
{
    FOR_EVERY_ROW (row_index, dimensions)
    {
        ipoint->RowIndex   = row_index ;

        if (   yval >= get_cell_location_y (dimensions, row_index)
            && yval <  get_cell_location_y (dimensions, row_index + 1))
        {
            ipoint->ActualYval = get_cell_location_y (dimensions, row_index) ;
            break ;
        }
    }

    ipoint->Yval = yval ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        ipoint->ColumnIndex = column_index ;

        if (   xval >= get_cell_location_x (dimensions, column_index)
            && xval <  get_cell_location_x (dimensions, column_index + 1))
        {
            ipoint->ActualXval = get_cell_location_x (dimensions, column_index) ;
            break ;
        }
    }

    ipoint->Xval = xval ;
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
        case TDICE_OUTPUT_TYPE_TCOOLANT :
        case TDICE_OUTPUT_TYPE_TFLPEL :

            return ipoint->Quantity == quantity ;

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
                ipoint->StackElement->Id, ipoint->Xval, ipoint->Yval) ;

            fprintf (output_stream,
                "%sNearest [column, row] indices found= [%d,%d] (locations [x=%5.3f,y=%5.3f])\n",
                prefix,
                ipoint->ColumnIndex, ipoint->RowIndex,
                ipoint->ActualXval,  ipoint->ActualYval);

            fprintf (output_stream,
                "%sTime(s) \t Temperature(K)\n", prefix);

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%s", prefix) ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

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

            fprintf (output_stream, "%sTime(s) \t ", prefix);

            FloorplanElementListNode_t *flpeln ;

            for (flpeln  = floorplan_element_list_begin (&ipoint->StackElement->Pointer.Die->Floorplan.ElementsList) ;
                 flpeln != NULL ;
                 flpeln  = floorplan_element_list_next (flpeln))
            {
                FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

                fprintf (output_stream, "%s(K) \t ", flpel->Id);

            }
            fprintf(output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            fprintf (output_stream, "%s", prefix) ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflpel\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the floorplan element %s of the die %s\n",
                ipoint->FloorplanElement->Id,
                ipoint->StackElement->Id) ;

            fprintf (output_stream,
                "%sTime(s) \t %s.%s(K)\n",
                prefix,
                ipoint->StackElement->Id,
                ipoint->FloorplanElement->Id) ;

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

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

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
                 ipoint->RowIndex, ipoint->ColumnIndex) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, *(temperatures + index)) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%5.3f \t ", current_time) ;

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            result = NULL ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                result = get_all_max_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                result = get_all_min_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else

                result = get_all_avg_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            for (index = 0 ; index != n_flp_el ; index++)

                fprintf (output_stream, "%5.3f \t ", result [index]) ;

            fprintf (output_stream, "\n") ;

            free (result) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else

                temperature = get_avg_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, temperature) ;

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :

            stack_element_print_thermal_map

                (ipoint->StackElement, dimensions,
                 temperatures, output_stream) ;

            fprintf (output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_PMAP :

            stack_element_print_power_map

                (ipoint->StackElement, dimensions,
                 sources, output_stream) ;

            fprintf (output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement), 0, 0) ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                 temperature = get_max_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

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
                 ipoint->RowIndex, ipoint->ColumnIndex) ;

            float temperature = *(temperatures + index) ;

            fprintf (stderr, "%.4f\n", temperature) ;

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

                    (&ipoint->StackElement->Pointer.Die->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                tmp = get_all_min_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            else

                tmp = get_all_avg_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan, dimensions,
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

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else

                temperature = get_avg_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

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
