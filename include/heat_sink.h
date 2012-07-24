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

        /*! Material of the sink layer (used in case of connection
         *  to ambient) */

        Material_t SinkMaterial ;

        /*! Material of the spreader layer */

        Material_t SpreaderMaterial ;
    } ;

    /*! Definition of the type HeatSink_t */

    typedef struct HeatSink_t HeatSink_t ;



/******************************************************************************/



    /*! Inits the fields of the \a hsink structure with default values
     *
     * \param hsink the address of the structure to initalize
     */

    void heat_sink_init (HeatSink_t *hsink) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void heat_sink_copy (HeatSink_t *dst, HeatSink_t *src) ;



    /*! Destroys the content of the fields of the structure \a hsink
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a heat_sink_init .
     *
     * \param hsink the address of the structure to destroy
     */

    void heat_sink_destroy (HeatSink_t *hsink) ;



    /*! Allocates memory for a structure of type HeatSink_t
     *
     * The content of the new structure is set to default values
     * calling \a heat_sink_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    HeatSink_t *heat_sink_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a hsink
     *
     * \param hsink the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a hsink is \c NULL
     */

    HeatSink_t *heat_sink_clone (HeatSink_t *hsink) ;



    /*! Frees the memory space pointed by \a hsink
     *
     * The function destroys the structure \a hsink and then frees
     * its memory. The pointer \a hsink must have been returned by
     * a previous call to \a heat_sink_calloc or \a heat_sink_clone .
     *
     * If \a hsink is \c NULL, no operation is performed.
     *
     * \param hsink the pointer to free
     */

    void heat_sink_free (HeatSink_t *hsink) ;



    /*! Prints the heat sink declaration as it looks in the stack file
     *
     * \param hsink the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

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
