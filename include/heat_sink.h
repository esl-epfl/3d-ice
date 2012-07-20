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

#ifndef _3DICE_HEAT_SINK_H_
#define _3DICE_HEAT_SINK_H_

/*! \file heat_sink.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"
#include "material.h"

/******************************************************************************/

     /*! \struct HeatSink_t
     *
     *  \brief Structure used to store data about the heat sink on
     *         top of the 2D/3D stack.
     */

    struct HeatSink_t
    {

        /*! The type of the heastink */

        HeatSinkModel_t SinkModel ;

        /*! The number of layer composing the heatsink in the 3d stack */

        CellIndex_t NLayers ;

        /*! The offset (\# layers) of the source layer within the heatsink */

        CellIndex_t SourceLayerOffset ;

        /*! The heat transfert coefficient (from 3d stack to the environment).
            Used for both connection to enviroment and spreader+sink */

        AmbientHTC_t AmbientHTC ;

        /*! The temperarute of the environment in \f$ K \f$ */

        Temperature_t AmbientTemperature ;

        /*! Height of the sink layer */

        CellDimension_t SinkHeight ;

        /*! Area of the sink layer */

        CellDimension_t SinkArea ;

        /*! Height of the spreader layer */

        CellDimension_t SpreaderHeight ;

        /*! Area of the spreader layer */

        CellDimension_t SpreaderArea ;

        /*! Material of the sink layer (used in case of connection to ambient) */

        Material_t SinkMaterial ;

        /*! Material of the spreader layer */

        Material_t SpreaderMaterial ;
    } ;

    /*! Definition of the type HeatSink_t */

    typedef struct HeatSink_t HeatSink_t ;

/******************************************************************************/



    void heat_sink_init (HeatSink_t *hsink) ;

    void heat_sink_copy (HeatSink_t *dst, HeatSink_t *src) ;

    void heat_sink_destroy (HeatSink_t *hsink) ;

    HeatSink_t *heat_sink_calloc (void) ;

    HeatSink_t *heat_sink_clone (HeatSink_t *hsink) ;

    void heat_sink_free (HeatSink_t *hsink) ;

    void heat_sink_print (HeatSink_t *hsink, FILE *stream, String_t prefix) ;


    /*! Returns the equivalent volumetric heat capacity of the material of the
     *  spread layer
     *
     * \param hsink      the pointer to the heat sink
     * \param chip_area the area of the chip
     *
     * \return the equivalent volumetric heat capacity of the spread layer
     * \return \c 0 if the model of the heat sink is not correct
     */

    SolidTC_t get_spreader_volumetric_heat_capacity
    (
        HeatSink_t      *hsink,
        CellDimension_t  chip_area
    ) ;


    /*! Returns the equivalent volumetric heat capacity of the material of the
     *  sink layer
     *
     * \param hsink      the pointer to the heat sink
     * \param chip_area the area of the chip
     *
     * \return the equivalent volumetric heat capacity of the sink layer
     * \return \c 0 if the model of the heat sink is not correct
     */

    SolidTC_t get_sink_volumetric_heat_capacity
    (
        HeatSink_t      *hsink,
        CellDimension_t  chip_area
    ) ;



    /*! Returns the equivalent thermal conductivity of the material of the
     *  spread layer
     *
     * \param hsink      the pointer to the heat sink
     * \param chip_area the area of the chip
     *
     * \return the equivalent thermal conductivity of the spread layer
     * \return \c 0 if the model of the heat sink is not correct
     */

    SolidTC_t get_spreader_thermal_conductivity
    (
        HeatSink_t      *hsink,
        CellDimension_t  chip_area
    ) ;


    /*! Returns the equivalent thermal conductivity of the material of the
     *  sink layer
     *
     * \param hsink      the pointer to the heat sink
     * \param chip_area the area of the chip
     *
     * \return the equivalent thermal conductivity of the sink layer
     * \return \c 0 if the model of the heat sink is not correct
     */

    SolidTC_t get_sink_thermal_conductivity
    (
        HeatSink_t      *hsink,
        CellDimension_t  chip_area
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_HEAT_SINK_H_ */
