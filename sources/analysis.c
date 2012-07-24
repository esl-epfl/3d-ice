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

#include "analysis.h"
#include "macros.h"

/******************************************************************************/

void analysis_init (Analysis_t *analysis)
{
    analysis->AnalysisType       = (AnalysisType_t) TDICE_ANALYSIS_TYPE_NONE ;
    analysis->StepTime           = (Time_t) 0.0 ;
    analysis->SlotTime           = (Time_t) 0.0 ;
    analysis->SlotLength         = (Quantity_t) 0u ;
    analysis->CurrentTime        = (Quantity_t) 0u ;
    analysis->InitialTemperature = (Temperature_t) 0.0 ;
}

/******************************************************************************/

void analysis_copy (Analysis_t *dst, Analysis_t *src)
{
    analysis_destroy (dst) ;

    dst->AnalysisType       = src->AnalysisType ;
    dst->StepTime           = src->StepTime ;
    dst->SlotTime           = src->SlotTime ;
    dst->SlotLength         = src->SlotLength ;
    dst->CurrentTime        = src->CurrentTime ;
    dst->InitialTemperature = src->InitialTemperature ;
}

/******************************************************************************/

void analysis_destroy (Analysis_t *analysis)
{
    // Nothing to do ...

    analysis_init (analysis) ;
}

/******************************************************************************/

Analysis_t *analysis_calloc ( void )
{
    Analysis_t *analysis = (Analysis_t *) malloc (sizeof (Analysis_t)) ;

    if (analysis != NULL)

        analysis_init (analysis) ;

    return analysis ;
}

/******************************************************************************/

Analysis_t *analysis_clone (Analysis_t *analysis)
{
    if (analysis == NULL)

        return NULL ;

    Analysis_t *newa = analysis_calloc ( ) ;

    if (newa != NULL)

        analysis_copy (newa, analysis) ;

    return newa ;
}

/******************************************************************************/

void analysys_free (Analysis_t *analysis)
{
    if (analysis == NULL)

        return ;

    analysis_destroy (analysis) ;

    free (analysis) ;
}

/******************************************************************************/

void analysis_print (Analysis_t *analysis, FILE *stream, String_t prefix)
{
    fprintf (stream, "%ssolver : \n", prefix) ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_STEADY)

        fprintf (stream, "  steady ;\n") ;

    else

        fprintf (stream, "  transient step %.2f, slot %.2f ;\n",
            analysis->StepTime, analysis->SlotTime) ;

    fprintf (stream, "%s  initial temperature  %.2f ;\n",
        prefix, analysis->InitialTemperature) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

Time_t get_simulated_time (Analysis_t *analysis
)
{
  return analysis->CurrentTime * analysis->StepTime ;
}

/******************************************************************************/

void increase_by_step_time (Analysis_t *analysis)
{
    analysis->CurrentTime++ ;
}

/******************************************************************************/

bool slot_completed (Analysis_t *analysis)
{
    if (analysis->CurrentTime % analysis->SlotLength == 0u)

        return true ;

    return false ;
}

/******************************************************************************/
