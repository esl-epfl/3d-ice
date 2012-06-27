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

#ifndef _3DICE_CHANNEL_H_
#define _3DICE_CHANNEL_H_

/*! \file channel.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "material.h"

/******************************************************************************/

    /*! \struct Channel_t
     *
     *  \brief Structure used to store data about the channel that compose the 2D/3D stack.
     *
     *  Channel is one of the elements that can be used to build a 3d stack
     */

    struct Channel_t
    {
        /*! The channel model (4rm - 2rm) */

        ChannelModel_t ChannelModel ;

        /*! The height of the channel in \f$ \mu m \f$ (1 cell) */

        CellDimension_t Height ;

        /*! The length of the channel in \f$ \mu m \f$ */

        ChannelDimension_t Length ;

        /*! The pitch of the channel in \f$ \mu m \f$ */

        ChannelDimension_t Pitch ;

        /*! Porosity */

        ChannelDimension_t Porosity ;

        /*!  Number of channels (per cavity) along chip length */

        Quantity_t NChannels ;

        /*! The number of layer composing the channel */

        CellIndex_t NLayers ;

        /*! The offset (\# layers) of the source layer within the channel */

        CellIndex_t SourceLayerOffset ;

        /*! The properties of the fluid used as coolant */

        Coolant_t Coolant ;

        /*! The material composing the wall */

        Material_t *WallMaterial ;

    } ;

    /*! Definition of the type Channel_t */

    typedef struct Channel_t Channel_t ;

/******************************************************************************/



    /*! Sets all the fields of \a coolant to a default value.
     *
     * \param this the address of the coolant structure to initialize
     */

    void init_coolant (Coolant_t *this) ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the channel structure to initialize
     */

    void init_channel (Channel_t *this) ;



    /*! Allocates and inits memory for a structure of type Channel_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Channel_t *calloc_channel ( void ) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_channel . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_channel (Channel_t *this) ;



    /*! Prints the channel as it looks in the stack file
     *
     * \param this    the channel to print
     * \param stream  the output stream (must be already open)
     * \param prefix  a string to be printed as prefix at the beginning of each line
     * \param dimensions pointer to the structure storing the dimensions
     */

    void print_channel

        (Channel_t *this, FILE *stream, String_t prefix, Dimensions_t *dimensions) ;



    /*! Returns the convective C term, depending on the type of channel and
     *  the location of the thermal cell
     *
     *  \param this         pointer to the channel
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param layer_index  the index of the layer
     *  \param row_index    the index of the row
     *  \param column_index the index of the column
     *
     *  \return \c C , the convective term
     */

    Cconv_t get_convective_term
    (
        Channel_t    *this,
        Dimensions_t *dimensions,
        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index
    ) ;



    /*! Returns the maximum temperature at the outlet of the channel
     *
     *  \param this          pointer to the channel
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param temperatures  pointer to the temperature of the first thermal
     *                       cell in the layer where \a channel is placed
     *
     *  \return The maximum temperature at the outlet of the channel
     */

    Temperature_t get_max_temperature_channel_outlet

        (Channel_t *this, Dimensions_t *dimensions, Temperature_t *temperatures) ;



    /*! Returns the minimum temperature at the outlet of the channel
     *
     *  \param this          pointer to the channel
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param temperatures  pointer to the temperature of the first thermal
     *                       cell in the layer where the channel is placed
     *
     *  \return The minimum temperature at the outlet of the channel
     */

    Temperature_t get_min_temperature_channel_outlet

        (Channel_t *this, Dimensions_t *dimensions, Temperature_t *temperatures) ;



    /*! Returns the average temperature at the outlet of the channel
     *
     *  \param this          pointer to the channel
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param temperatures  pointer to the temperature of the first thermal
     *                       cell in the layer where \a channel is placed
     *
     *  \return The average temperature at the outlet of the channel
     */

    Temperature_t get_avg_temperature_channel_outlet

        (Channel_t *this, Dimensions_t *dimensions, Temperature_t *temperatures) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CHANNEL_H_ */
