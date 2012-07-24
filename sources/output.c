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

#include "output.h"
#include "macros.h"

/******************************************************************************/

void output_init (Output_t *output)
{
    inspection_point_list_init (&output->InspectionPointListFinal) ;
    inspection_point_list_init (&output->InspectionPointListSlot) ;
    inspection_point_list_init (&output->InspectionPointListStep) ;
}

/******************************************************************************/

void output_copy (Output_t *dst, Output_t *src)
{
    output_destroy (dst) ;

    inspection_point_list_copy

        (&dst->InspectionPointListFinal, &src->InspectionPointListFinal) ;

    inspection_point_list_copy

        (&dst->InspectionPointListSlot, &src->InspectionPointListSlot) ;

    inspection_point_list_copy

        (&dst->InspectionPointListStep, &src->InspectionPointListStep) ;
}

/******************************************************************************/

void output_destroy (Output_t *output)
{
    inspection_point_list_destroy (&output->InspectionPointListFinal) ;
    inspection_point_list_destroy (&output->InspectionPointListSlot) ;
    inspection_point_list_destroy (&output->InspectionPointListStep) ;

    output_init (output) ;
}

/******************************************************************************/

Quantity_t get_number_of_inspection_points
(
    Output_t         *output,
    OutputInstant_t   instant,
    OutputType_t      type,
    OutputQuantity_t  quantity
)
{
    Quantity_t number = 0u ;

    InspectionPointList_t *list = NULL ;

    if (instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = &output->InspectionPointListFinal ;

    else if (instant == TDICE_OUTPUT_INSTANT_STEP)

        list = &output->InspectionPointListStep ;

    else if (instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = &output->InspectionPointListSlot ;

    else
    {
        fprintf (stderr, "Error: Wrong ipoint instant %d\n", instant) ;

        return number ;
    }

    InspectionPointListNode_t *ipn ;

    for (ipn  = inspection_point_list_begin (list) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        if (is_inspection_point (ipoint, type, quantity) == true)

            number++ ;
    }

    return number ;
}

/******************************************************************************/

void output_print (Output_t *output, FILE *stream, String_t prefix)
{
    fprintf (stream, "%soutput :\n", prefix) ;

    String_t new_prefix = (String_t) malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    inspection_point_list_print (&output->InspectionPointListFinal, stream, new_prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    inspection_point_list_print (&output->InspectionPointListSlot, stream, new_prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    inspection_point_list_print (&output->InspectionPointListStep, stream, new_prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    free (new_prefix) ;
}

/******************************************************************************/

void add_inspection_point (Output_t *output, InspectionPoint_t *ipoint)
{
    InspectionPointList_t *list = NULL ;

    if (ipoint->Instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = &output->InspectionPointListFinal ;

    else if (ipoint->Instant == TDICE_OUTPUT_INSTANT_STEP)

        list = &output->InspectionPointListStep ;

    else if (ipoint->Instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = &output->InspectionPointListSlot ;

    else
    {
        fprintf (stderr, "Error: Unset ipoint instant %d\n", ipoint->Instant) ;

        return ;
    }


    inspection_point_list_insert_end (list, ipoint) ;
}

/******************************************************************************/

Error_t generate_output_headers
(
    Output_t     *output,
    Dimensions_t *dimensions,
    String_t      prefix
)
{
    InspectionPointListNode_t *ipn ;

    for (ipn  = inspection_point_list_begin (&output->InspectionPointListFinal) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        if (generate_inspection_point_header (ipoint, dimensions, prefix) != TDICE_SUCCESS)

            return TDICE_FAILURE ;
    }

    for (ipn  = inspection_point_list_begin (&output->InspectionPointListSlot) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        if (generate_inspection_point_header (ipoint, dimensions, prefix) != TDICE_SUCCESS)

            return TDICE_FAILURE ;
    }

    for (ipn  = inspection_point_list_begin (&output->InspectionPointListStep) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        if (generate_inspection_point_header (ipoint, dimensions, prefix) != TDICE_SUCCESS)

            return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t generate_output
(
    Output_t        *output,
    Dimensions_t    *dimensions,
    Temperature_t   *temperatures,
    Source_t        *sources,
    Time_t           current_time,
    OutputInstant_t  output_instant
)
{
    InspectionPointList_t *list ;

    if (output_instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = &output->InspectionPointListFinal ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_STEP)

        list = &output->InspectionPointListStep ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = &output->InspectionPointListSlot ;

    else
    {
        fprintf (stderr, "Error: Wrong ipoint instant %d\n", output_instant) ;

        return TDICE_FAILURE ;
    }

    InspectionPointListNode_t *ipn ;

    for (ipn  = inspection_point_list_begin (list) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        Error_t error = generate_inspection_point_output

            (ipoint, dimensions, temperatures, sources, current_time) ;

        if (error != TDICE_SUCCESS)

            return TDICE_FAILURE ;
    }

   return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t fill_output_message
(
    Output_t         *output,
    Dimensions_t     *dimensions,
    Temperature_t    *temperatures,
    Source_t         *sources,
    OutputInstant_t   output_instant,
    OutputType_t      output_type,
    OutputQuantity_t  output_quantity,
    NetworkMessage_t *message
)
{
    InspectionPointList_t *list ;

    if (output_instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = &output->InspectionPointListFinal ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_STEP)

        list = &output->InspectionPointListStep ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = &output->InspectionPointListSlot ;

    else
    {
        fprintf (stderr, "Error: Wrong ipoint instant %d\n", output_instant) ;

        return TDICE_FAILURE ;
    }

    InspectionPointListNode_t *ipn ;

    for (ipn  = inspection_point_list_begin (list) ;
         ipn != NULL ;
         ipn  = inspection_point_list_next (ipn))
    {
        InspectionPoint_t *ipoint = inspection_point_list_data (ipn) ;

        if (output_type == ipoint->Type)

            fill_message_inspection_point

                (ipoint, output_quantity, dimensions, temperatures, sources, message) ;
    }

   return TDICE_SUCCESS ;
}

/******************************************************************************/
