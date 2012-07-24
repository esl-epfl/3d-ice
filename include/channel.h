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
     *  \brief Structure used to store data about the channel that compose
     *         the 2D/3D stack.
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

        Material_t WallMaterial ;

    } ;

    /*! Definition of the type Channel_t */

    typedef struct Channel_t Channel_t ;



/******************************************************************************/



    /*! Inits the fields of the \a coolant structure with default values
     *
     * \param coolant the address of the structure to initalize
     */

    void coolant_init    (Coolant_t *coolant) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void coolant_copy    (Coolant_t *dst, Coolant_t *src) ;



    /*! Destroys the content of the fields of the structure \a coolant
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a coolant_init .
     *
     * \param coolant the address of the structure to destroy
     */

    void coolant_destroy (Coolant_t *coolant) ;



    /*! Inits the fields of the \a channel structure with default values
     *
     * \param channel the address of the structure to initalize
     */

    void channel_init    (Channel_t *channel) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure
     * \param src the address of the right term structure
     */

    void channel_copy    (Channel_t *dst, Channel_t *src) ;



    /*! Destroys the content of the fields of the structure \a channel
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a channel_init .
     *
     * \param channel the address of the structure to destroy
     */

    void channel_destroy (Channel_t *channel) ;



    /*! Allocates memory for a structure of type Channel_t
     *
     * The content of the new structure is set to default values
     * calling \a channel_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    Channel_t *channel_calloc ( void ) ;



    /*! Allocates memory for a new copy of the structure \a channel
     *
     * \param channel the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a channel is \c NULL
     */

    Channel_t *channel_clone  (Channel_t *channel) ;



    /*! Frees the memory space pointed by \a channel
     *
     * The function destroys the structure \a channel and then frees
     * its memory. The pointer \a channel must have been returned by
     * a previous call to \a channel_calloc or \a channel_clone .
     *
     * If \a channel is \c NULL, no operation is performed.
     *
     * \param channel the pointer to free
     */

    void channel_free (Channel_t *channel) ;



    /*! Prints the channel declaration as it looks in the stack file
     *
     * \param channel the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     * \param dimensions pointer to the structure storing the dimensions
     */

    void channel_print
    (
        Channel_t    *channel,
        FILE         *stream,
        String_t      prefix,
        Dimensions_t *dimensions
    ) ;



    /*! Returns the convective C term, depending on the type of channel and
     *  the location of the thermal cell
     *
     *  \param channel         pointer to the channel
     *  \param dimensions   pointer to the structure storing the dimensions
     *  \param layer_index  the index of the layer
     *  \param row_index    the index of the row
     *  \param column_index the index of the column
     *
     *  \return \c C , the convective term
     */

    Cconv_t get_convective_term
    (
        Channel_t    *channel,
        Dimensions_t *dimensions,
        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index
    ) ;



    /*! Returns the maximum temperature at the outlet of the channel
     *
     *  \param channel          pointer to the channel
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param temperatures  pointer to the temperature of the first thermal
     *                       cell in the layer where \a channel is placed
     *
     *  \return The maximum temperature at the outlet of the channel
     */

    Temperature_t get_max_temperature_channel_outlet
    (
        Channel_t     *channel,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures
    ) ;



    /*! Returns the minimum temperature at the outlet of the channel
     *
     *  \param channel          pointer to the channel
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param temperatures  pointer to the temperature of the first thermal
     *                       cell in the layer where the channel is placed
     *
     *  \return The minimum temperature at the outlet of the channel
     */

    Temperature_t get_min_temperature_channel_outlet
    (
        Channel_t     *channel,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures
    ) ;



    /*! Returns the average temperature at the outlet of the channel
     *
     *  \param channel          pointer to the channel
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param temperatures  pointer to the temperature of the first thermal
     *                       cell in the layer where \a channel is placed
     *
     *  \return The average temperature at the outlet of the channel
     */

    Temperature_t get_avg_temperature_channel_outlet
    (
        Channel_t     *channel,
        Dimensions_t  *dimensions,
        Temperature_t *temperatures
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CHANNEL_H_ */
