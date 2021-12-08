/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#include <stdlib.h> // For the memory functions malloc/free

#include "inspection_point.h"

/******************************************************************************/

void inspection_point_init (InspectionPoint_t *ipoint)
{
    string_init (&ipoint->FileName) ;

    ipoint->Instant          = (OutputInstant_t)  TDICE_OUTPUT_INSTANT_NONE ;
    ipoint->OType            = (OutputType_t)     TDICE_OUTPUT_TYPE_NONE ;
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
    dst->OType            = src->OType ;
    dst->Quantity         = src->Quantity ;
    dst->Xval             = src->Xval ;
    dst->ActualXval       = src->ActualXval ;
    dst->Yval             = src->Yval ;
    dst->ActualYval       = src->ActualYval ;
    dst->RowIndex         = src->RowIndex ;
    dst->ColumnIndex      = src->ColumnIndex ;
    dst->StackElement     = src->StackElement ;
    dst->FloorplanElement = src->FloorplanElement ;

    string_copy (&dst->FileName, &src->FileName) ;
}

/******************************************************************************/

void inspection_point_destroy (InspectionPoint_t *ipoint)
{
    string_destroy (&ipoint->FileName) ;

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
    return string_equal (&ipoint->FileName, &other->FileName) ;
}

/******************************************************************************/

void inspection_point_print
(
    InspectionPoint_t *ipoint,
    FILE              *stream,
    String_t           prefix
)
{
    switch (ipoint->OType)
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

            fprintf (stderr, "Undefined inspection point command type %d\n", ipoint->OType) ;
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

    CellIndex_t row ;

    for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
    {
        ipoint->RowIndex = row ;

        if (   yval >= get_cell_location_y (dimensions, row)
            && yval <  get_cell_location_y (dimensions, row + 1))
        {
            ipoint->ActualYval = get_cell_location_y (dimensions, row) ;
            break ;
        }
    }

    ipoint->Yval = yval ;

    CellIndex_t column ;

    for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
    {
        ipoint->ColumnIndex = column ;

        if (   xval >= get_cell_location_x (dimensions, column)
            && xval <  get_cell_location_x (dimensions, column + 1))
        {
            ipoint->ActualXval = get_cell_location_x (dimensions, column) ;
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
    if (ipoint->OType != type)

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

    switch (ipoint->OType)
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

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                fprintf (output_stream, "Gradient ");

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

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                fprintf (output_stream, "Gradient ");

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
            if (dimensions->NonUniform == 1)
            {
                fprintf (output_stream,
                    "%sThermal map for layer %s (please find axis information in \"xyaxis_[stack id].txt\")\n",
                    prefix, ipoint->StackElement->Id);
            }
            else
            {
                fprintf (output_stream,
                    "%sThermal map for layer %s (please find axis information in \"xaxis.txt\" and \"yaxis.txt\")\n",
                    prefix, ipoint->StackElement->Id);

                print_axes (dimensions) ;
            }

            break ;

        case TDICE_OUTPUT_TYPE_PMAP :
            if (dimensions->NonUniform == 1)
            {
                fprintf (output_stream,
                    "%sPower map for layer %s (please find axis information in \"xyaxis_[stack id].txt\")\n",
                    prefix, ipoint->StackElement->Id);
            }
            else
            {
                fprintf (output_stream,
                    "%sPower map for layer %s (please find axis information in \"xaxis.txt\" and \"yaxis.txt\")\n",
                    prefix, ipoint->StackElement->Id);

                print_axes (dimensions) ;

            }

            break ;

        case TDICE_OUTPUT_TYPE_TCOOLANT :

            fprintf (output_stream, "%s", prefix) ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                fprintf (output_stream, "Maximum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                fprintf (output_stream, "Minimum ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                fprintf (output_stream, "Average ");

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                fprintf (output_stream, "Gradient ");

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
    Quantity_t index = 0;
    Quantity_t n_flp_el;
    Temperature_t temperature, *result ;

    FILE *output_stream = fopen (ipoint->FileName, "a") ;

    if (output_stream == NULL)
    {
        fprintf (stderr,
            "Inspection Point: Cannot open output file %s\n",
            ipoint->FileName);

        return TDICE_FAILURE ;
    }

    switch (ipoint->OType)
    {
        case TDICE_OUTPUT_TYPE_TCELL :

            if (dimensions->NonUniform == 1)
            {
                // find the thermal cell in the nou-uniform grid scenario
                CellIndex_t counter = 0;
                ChipDimension_t x = ipoint->Xval;
                ChipDimension_t y = ipoint->Yval;
                CellIndex_t layer_offset = get_source_layer_offset(ipoint->StackElement) ;
                for (Non_uniform_cellListNode_t* cell_i = dimensions->Cell_list.First; cell_i != NULL; cell_i = cell_i->Next)
                {
                    if (cell_i->Data.layer_info == layer_offset)
                    {
                        if (
                            x >= cell_i->Data.left_x && 
                            x <= cell_i->Data.left_x + cell_i->Data.length &&
                            y >= cell_i->Data.left_y && 
                            y <= cell_i->Data.left_y + cell_i->Data.width
                        )
                        {
                            index = counter;
                            break;
                        }
                    }
                    counter++;
                }
            }
            else
            {
                index = get_cell_offset_in_stack

                    (dimensions,
                    get_source_layer_offset(ipoint->StackElement),
                    ipoint->RowIndex, ipoint->ColumnIndex) ;
            }



            fprintf (output_stream,
                "%5.3f \t %7.3f\n", current_time, *(temperatures + index)) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLP :

            fprintf (output_stream, "%5.3f \t ", current_time) ;
            
            if (dimensions->NonUniform != 1)
            {
                temperatures += get_cell_offset_in_stack

                    (dimensions,
                    get_source_layer_offset(ipoint->StackElement),
                    first_row (dimensions), first_column (dimensions)) ;
            }
            
            result = NULL ;

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                result = get_all_max_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                result = get_all_min_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                result = get_all_avg_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                result = get_all_gradient_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan,
                     dimensions, temperatures, &n_flp_el, NULL) ;

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflp\n") ;

                break ;
            }

            for (index = 0 ; index != n_flp_el ; index++)

                fprintf (output_stream, "%5.3f \t ", result [index]) ;

            fprintf (output_stream, "\n") ;

            free (result) ;

            break ;

        case TDICE_OUTPUT_TYPE_TFLPEL :
            
            // offset the temperature pointer in unifrom grid scenario, while it doesn't need to
            // offset the temperature pointer in non-uniform grid scenario.
            if (dimensions->NonUniform != 1)
            {
                temperatures += get_cell_offset_in_stack

                    (dimensions,
                    get_source_layer_offset(ipoint->StackElement),
                    first_row (dimensions), first_column (dimensions)) ;
            }


            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                temperature = get_avg_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                temperature = get_gradient_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflpel\n") ;

                break ;
            }

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

            if (dimensions->NonUniform == 1)
            {
                // output all of the cells' temperature in the layer for non-uniform scenario
                CellIndex_t counter = 0;
                CellIndex_t layer_offset = get_source_layer_offset(ipoint->StackElement) ;
                for (Non_uniform_cellListNode_t* cell_i = dimensions->Cell_list.First; cell_i != NULL; cell_i = cell_i->Next)
                {
                    if (cell_i->Data.layer_info == layer_offset)
                    {
                        break ;
                    }
                    counter++;
                }
                temperatures += counter;
            }
            else{
                temperatures += get_cell_offset_in_stack

                    (dimensions,
                    get_source_layer_offset(ipoint->StackElement),
                    first_row (dimensions), first_column (dimensions)) ;
            }

            if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                 temperature = get_max_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                 temperature = get_min_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                 temperature = get_avg_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else if (ipoint->Quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                 temperature = get_gradient_temperature_channel_outlet

                     (ipoint->StackElement->Pointer.Channel, dimensions, temperatures) ;

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tcoolant\n") ;

                break ;
            }

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
    switch (ipoint->OType)
    {
        case TDICE_OUTPUT_TYPE_TCELL :
        {
            Quantity_t index = get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement),
                 ipoint->RowIndex, ipoint->ColumnIndex) ;

            float temperature = *(temperatures + index) ;

            insert_message_word (message, &temperature) ;

            break ;
        }
        case TDICE_OUTPUT_TYPE_TFLP :
        {
            temperatures += get_cell_offset_in_stack

                (dimensions,
                 get_source_layer_offset(ipoint->StackElement),
                 first_row (dimensions), first_column (dimensions)) ;

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

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                tmp = get_all_avg_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                tmp = get_all_gradient_temperatures_floorplan

                    (&ipoint->StackElement->Pointer.Die->Floorplan, dimensions,
                      temperatures, &nflp, NULL) ;

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflp\n") ;

                break ;
            }

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
                 get_source_layer_offset(ipoint->StackElement),
                 first_row (dimensions), first_column (dimensions)) ;


            float temperature ;

            if (output_quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                temperature = get_avg_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                temperature = get_gradient_temperature_floorplan_element

                    (ipoint->FloorplanElement, dimensions, temperatures) ;

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tflpel\n") ;

                break ;
            }

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
                 first_row (dimensions), first_column (dimensions)) ;

            CellIndex_t row ;
            CellIndex_t column ;

            for (row = first_row (dimensions) ; row <= last_row  (dimensions) ; row++)
            {
                for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
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
                 first_row (dimensions), first_column (dimensions)) ;

            CellIndex_t row ;
            CellIndex_t column ;

            for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
            {
                for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
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
                 get_source_layer_offset(ipoint->StackElement),
                 first_row (dimensions), first_column (dimensions)) ;

            float temperature ;

            if (output_quantity == TDICE_OUTPUT_QUANTITY_MAXIMUM)

                temperature = get_max_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_MINIMUM)

                temperature = get_min_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_AVERAGE)

                temperature = get_avg_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            else if (output_quantity == TDICE_OUTPUT_QUANTITY_GRADIENT)

                temperature = get_gradient_temperature_channel_outlet

                    (ipoint->StackElement->Pointer.Channel,
                     dimensions, temperatures) ;

            else
            {
                fprintf (stderr,
                    "Inspection Point: Error reading output quantity for Tcoolant\n") ;

                break ;
            }

            insert_message_word (message, &temperature) ;

            break ;
        }
        default :

            fprintf (stderr, "Error reading inspection point instruction\n") ;
   }
}

/******************************************************************************/
