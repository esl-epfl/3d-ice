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
#include <stdbool.h>

#include "analysis.h"
#include "macros.h"

/******************************************************************************/

void init_analysis (Analysis *analysis)
{
    analysis->AnalysisType        = ANALYSISTYPE_I;
    analysis->StepTime            = 0.0 ;
    analysis->SlotTime            = 0.0 ;
    analysis->SlotLength          = 0u ;
    analysis->CurrentTime         = 0u ;
    analysis->InitialTemperature  = 0.0 ;
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
    FREE_POINTER (free, analysis) ;
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

    fprintf (stream, "\n%sinitial tempreature  %.2f ;\n",
        prefix, analysis->InitialTemperature) ;
}

/******************************************************************************/

void print_detailed_analysis
(
  FILE     *stream,
  char     *prefix,
  Analysis *analysis
)
{
    fprintf (stream,
        "%sAnalysis                    = %p\n",
        prefix, analysis) ;

    fprintf (stream,
        "%s  AnalysisType              = %d\n",
        prefix, analysis->AnalysisType) ;

    fprintf (stream,
        "%s  StepTime                  = %.2f\n",
        prefix, analysis->StepTime) ;

    fprintf (stream,
        "%s  SlotTime                  = %.2f\n",
        prefix, analysis->SlotTime) ;

    fprintf (stream,
        "%s  InitialTemperature        = %.2f\n",
        prefix, analysis->InitialTemperature) ;
}


/******************************************************************************/
