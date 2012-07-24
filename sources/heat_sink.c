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
#include <math.h>

#include "heat_sink.h"
#include "macros.h"

/******************************************************************************/

void heat_sink_init (HeatSink_t *hsink)
{
    hsink->SinkModel          = TDICE_HEATSINK_MODEL_NONE ;
    hsink->NLayers            = (CellIndex_t) 0u ;
    hsink->SourceLayerOffset  = (CellIndex_t) 0u ;
    hsink->AmbientHTC         = (AmbientHTC_t) 0.0 ;
    hsink->AmbientTemperature = (Temperature_t) 0.0 ;
    hsink->SinkHeight         = (CellDimension_t) 0.0 ;
    hsink->SinkArea           = (CellDimension_t) 0.0 ;
    hsink->SpreaderHeight     = (CellDimension_t) 0.0 ;
    hsink->SpreaderArea       = (CellDimension_t) 0.0 ;

    material_init (&hsink->SinkMaterial) ;
    material_init (&hsink->SpreaderMaterial) ;
}

/******************************************************************************/

void heat_sink_copy (HeatSink_t *dst, HeatSink_t *src)
{
    heat_sink_destroy (dst) ;

    dst->SinkModel          = src->SinkModel ;
    dst->NLayers            = src->NLayers ;
    dst->SourceLayerOffset  = src->SourceLayerOffset ;
    dst->AmbientHTC         = src->AmbientHTC ;
    dst->AmbientTemperature = src->AmbientTemperature ;
    dst->SinkHeight         = src->SinkHeight ;
    dst->SinkArea           = src->SinkArea ;
    dst->SpreaderHeight     = src->SpreaderHeight ;
    dst->SpreaderArea       = src->SpreaderArea ;

    material_copy (&dst->SinkMaterial,     &src->SinkMaterial) ;
    material_copy (&dst->SpreaderMaterial, &src->SpreaderMaterial) ;
}

/******************************************************************************/

void heat_sink_destroy (HeatSink_t *hsink)
{
    material_destroy (&hsink->SinkMaterial) ;
    material_destroy (&hsink->SpreaderMaterial) ;

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

void heat_sink_print (HeatSink_t *hsink, FILE *stream, String_t prefix)
{
    if (hsink->SinkModel == TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT)

        fprintf (stream, "%sconnection to ambient :\n", prefix) ;

    else if (hsink->SinkModel == TDICE_HEATSINK_MODEL_TRADITIONAL)
    {
        fprintf (stream, "%sheat sink :\n", prefix) ;

        fprintf (stream,
            "%s   sink     height %.2f, area %.1e, material %s ;\n",
            prefix,
            hsink->SinkHeight,
            hsink->SinkArea,
            hsink->SinkMaterial.Id) ;

        fprintf (stream,
            "%s   spreader height %.2f, area %.1e, material %s ;\n",
            prefix,
            hsink->SpreaderHeight,
            hsink->SpreaderArea,
            hsink->SpreaderMaterial.Id) ;
    }

    fprintf (stream,
        "%s   heat transfer coefficient %.4e ;\n",
        prefix, hsink->AmbientHTC) ;

    fprintf (stream,
        "%s   ambient temperature       %.2f ;\n",
        prefix, hsink->AmbientTemperature) ;
}

/******************************************************************************/

SolidTC_t get_spreader_volumetric_heat_capacity
(
    HeatSink_t      *hsink,
    CellDimension_t  chip_area
)
{
    if (hsink->SinkModel != TDICE_HEATSINK_MODEL_TRADITIONAL)
    {
        fprintf (stderr,
            "Cannot compute spreader volumetric heat capacity (bad model)\n") ;

        return 0.0 ;
    }

    return hsink->SpreaderMaterial.VolumetricHeatCapacity
           * (hsink->SpreaderArea / chip_area) ;
}

/******************************************************************************/

SolidTC_t get_sink_volumetric_heat_capacity
(
    HeatSink_t      *hsink,
    CellDimension_t  chip_area
)
{
    if (hsink->SinkModel != TDICE_HEATSINK_MODEL_TRADITIONAL)
    {
        fprintf (stderr,
            "Cannot compute sink volumetric heat capacity (bad model)\n") ;

        return 0.0 ;
    }

    return hsink->SinkMaterial.VolumetricHeatCapacity
           * (hsink->SinkArea / chip_area) ;
}

/******************************************************************************/

SolidTC_t get_spreader_thermal_conductivity
(
    HeatSink_t      *hsink,
    ChipDimension_t  chip_area
)
{
    if (hsink->SinkModel != TDICE_HEATSINK_MODEL_TRADITIONAL)
    {
        fprintf (stderr,
            "Cannot compute spreader thermal conductivity (bad model)\n") ;

        return 0.0 ;
    }

    // FIXME: check varible types (all doubles so far...)

    // To get the equivalent thermal conductivity, some values coming from
    // the sink are needed ...

    CellDimension_t e_sink = sqrt (hsink->SpreaderArea / hsink->SinkArea) ;

    CellDimension_t tau_sink = hsink->SinkHeight / sqrt (hsink->SinkArea / PI) ;

    SolidTC_t Bi_sink =   (hsink->AmbientHTC * sqrt (hsink->SinkArea / PI))

                        / hsink->SinkMaterial.ThermalConductivity ;

    SolidTC_t lambda_sink = PI + (1.0 / (sqrt (PI) * e_sink)) ;

    SolidTC_t phi_sink = (tanh (lambda_sink * tau_sink) + (lambda_sink / Bi_sink))
                         /
                         (1.0 + (lambda_sink / Bi_sink) * tanh (lambda_sink * tau_sink)) ;

    SolidTC_t g_sink = (  2.0
                        * hsink->SinkMaterial.ThermalConductivity
                        * sqrt(hsink->SpreaderArea)
                       )
                       /
                       (pow(1.0 - e_sink, 1.5) * phi_sink) ;

    SolidTC_t gm_sink = hsink->SinkMaterial.ThermalConductivity

                      * (hsink->SinkArea / hsink->SinkHeight) ;

    // Values just for the spreader

    CellDimension_t e   = sqrt (chip_area / hsink->SpreaderArea) ;

    CellDimension_t tau = hsink->SpreaderHeight / sqrt (hsink->SpreaderArea / PI) ;

    AmbientHTC_t R_air  = 1.0 / (hsink->AmbientHTC * hsink->SinkArea) ;

    AmbientHTC_t R_sink = 1.0 / PARALLEL (g_sink, gm_sink) ;

    AmbientHTC_t H_sink =   (1.0 / (R_air + R_sink))

                          * (1.0 / hsink->SpreaderArea) ;

    SolidTC_t Bi =   (H_sink * sqrt (hsink->SpreaderArea / PI))

                   / hsink->SpreaderMaterial.ThermalConductivity ;

    SolidTC_t lambda = ( PI + (1.0 / (sqrt (PI) * e)) ) ;

    SolidTC_t phi = (tanh (lambda * tau) + (lambda / Bi))
                    /
                    (1.0 + (lambda / Bi) * tanh (lambda * tau)) ;

    SolidTC_t g = (  2.0
                   * hsink->SpreaderMaterial.ThermalConductivity
                   * sqrt(chip_area)
                  )
                  /
                  (pow(1.0 - e, 1.5) * phi) ;

    SolidTC_t gm = hsink->SpreaderMaterial.ThermalConductivity

                   * (hsink->SpreaderArea / hsink->SpreaderHeight) ;

    return PARALLEL (g, gm) * (hsink->SpreaderHeight / chip_area) ;
}

/******************************************************************************/

SolidTC_t get_sink_thermal_conductivity
(
    HeatSink_t      *hsink,
    ChipDimension_t  chip_area
)
{
    if (hsink->SinkModel != TDICE_HEATSINK_MODEL_TRADITIONAL)
    {
        fprintf (stderr,
            "Cannot compute spreader thermal conductivity (bad model)\n") ;

        return 0.0 ;
    }

    // FIXME: check varible types (all doubles so far...)

    CellDimension_t e   = sqrt (hsink->SpreaderArea / hsink->SinkArea) ;

    CellDimension_t tau = hsink->SinkHeight / sqrt (hsink->SinkArea / PI) ;

    SolidTC_t Bi =   (hsink->AmbientHTC * sqrt (hsink->SinkArea / PI))

                   / hsink->SinkMaterial.ThermalConductivity ;

    SolidTC_t lambda = ( PI + (1.0 / (sqrt (PI) * e)) ) ;

    SolidTC_t phi = ( (tanh (lambda * tau) + (lambda / Bi))
                    /
                    (1.0 + (lambda / Bi) * tanh (lambda * tau)) ) ;

    SolidTC_t g = (  2.0
                   * hsink->SinkMaterial.ThermalConductivity
                   * sqrt(hsink->SpreaderArea)
                  )
                  /
                  (pow(1.0 - e, 1.5) * phi) ;

    SolidTC_t gm = hsink->SinkMaterial.ThermalConductivity

                   * (hsink->SinkArea / hsink->SinkHeight) ;

    return PARALLEL (g, gm) * (hsink->SinkHeight / chip_area) ;
}
