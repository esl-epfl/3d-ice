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

#include "analysis.h"
#include "macros.h"

/******************************************************************************/

void init_analysis (Analysis_t *this)
{
    this->AnalysisType         = TDICE_ANALYSIS_TYPE_NONE ;
    this->StepTime             = (Time_t) 0.0 ;
    this->SlotTime             = (Time_t) 0.0 ;
    this->SlotLength           = 0u ;
    this->CurrentTime          = 0u ;
    this->InitialTemperature   = (Temperature_t) 0.0 ;
    this->InspectionPointListFinal = NULL ;
    this->InspectionPointListSlot  = NULL ;
    this->InspectionPointListStep  = NULL ;
}

/******************************************************************************/

Analysis_t *alloc_and_init_analysis (void)
{
    Analysis_t *analysis = (Analysis_t *) malloc (sizeof(Analysis_t)) ;

    if (analysis != NULL)

        init_analysis (analysis) ;

    return analysis ;
}

/******************************************************************************/

void free_analysis (Analysis_t *this)
{
    FREE_LIST (InspectionPoint_t, this->InspectionPointListFinal, free_inspection_point) ;
    FREE_LIST (InspectionPoint_t, this->InspectionPointListSlot, free_inspection_point) ;
    FREE_LIST (InspectionPoint_t, this->InspectionPointListStep, free_inspection_point) ;
}

/******************************************************************************/

Time_t get_simulated_time (Analysis_t *this)
{
  return this->CurrentTime * this->StepTime ;
}

/******************************************************************************/

void increase_by_step_time (Analysis_t *this)
{
    this->CurrentTime++ ;
}

/******************************************************************************/

Quantity_t get_number_of_inspection_points
(
    Analysis_t       *this,
    OutputInstant_t   instant,
    OutputType_t      type,
    OutputQuantity_t  quantity
)
{
    Quantity_t number = 0u ;

    InspectionPoint_t *list ;

    if (instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = this->InspectionPointListFinal ;

    else if (instant == TDICE_OUTPUT_INSTANT_STEP)

        list = this->InspectionPointListStep ;

    else if (instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = this->InspectionPointListSlot ;

    else

        return number ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, ipoint, list)

        if (is_inspection_point (ipoint, type, quantity) == true)

            number++ ;

    return number ;
}

/******************************************************************************/

bool slot_completed (Analysis_t *this)
{
    if (this->CurrentTime % this->SlotLength == 0u)

        return true ;

    return false ;
}

/******************************************************************************/

void print_formatted_analysis
(
    Analysis_t *this,
    FILE       *stream,
    String_t    prefix
)
{
    fprintf (stream, "%ssolver : ", prefix) ;

    if (this->AnalysisType == TDICE_ANALYSIS_TYPE_STEADY)

        fprintf (stream, "steady ;\n") ;

    else

        fprintf (stream, "transient step %.2f, slot %.2f ;\n",
            this->StepTime, this->SlotTime) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream, "%sinitial temperature  %.2f ;\n",
        prefix, this->InitialTemperature) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream, "%soutput :\n", prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_inspection_point_list

        (this->InspectionPointListFinal, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_inspection_point_list

        (this->InspectionPointListSlot, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_inspection_point_list

        (this->InspectionPointListStep, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

void print_detailed_analysis
(
  Analysis_t *this,
  FILE       *stream,
  String_t    prefix
)
{
    String_t new_prefix = (String_t) malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
        "%sAnalysis                        = %p\n",
        prefix, this) ;

    fprintf (stream,
        "%s  AnalysisType                  = %d\n",
        prefix, this->AnalysisType) ;

    fprintf (stream,
        "%s  StepTime                      = %.2f\n",
        prefix, this->StepTime) ;

    fprintf (stream,
        "%s  SlotTime                      = %.2f\n",
        prefix, this->SlotTime) ;

    fprintf (stream,
        "%s  SlotLength                    = %d\n",
        prefix, this->SlotLength) ;

    fprintf (stream,
        "%s  CurrentTime                   = %d\n",
        prefix, this->CurrentTime) ;

    fprintf (stream,
        "%s  InitialTemperature            = %.2f\n",
        prefix, this->InitialTemperature) ;

    fprintf (stream,
        "%s  InspectionPointListFinal          = %p\n",
        prefix, this->InspectionPointListFinal) ;

    if (this->InspectionPointListFinal != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_inspection_point_list

            (this->InspectionPointListFinal, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
        "%s  InspectionPointListSlot           = %p\n",
        prefix, this->InspectionPointListSlot) ;

    if (this->InspectionPointListSlot != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_inspection_point_list

            (this->InspectionPointListSlot, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
        "%s  InspectionPointListStep           = %p\n",
        prefix, this->InspectionPointListStep) ;

    if (this->InspectionPointListStep != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_inspection_point_list

            (this->InspectionPointListStep, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void add_inspection_point_to_analysis
(
    Analysis_t        *this,
    InspectionPoint_t *inspection_point
)
{
    InspectionPoint_t **list = NULL ;

    if (   this->AnalysisType == TDICE_ANALYSIS_TYPE_STEADY
        || inspection_point->Instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = &this->InspectionPointListFinal ;

    else if (inspection_point->Instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = &this->InspectionPointListSlot ;

    else if (inspection_point->Instant == TDICE_OUTPUT_INSTANT_STEP)

        list = &this->InspectionPointListStep ;

    while (*list != NULL) list = &( (*list)->Next ) ;

    *list = inspection_point ;

    // CHECKME any warning when using the same filename twice?
}

/******************************************************************************/

Error_t generate_analysis_headers
(
    Analysis_t   *this,
    Dimensions_t *dimensions,
    String_t      prefix
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, final, this->InspectionPointListFinal)

        if (generate_inspection_point_header (final, dimensions, prefix) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, slot, this->InspectionPointListSlot)

        if (generate_inspection_point_header (slot, dimensions, prefix) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, step, this->InspectionPointListStep)

        if (generate_inspection_point_header (step, dimensions, prefix) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t generate_analysis_output
(
    Analysis_t      *this,
    Dimensions_t    *dimensions,
    Temperature_t   *temperatures,
    OutputInstant_t  output_instant
)
{
    Time_t current_time = get_simulated_time (this) ;

    InspectionPoint_t *list ;

    if (output_instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = this->InspectionPointListFinal ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_STEP)

        list = this->InspectionPointListStep ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = this->InspectionPointListSlot ;

    else

        return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, ipoint, list)

        if (generate_inspection_point_output (ipoint, dimensions, temperatures, current_time) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

   return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t fill_analysis_message
(
    Analysis_t       *this,
    Dimensions_t     *dimensions,
    Temperature_t    *temperatures,
    OutputInstant_t   output_instant,
    OutputType_t      output_type,
    OutputQuantity_t  output_quantity,
    NetworkMessage_t *message
)
{
    InspectionPoint_t *list ;

    if (output_instant == TDICE_OUTPUT_INSTANT_FINAL)

        list = this->InspectionPointListFinal ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_STEP)

        list = this->InspectionPointListStep ;

    else if (output_instant == TDICE_OUTPUT_INSTANT_SLOT)

        list = this->InspectionPointListSlot ;

    else

        return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (InspectionPoint_t, ipoint, list)

        if (output_type == ipoint->Type)

            fill_message_inspection_point

                (ipoint, output_quantity, dimensions, temperatures, message) ;

   return TDICE_SUCCESS ;
}

/******************************************************************************/
