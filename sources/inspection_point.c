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
#include "floorplan.h"
#include "macros.h"

/******************************************************************************/

void init_tcell (Tcell *tcell)
{
    tcell->Xval        = 0.0 ;
    tcell->ActualXval  = 0.0 ;
    tcell->Yval        = 0.0 ;
    tcell->ActualYval  = 0.0 ;
    tcell->RowIndex    = 0u ;
    tcell->ColumnIndex = 0u ;
}

/******************************************************************************/

Tcell *alloc_and_init_tcell (void)
{
    Tcell *tcell = (Tcell *) malloc (sizeof(Tcell)) ;

    if (tcell != NULL)

        init_tcell (tcell) ;

    return tcell ;
}

/******************************************************************************/

void free_tcell (Tcell *tcell)
{
    FREE_POINTER(free, tcell) ;
}

/******************************************************************************/

void print_detailed_tcell (FILE  *stream, String_t prefix, Tcell *tcell)
{
    fprintf (stream,
        "%sTcell                   = %p\n",
        prefix, tcell) ;

    fprintf (stream,
        "%s  Xval                  = %.2f\n",
        prefix, tcell->Xval) ;

    fprintf (stream,
        "%s  actualXval            = %.2f\n",
        prefix, tcell->ActualXval) ;

    fprintf (stream,
        "%s  Yval                  = %.2f\n",
        prefix, tcell->Yval) ;

    fprintf (stream,
        "%s  actualYval            = %.2f\n",
        prefix, tcell->ActualYval) ;

    fprintf (stream,
        "%s  RowIndex              = %d\n",
        prefix, tcell->RowIndex) ;

    fprintf (stream,
        "%s  ColumnIndex           = %d\n",
        prefix, tcell->ColumnIndex) ;
}

/******************************************************************************/

void align_tcell
(
    Tcell           *tcell,
    ChipDimension_t  xval,
    ChipDimension_t  yval,
    Dimensions      *dimensions
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

void init_tflp (Tflp *tflp)
{
    tflp->Quantity = TDICE_OUTPUT_QUANTITY_NONE ;
}

/******************************************************************************/

Tflp *alloc_and_init_tflp (void)
{
    Tflp *tflp = (Tflp *) malloc (sizeof(Tflp)) ;

    if (tflp != NULL)

        init_tflp (tflp) ;

    return tflp ;
}

/******************************************************************************/

void free_tflp (Tflp *tflp)
{
    FREE_POINTER(free, tflp) ;
}

/******************************************************************************/

void print_detailed_tflp (FILE  *stream, String_t prefix, Tflp *tflp)
{
    fprintf (stream,
        "%sTflp                    = %p\n",
        prefix, tflp) ;

    fprintf (stream,
        "%s  Quantity              = %d\n",
        prefix, tflp->Quantity) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tflpel (Tflpel *tflpel)
{
    tflpel->FloorplanElement = NULL ;
    tflpel->Quantity         = TDICE_OUTPUT_QUANTITY_NONE ;
}

/******************************************************************************/

Tflpel *alloc_and_init_tflpel (void)
{
    Tflpel *tflpel = (Tflpel *) malloc (sizeof(Tflpel)) ;

    if (tflpel != NULL)

        init_tflpel(tflpel) ;

    return tflpel ;
}

/******************************************************************************/

void free_tflpel (Tflpel *tflpel)
{
    FREE_POINTER(free, tflpel) ;
}

/******************************************************************************/

void print_detailed_tflpel (FILE  *stream, String_t prefix, Tflpel *tflpel)
{
    fprintf (stream,
        "%sTflpel                  = %p\n",
        prefix, tflpel) ;

    fprintf (stream,
        "%s  FloorplanElement      = %p\n",
        prefix, tflpel->FloorplanElement) ;

    fprintf (stream,
        "%s  Quantity              = %d\n",
        prefix, tflpel->Quantity) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tcoolant (Tcoolant *tcoolant)
{
    tcoolant->Quantity = TDICE_OUTPUT_QUANTITY_NONE ;
}

/******************************************************************************/

Tcoolant *alloc_and_init_tcoolant (void)
{
    Tcoolant *tcoolant = (Tcoolant *) malloc (sizeof(Tcoolant)) ;

    if (tcoolant != NULL)

        init_tcoolant (tcoolant) ;

    return tcoolant ;
}

/******************************************************************************/

void free_tcoolant (Tcoolant *tcoolant)
{
    FREE_POINTER (free, tcoolant) ;
}

/******************************************************************************/

void print_detailed_tcoolant (FILE *stream, String_t prefix, Tcoolant *tcoolant)
{
    fprintf (stream,
        "%sTcoolant                = %p\n",
        prefix, tcoolant) ;

    fprintf (stream,
        "%s  Quantity              = %d\n",
        prefix, tcoolant->Quantity) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_inspection_point (InspectionPoint *inspection_point)
{
    inspection_point->Type             = TDICE_OUTPUT_TYPE_NONE ;
    inspection_point->Instant          = TDICE_OUTPUT_INSTANT_NONE ;
    inspection_point->FileName         = NULL ;
    inspection_point->Pointer.Tcell    = NULL ;
    inspection_point->Pointer.Tflp     = NULL ;
    inspection_point->Pointer.Tflpel   = NULL ;
    inspection_point->Pointer.Tcoolant = NULL ;
    inspection_point->StackElement     = NULL ;
    inspection_point->Next             = NULL ;
}

/******************************************************************************/

InspectionPoint *alloc_and_init_inspection_point (void)
{
    InspectionPoint *inspection_point = (InspectionPoint *) malloc (sizeof(InspectionPoint)) ;

    if (inspection_point != NULL)

        init_inspection_point (inspection_point) ;

    return inspection_point ;
}

/******************************************************************************/

void free_inspection_point (InspectionPoint *inspection_point)
{
    if (   inspection_point->Type == TDICE_OUTPUT_TYPE_TCELL
        && inspection_point->Pointer.Tcell != NULL)

        FREE_POINTER (free_tcell, inspection_point->Pointer.Tcell) ;

    else if (   inspection_point->Type == TDICE_OUTPUT_TYPE_TFLP
             && inspection_point->Pointer.Tflp != NULL)

        FREE_POINTER (free_tflp, inspection_point->Pointer.Tflp) ;

    else if (   inspection_point->Type == TDICE_OUTPUT_TYPE_TFLPEL
             && inspection_point->Pointer.Tflpel != NULL)

        FREE_POINTER (free_tflpel, inspection_point->Pointer.Tflpel) ;

    else if (   inspection_point->Type == TDICE_OUTPUT_TYPE_TCOOLANT
             && inspection_point->Pointer.Tcoolant != NULL)

        FREE_POINTER (free_tcoolant, inspection_point->Pointer.Tcoolant) ;

    inspection_point->StackElement = NULL ;

    FREE_POINTER (free, inspection_point->FileName) ;
    FREE_POINTER (free, inspection_point) ;
}

/******************************************************************************/

void free_inspection_point_list (InspectionPoint *list)
{
    FREE_LIST (InspectionPoint, list, free_inspection_point) ;
}

/******************************************************************************/

void print_formatted_inspection_point_list
(
    FILE            *stream,
    String_t         prefix,
    InspectionPoint *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint, inspection_point, list)
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

void print_detailed_inspection_point_list
(
    FILE            *stream,
    String_t         prefix,
    InspectionPoint *list
)
{
    String_t new_prefix =

        (String_t) malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint, ipoint, list)
    {
        fprintf (stream,
            "%sinspection_point                = %p\n",
            prefix, ipoint);

        fprintf (stream,
            "%s  FileName                  = %s\n",
            prefix, ipoint->FileName);

        fprintf (stream,
            "%s  Instant                   = %d\n",
            prefix, ipoint->Instant);

        fprintf (stream,
            "%s  Type                      = %d\n",
            prefix, ipoint->Type);

        switch(ipoint->Type)
        {
            case TDICE_OUTPUT_TYPE_TCELL :

                fprintf (stream,
                    "%s  Pointer.Tcell             = %p\n",
                    prefix, ipoint->Pointer.Tcell);

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tcell

                        (stream, new_prefix, ipoint->Pointer.Tcell) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            case TDICE_OUTPUT_TYPE_TFLP :

                fprintf (stream,
                    "%s  Pointer.Tflp              = %p\n",
                    prefix, ipoint->Pointer.Tflp);

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tflp

                        (stream, new_prefix, ipoint->Pointer.Tflp) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            case TDICE_OUTPUT_TYPE_TFLPEL :

                fprintf (stream,
                    "%s  Pointer.Tflpel            = %p\n",
                    prefix, ipoint->Pointer.Tflpel);

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tflpel

                        (stream, new_prefix, ipoint->Pointer.Tflpel) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            case TDICE_OUTPUT_TYPE_TCOOLANT :

                fprintf (stream,
                    "%s  Pointer.Tcoolant          = %p\n",
                    prefix, ipoint->Pointer.Tcoolant);

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tcoolant

                        (stream, new_prefix, ipoint->Pointer.Tcoolant) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            default :

                fprintf (stream, "Undefined inspection point command Type %d\n", ipoint->Type) ;

                break ;
        }

        fprintf (stream,
            "%s  StackElement              = %p\n",
            prefix, ipoint->StackElement);

        fprintf (stream,
            "%s  Next                      = %p\n",
            prefix, ipoint->Next);

        fprintf (stream, "%s\n", prefix) ;

    } // FOR_EVERY_ELEMENT_IN_LIST

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

Error_t generate_inspection_point_header
(
    InspectionPoint *inspection_point,
    Dimensions      *dimensions,
    String_t         prefix
)
{
    FILE *output_stream = fopen (inspection_point->FileName, "w") ;

    if (output_stream == NULL)
    {
        fprintf (stderr,
            "Inspection Point: Cannot open output file %s\n",
            inspection_point->FileName);

        return TDICE_FAILURE ;
    }

    switch (inspection_point->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :

            fprintf (output_stream,
                "%sCell temperature for the location [%s, x=%5.3f,y=%5.3f]\n",
                prefix,
                inspection_point->StackElement->Id,
                inspection_point->Pointer.Tcell->Xval,
                inspection_point->Pointer.Tcell->Yval) ;

            fprintf (output_stream,
                "%sNearest [column, row] indices found= [%d,%d] (locations [x=%5.3f,y=%5.3f])\n",
                prefix,
                inspection_point->Pointer.Tcell->ColumnIndex,
                inspection_point->Pointer.Tcell->RowIndex,
                inspection_point->Pointer.Tcell->ActualXval,
                inspection_point->Pointer.Tcell->ActualYval);

            fprintf (output_stream,
                "%sTime(s) \t Temperature(K)\n", prefix);

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%s", prefix) ;

            if (inspection_point->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (inspection_point->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (inspection_point->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflp\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the floorplan of the die %s\n",
                inspection_point->StackElement->Id) ;

            if (inspection_point->StackElement->Floorplan == NULL)
            {
                fprintf (stderr,
                    "Inspection Point: Error reading floorplan for the die %s\n",
                    inspection_point->StackElement->Id) ;

                goto header_error ;
            }

            fprintf (output_stream, "%sTime(s) \t ", prefix);

            FOR_EVERY_ELEMENT_IN_LIST_NEXT

            (FloorplanElement, flp_el, inspection_point->StackElement->Floorplan->ElementsList)

                fprintf (output_stream, "%s(K) \t ", flp_el->Id);

            fprintf(output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            fprintf (output_stream, "%s", prefix) ;

            if (inspection_point->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (inspection_point->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (inspection_point->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflpel\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the floorplan element %s of the die %s\n",
                inspection_point->Pointer.Tflpel->FloorplanElement->Id,
                inspection_point->StackElement->Id) ;

            fprintf (output_stream,
                "%sTime(s) \t %s.%s(K)\n",
                prefix,
                inspection_point->StackElement->Id,
                inspection_point->Pointer.Tflpel->FloorplanElement->Id) ;

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :

            fprintf (output_stream,
                "%sThermal map for layer %s (please find axis information in \"xaxis.txt\" and \"yaxis.txt\")\n",
                prefix, inspection_point->StackElement->Id);

            print_axes (dimensions) ;

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            fprintf (output_stream, "%s", prefix) ;

            if (inspection_point->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (inspection_point->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (inspection_point->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tcoolant\n") ;

                goto header_error ;
            }

            fprintf (output_stream,
                "temperatures for the outlet of the channel %s\n",
                inspection_point->StackElement->Id) ;

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
    InspectionPoint *inspection_point,
    Dimensions      *dimensions,
    Temperature_t   *temperatures,
    Time_t           current_time
)
{
    Quantity_t index, n_flp_el ;
    Temperature_t temperature, *result ;

    FILE *output_stream = fopen (inspection_point->FileName, "a") ;

    if (output_stream == NULL)
    {
        fprintf (stderr,
            "Inspection Point: Cannot open output file %s\n",
            inspection_point->FileName);

        return TDICE_FAILURE ;
    }

    switch (inspection_point->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :

            index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(inspection_point->StackElement),
                 inspection_point->Pointer.Tcell->RowIndex,
                 inspection_point->Pointer.Tcell->ColumnIndex) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, *(temperatures + index)) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%5.3f \t ", current_time) ;

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(inspection_point->StackElement), 0, 0) ;

            result = NULL ;

            if (inspection_point->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                result = get_all_max_temperatures_floorplan

                    (inspection_point->StackElement->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else if (inspection_point->Pointer.Tflp->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                result = get_all_min_temperatures_floorplan

                    (inspection_point->StackElement->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else

                result = get_all_avg_temperatures_floorplan

                    (inspection_point->StackElement->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            for (index = 0 ; index != n_flp_el ; index++)

                fprintf (output_stream, "%5.3f \t ", result [index]) ;

            fprintf (output_stream, "\n") ;

            FREE_POINTER (free, result) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(inspection_point->StackElement), 0, 0) ;

            if (inspection_point->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_floorplan_element

                    (inspection_point->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            else if (inspection_point->Pointer.Tflpel->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (inspection_point->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            else

                temperature = get_avg_temperature_floorplan_element

                    (inspection_point->Pointer.Tflpel->FloorplanElement,
                     dimensions, temperatures) ;

            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, temperature) ;

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :

            print_thermal_map_stack_element

                (inspection_point->StackElement, dimensions,
                 temperatures, output_stream) ;

            fprintf (output_stream, "\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(inspection_point->StackElement), 0, 0) ;

            if (inspection_point->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                 temperature = get_max_temperature_channel_outlet

                     (inspection_point->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else if (inspection_point->Pointer.Tcoolant->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                 temperature = get_min_temperature_channel_outlet

                     (inspection_point->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else

                 temperature = get_avg_temperature_channel_outlet

                     (inspection_point->StackElement->Pointer.Channel, dimensions, temperatures) ;

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
    InspectionPoint *inspection_point,
    Dimensions      *dimensions,
    Temperature_t   *temperatures,
    NetworkMessage  *message
)
{
    switch (inspection_point->Type)
    {
        case TDICE_OUTPUT_TYPE_TCELL :
        {
            Quantity_t index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(inspection_point->StackElement),
                 inspection_point->Pointer.Tcell->RowIndex,
                 inspection_point->Pointer.Tcell->ColumnIndex) ;

            float temperature = *(temperatures + index) ;

            insert_message_word (message, &temperature) ;

            break ;
        }
        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (stderr, "TFLP output in message not supperted\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :

            fprintf (stderr, "TFLPEL output in message not supperted\n") ;

            break ;

        case TDICE_OUTPUT_TYPE_TMAP :
        {
            CellIndex_t n ;

            n = get_number_of_rows (dimensions) ;

            insert_message_word (message, &n) ;

            n = get_number_of_columns (dimensions) ;

            insert_message_word (message, &n) ;

            Quantity_t index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(inspection_point->StackElement),
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
        case TDICE_OUTPUT_TYPE_TCOOLANT :

            fprintf (stderr, "TCOOLANT output in message not supperted\n") ;

            break ;

        default :

            fprintf (stderr, "Error reading inspection point instruction\n") ;
   }
}

/******************************************************************************/
