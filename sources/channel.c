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

#include "channel.h"
#include "macros.h"

void init_coolant (Coolant_t *coolant)
{
    coolant->HTCSide       = 0.0 ;
    coolant->HTCTop        = 0.0 ;
    coolant->HTCBottom     = 0.0 ;
    coolant->VHC           = 0.0 ;
    coolant->FlowRate      = 0.0 ;
    coolant->DarcyVelocity = 0.0 ;
    coolant->TIn           = 0.0 ;
}

/******************************************************************************/

void copy_coolant (Coolant_t *dest, Coolant_t *src)
{
    dest->HTCSide       = src->HTCSide ;
    dest->HTCTop        = src->HTCTop ;
    dest->HTCBottom     = src->HTCBottom ;
    dest->VHC           = src->VHC ;
    dest->FlowRate      = src->FlowRate ;
    dest->DarcyVelocity = src->DarcyVelocity ;
    dest->TIn           = src->TIn ;
}

/******************************************************************************/

void init_channel (Channel_t *channel)
{
    channel->ChannelModel          = TDICE_CHANNEL_MODEL_NONE ;
    channel->Height                = 0.0 ;
    channel->Length                = 0.0 ;
    channel->Pitch                 = 0.0 ;
    channel->Porosity              = 0.0 ;
    channel->NChannels             = 0u ;
    channel->NLayers               = 0u ;
    channel->SourceLayerOffset     = 0u ;

    init_coolant ( &channel->Coolant ) ;

    channel->WallMaterial          = NULL ;
}

/******************************************************************************/

Channel_t *alloc_and_init_channel (void)
{
    Channel_t *channel = (Channel_t *) malloc (sizeof(Channel_t)) ;

    if (channel != NULL)

        init_channel (channel) ;

    return channel ;
}

/******************************************************************************/

void free_channel (Channel_t *channel)
{
    FREE_POINTER (free, channel) ;
}

/******************************************************************************/

void print_formatted_channel
(
    FILE         *stream,
    String_t      prefix,
    Channel_t    *channel,
    Dimensions_t *dimensions
)
{
    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_4RM)
    {
        fprintf (stream, "%smicrochannel 4rm :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   channel    length %7.1f ;\n", prefix, dimensions->Cell.ChannelLength) ;
        fprintf (stream, "%s   wall       length %7.1f ;\n", prefix, dimensions->Cell.WallLength) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   first wall length %7.1f ;\n", prefix, dimensions->Cell.FirstWallLength) ;
        fprintf (stream, "%s    last wall length %7.1f ;\n", prefix, dimensions->Cell.LastWallLength) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   wall material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant flow rate  %.2f ;\n", prefix, FLOW_RATE_FROM_UM3SEC_TO_MLMIN(channel->Coolant.FlowRate)) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant heat transfer coefficient side   %.4e ,\n", prefix, channel->Coolant.HTCSide) ;
        fprintf (stream, "%s                                     top    %.4e ,\n", prefix, channel->Coolant.HTCTop) ;
        fprintf (stream, "%s                                     bottom %.4e ;\n",  prefix, channel->Coolant.HTCBottom) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
    {
        fprintf (stream, "%smicrochannel 2rm :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   channel    length %7.1f ;\n", prefix, channel->Length) ;
        fprintf (stream, "%s   wall       length %7.1f ;\n", prefix, channel->Pitch - channel->Length) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   wall material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant flow rate  %.2f ;\n", prefix, FLOW_RATE_FROM_UM3SEC_TO_MLMIN(channel->Coolant.FlowRate)) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant heat transfer coefficient top    %.4e ,\n", prefix, channel->Coolant.HTCTop) ;
        fprintf (stream, "%s                                     bottom %.4e ;\n",  prefix, channel->Coolant.HTCBottom) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE)
    {
        fprintf (stream, "%spinfin :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin diameter %7.1f ;\n",   prefix, DIAMETER(channel->Porosity, channel->Pitch)) ;
        fprintf (stream, "%s   pin pitch    %7.1f ;\n", prefix, channel->Pitch) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin distribution inline ;\n", prefix) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   darcy velocity  %.4e ;\n", prefix, channel->Coolant.DarcyVelocity) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
    {
        fprintf (stream, "%spinfin :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin diameter %7.1f ;\n",   prefix, DIAMETER(channel->Porosity, channel->Pitch)) ;
        fprintf (stream, "%s   pin pitch    %7.1f ;\n", prefix, channel->Pitch) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin distribution staggered ;\n", prefix) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   darcy velocity  %.4e ;\n", prefix, channel->Coolant.DarcyVelocity) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }
}

/******************************************************************************/

void print_detailed_channel
(
  FILE      *stream,
  String_t   prefix,
  Channel_t *channel
)
{
    fprintf (stream,
        "%schannel                     = %p\n",
        prefix,   channel) ;

    fprintf (stream,
        "%s  ChannelModel              = %d\n",
        prefix, channel->ChannelModel) ;

    fprintf (stream,
        "%s  Height                    = %.1f\n",
        prefix,  channel->Height) ;

    fprintf (stream,
        "%s  Length                    = %.1f\n",
        prefix,  channel->Length) ;

    fprintf (stream,
        "%s  Pitch                     = %.1f\n",
        prefix,  channel->Pitch) ;

    fprintf (stream,
        "%s  Porosity                  = %.1f\n",
        prefix,  channel->Porosity) ;

    fprintf (stream,
        "%s  NChannels                 = %d\n",
        prefix,  channel->NChannels) ;

    fprintf (stream,
        "%s  NLayers                   = %d\n",
        prefix,  channel->NLayers) ;

    fprintf (stream,
        "%s  SourceLayerOffset         = %d\n",
        prefix, channel->SourceLayerOffset) ;

    fprintf (stream,
        "%s  Coolant.HTCSide           = %.4e\n",
        prefix,  channel->Coolant.HTCSide) ;

    fprintf (stream,
        "%s  Coolant.HTCTop            = %.4e\n",
        prefix,  channel->Coolant.HTCTop) ;

    fprintf (stream,
        "%s  Coolant.HTCBottom         = %.4e\n",
        prefix,  channel->Coolant.HTCBottom) ;

    fprintf (stream,
        "%s  Coolant.VHC               = %.4e\n",
        prefix,  channel->Coolant.VHC) ;

    fprintf (stream,
        "%s  Coolant.TIn               = %.2f\n",
        prefix,  channel->Coolant.TIn) ;

    fprintf (stream,
        "%s  CoolantFR                 = %.4e\n",
        prefix,  channel->Coolant.FlowRate) ;

    fprintf (stream,
        "%s  DarcyVelocity             = %.4e\n",
        prefix,  channel->Coolant.DarcyVelocity) ;

    fprintf (stream,
        "%s  WallMaterial              = %p\n",
        prefix,  channel->WallMaterial) ;
}

/******************************************************************************/

Cconv_t get_convective_term
(
    Channel_t    *channel,
    Dimensions_t *dimensions,
    CellIndex_t   layer_index,
    CellIndex_t   __attribute__ ((unused)) row_index,
    CellIndex_t   column_index
)
{
    Cconv_t C = 0.0 ;

    switch (channel->ChannelModel)
    {
        case TDICE_CHANNEL_MODEL_MC_4RM :

            C = CCONV_MC_4RM

                (channel->NChannels, channel->Coolant.VHC,
                 channel->Coolant.FlowRate);

            break ;

        case TDICE_CHANNEL_MODEL_MC_2RM :

            C = CCONV_MC_2RM

                (channel->NChannels,        channel->Coolant.VHC,
                 channel->Coolant.FlowRate, channel->Porosity,
                 get_cell_length (dimensions, column_index),
                 channel->Length) ;

            break ;

        case TDICE_CHANNEL_MODEL_PF_INLINE :
        case TDICE_CHANNEL_MODEL_PF_STAGGERED :

            C = CCONV_PF

                (channel->Coolant.VHC, channel->Coolant.DarcyVelocity,
                 get_cell_length (dimensions, column_index),
                 get_cell_height (dimensions, layer_index));

            break ;

        case TDICE_CHANNEL_MODEL_NONE :

            fprintf (stderr, "WARNING: unsert channel model\n") ;

            break ;

        default :

            fprintf (stderr, "ERROR: unknown channel model %d\n",
                channel->ChannelModel) ;
    }

    return C ;
}

/******************************************************************************/

Temperature_t get_max_temperature_channel_outlet
(
    Channel_t     *channel,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t max = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            max = MAX (max, *temperatures) ;

        temperatures++ ;
    }

    return max ;
}

/******************************************************************************/

Temperature_t get_min_temperature_channel_outlet
(
    Channel_t     *channel,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t min = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            min = MIN (min, *temperatures) ;

        temperatures++ ;
    }

    return min ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_channel_outlet
(
    Channel_t     *channel,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t avg = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            avg += *temperatures ;

        temperatures++ ;
    }

    return avg / (Temperature_t) channel->NChannels ;
}

/******************************************************************************/
