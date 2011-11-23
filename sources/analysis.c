/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
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

void init_analysis (Analysis *analysis)
{
    analysis->AnalysisType         = ANALYSISTYPE_I;
    analysis->StepTime             = 0.0 ;
    analysis->SlotTime             = 0.0 ;
    analysis->SlotLength           = 0u ;
    analysis->CurrentTime          = 0u ;
    analysis->InitialTemperature   = 0.0 ;
    analysis->PrintOutputListFinal = NULL ;
    analysis->PrintOutputListSlot  = NULL ;
    analysis->PrintOutputListStep  = NULL ;
}

/******************************************************************************/

Analysis *alloc_and_init_analysis (void)
{
    Analysis *analysis = (Analysis *) malloc (sizeof(Analysis)) ;

    if (analysis != NULL)

        init_analysis (analysis) ;

    return analysis ;
}

/******************************************************************************/

void free_analysis (Analysis *analysis)
{
    FREE_LIST (PrintOutput, analysis->PrintOutputListFinal, free_print_output) ;
    FREE_LIST (PrintOutput, analysis->PrintOutputListSlot, free_print_output) ;
    FREE_LIST (PrintOutput, analysis->PrintOutputListStep, free_print_output) ;
}

/******************************************************************************/

double get_simulated_time (Analysis *analysis)
{
  return analysis->CurrentTime * analysis->StepTime ;
}

/******************************************************************************/

bool slot_completed (Analysis *analysis)
{
  if (analysis->AnalysisType == TDICE_STEADY)

    return FALSE_V ;

  if (analysis->CurrentTime % analysis->SlotLength == 0)

    return TRUE_V ;

  return FALSE_V ;
}

/******************************************************************************/

void print_formatted_analysis
(
    FILE     *stream,
    char     *prefix,
    Analysis *analysis
)
{
    fprintf (stream, "%ssolver : ", prefix) ;

    if (analysis->AnalysisType == TDICE_STEADY)

        fprintf (stream, "steady ;\n") ;

    else

        fprintf (stream, "transient step %.2f, slot %.2f ;\n",
            analysis->StepTime, analysis->SlotTime) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream, "%sinitial temperature  %.2f ;\n",
        prefix, analysis->InitialTemperature) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream, "%soutput :\n", prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_print_output_list

        (stream, prefix, analysis->PrintOutputListFinal) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_print_output_list

        (stream, prefix, analysis->PrintOutputListSlot) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_print_output_list

        (stream, prefix, analysis->PrintOutputListStep) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

void print_detailed_analysis
(
  FILE     *stream,
  char     *prefix,
  Analysis *analysis
)
{
    char *new_prefix = malloc (sizeof(char) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
        "%sAnalysis                        = %p\n",
        prefix, analysis) ;

    fprintf (stream,
        "%s  AnalysisType                  = %d\n",
        prefix, analysis->AnalysisType) ;

    fprintf (stream,
        "%s  StepTime                      = %.2f\n",
        prefix, analysis->StepTime) ;

    fprintf (stream,
        "%s  SlotTime                      = %.2f\n",
        prefix, analysis->SlotTime) ;

    fprintf (stream,
        "%s  SlotLength                    = %d\n",
        prefix, analysis->SlotLength) ;

    fprintf (stream,
        "%s  CurrentTime                   = %d\n",
        prefix, analysis->CurrentTime) ;

    fprintf (stream,
        "%s  InitialTemperature            = %.2f\n",
        prefix, analysis->InitialTemperature) ;

    fprintf (stream,
        "%s  PrintOutputListFinal          = %p\n",
        prefix, analysis->PrintOutputListFinal) ;

    if (analysis->PrintOutputListFinal != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_print_output_list

            (stream, new_prefix, analysis->PrintOutputListFinal) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
        "%s  PrintOutputListSlot           = %p\n",
        prefix, analysis->PrintOutputListSlot) ;

    if (analysis->PrintOutputListSlot != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_print_output_list

            (stream, new_prefix, analysis->PrintOutputListSlot) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
        "%s  PrintOutputListStep           = %p\n",
        prefix, analysis->PrintOutputListStep) ;

    if (analysis->PrintOutputListStep != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_print_output_list

            (stream, new_prefix, analysis->PrintOutputListStep) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void add_print_output_to_analysis
(
    Analysis    *analysis,
    PrintOutput *print_output
)
{
    PrintOutput **list = NULL ;

    if (   analysis->AnalysisType == TDICE_STEADY
        || print_output->InstanceType == TDICE_OUTPUT_FINAL)

        list = &analysis->PrintOutputListFinal ;

    else if (print_output->InstanceType==TDICE_OUTPUT_SLOT)

        list = &analysis->PrintOutputListSlot ;

    else if (print_output->InstanceType==TDICE_OUTPUT_STEP)

        list = &analysis->PrintOutputListStep ;

    while (*list != NULL) list = &( (*list)->Next ) ;

    *list = print_output ;
}

/******************************************************************************/

Error_t initialize_print_output_instructions (Analysis* analysis, StackElement *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (PrintOutput, final, analysis->PrintOutputListFinal)

        if (initialize_print_output (final, list) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (PrintOutput, slot, analysis->PrintOutputListSlot)

        if (initialize_print_output (slot, list) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (PrintOutput, step, analysis->PrintOutputListStep)

        if (initialize_print_output (step, list) != TDICE_SUCCESS)

            return TDICE_FAILURE ;

//   FIXME
//
//   if (   there_is_tmap_in_list (analysis->PrintOutputListFinal)
//       || there_is_tmap_in_list (analysis->PrintOutputListSlot)
//       || there_is_tmap_in_list (analysis->PrintOutputListStep))
//
//     print_axes (analysis) ;

   return TDICE_SUCCESS ;
}

/******************************************************************************/
