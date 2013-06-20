/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#include <math.h>   // Fo the math function sqrt
#include <stdlib.h> // For the memory functions malloc/free

#include "heat_sink.h"
#include "macros.h"

/******************************************************************************/

void heat_sink_init (HeatSink_t *hsink)
{
    hsink->SinkModel          = TDICE_HEATSINK_NONE ;
    hsink->AmbientHTC         = (AmbientHTC_t) 0.0 ;
    hsink->AmbientTemperature = (Temperature_t) 0.0 ;
}

/******************************************************************************/

void heat_sink_copy (HeatSink_t *dst, HeatSink_t *src)
{
    heat_sink_destroy (dst) ;

    dst->SinkModel          = src->SinkModel ;
    dst->AmbientHTC         = src->AmbientHTC ;
    dst->AmbientTemperature = src->AmbientTemperature ;
}

/******************************************************************************/

void heat_sink_destroy (HeatSink_t *hsink)
{
    // Nothing to do ...

    heat_sink_init (hsink) ;
}

/******************************************************************************/

HeatSink_t *heat_sink_calloc (void)
{
    HeatSink_t *hsink = (HeatSink_t *) malloc (sizeof(HeatSink_t)) ;

    if (hsink != NULL)

        heat_sink_init (hsink) ;

    return hsink ;
}

/******************************************************************************/

HeatSink_t *heat_sink_clone (HeatSink_t *hsink)
{
    if (hsink == NULL)

        return NULL ;

    HeatSink_t *newh = heat_sink_calloc ( ) ;

    if (newh != NULL)

        heat_sink_copy (newh, hsink) ;

    return newh ;
}

/******************************************************************************/

void heat_sink_free (HeatSink_t *hsink)
{
    if (hsink == NULL)

        return ;

    heat_sink_destroy (hsink) ;

    free (hsink) ;
}

/******************************************************************************/

Conductance_t heat_sink_conductance
(
    HeatSink_t    *hsink,
    Dimensions_t  *dimensions,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    return    hsink->AmbientHTC
            * get_cell_length (dimensions, column_index)
            * get_cell_width  (dimensions, row_index) ;
}

/******************************************************************************/

void heat_sink_print (HeatSink_t *hsink, FILE *stream, String_t prefix)
{
    if (hsink->SinkModel == TDICE_HEATSINK_TOP)

        fprintf (stream, "%stop heat sink :\n", prefix) ;

    else if (hsink->SinkModel == TDICE_HEATSINK_BOTTOM)

        fprintf (stream, "%sbottom heat sink :\n", prefix) ;

    else
    {
        fprintf (stream, "wrong heat sink model\n") ;

        return ;
    }

    fprintf (stream,
        "%s   heat transfer coefficient %.4e ;\n",
        prefix, hsink->AmbientHTC) ;

    fprintf (stream,
        "%s   temperature               %.2f ;\n",
        prefix, hsink->AmbientTemperature) ;
}

/******************************************************************************/
