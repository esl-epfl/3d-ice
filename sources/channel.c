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

void init_coolant (Coolant_t *this)
{
    this->HTCSide       = (CoolantHTC_t) 0.0 ;
    this->HTCTop        = (CoolantHTC_t) 0.0 ;
    this->HTCBottom     = (CoolantHTC_t) 0.0 ;
    this->VHC           = (CoolantVHC_t) 0.0 ;
    this->FlowRate      = (CoolantFR_t) 0.0 ;
    this->DarcyVelocity = (DarcyVelocity_t) 0.0 ;
    this->TIn           = (Temperature_t) 0.0 ;
}

/******************************************************************************/

void copy_coolant (Coolant_t *dst, Coolant_t *src)
{
    dst->HTCSide       = src->HTCSide ;
    dst->HTCTop        = src->HTCTop ;
    dst->HTCBottom     = src->HTCBottom ;
    dst->VHC           = src->VHC ;
    dst->FlowRate      = src->FlowRate ;
    dst->DarcyVelocity = src->DarcyVelocity ;
    dst->TIn           = src->TIn ;
}

/******************************************************************************/

void init_channel (Channel_t *this)
{
    this->ChannelModel      = (ChannelModelType_t) TDICE_CHANNEL_MODEL_NONE ;
    this->Height            = (CellDimension_t) 0.0 ;
    this->Length            = (ChannelDimension_t) 0.0 ;
    this->Pitch             = (ChannelDimension_t) 0.0 ;
    this->Porosity          = (ChannelDimension_t) 0.0 ;
    this->NChannels         = (Quantity_t) 0u ;
    this->NLayers           = (CellIndex_t) 0u ;
    this->SourceLayerOffset = (CellIndex_t) 0u ;

    init_coolant ( &this->Coolant ) ;

    this->WallMaterial = NULL ;
}

/******************************************************************************/

void copy_channel (Channel_t *dst, Channel_t *src)
{
    dst->ChannelModel          = src->ChannelModel ;
    dst->Height                = src->Height ;
    dst->Length                = src->Length ;
    dst->Pitch                 = src->Pitch ;
    dst->Porosity              = src->Porosity ;
    dst->NChannels             = src->NChannels ;
    dst->NLayers               = src->NLayers ;
    dst->SourceLayerOffset     = src->SourceLayerOffset ;

    copy_coolant (&dst->Coolant, &src->Coolant) ;

    dst->WallMaterial = src->WallMaterial ;
}

/******************************************************************************/

Channel_t *calloc_channel ( void )
{
    Channel_t *channel = (Channel_t *) malloc (sizeof(Channel_t)) ;

    if (channel != NULL)

        init_channel (channel) ;

    return channel ;
}

/******************************************************************************/

Channel_t *clone_channel (Channel_t *this)
{
    if (this == NULL)

        return NULL ;

    Channel_t *channel = calloc_channel ( ) ;

    if (channel != NULL)

        copy_channel (channel, this) ;

    return channel ;
}

/******************************************************************************/

void free_channel (Channel_t *this)
{
    if (this != NULL)

        FREE_POINTER (free, this) ;
}

/******************************************************************************/

void print_formatted_channel
(
    Channel_t    *this,
    FILE         *stream,
    String_t      prefix,
    Dimensions_t *dimensions
)
{
    if (this->ChannelModel == TDICE_CHANNEL_MODEL_MC_4RM)
    {
        fprintf (stream, "%smicrochannel 4rm :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, this->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   channel    length %7.1f ;\n", prefix, dimensions->Cell.ChannelLength) ;
        fprintf (stream, "%s   wall       length %7.1f ;\n", prefix, dimensions->Cell.WallLength) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   first wall length %7.1f ;\n", prefix, dimensions->Cell.FirstWallLength) ;
        fprintf (stream, "%s    last wall length %7.1f ;\n", prefix, dimensions->Cell.LastWallLength) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   wall material %s ;\n", prefix, this->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant flow rate  %.2f ;\n", prefix, FLOW_RATE_FROM_UM3SEC_TO_MLMIN(this->Coolant.FlowRate)) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant heat transfer coefficient side   %.4e ,\n", prefix, this->Coolant.HTCSide) ;
        fprintf (stream, "%s                                     top    %.4e ,\n", prefix, this->Coolant.HTCTop) ;
        fprintf (stream, "%s                                     bottom %.4e ;\n",  prefix, this->Coolant.HTCBottom) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, this->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, this->Coolant.TIn ) ;
    }

    if (this->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
    {
        fprintf (stream, "%smicrochannel 2rm :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, this->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   channel    length %7.1f ;\n", prefix, this->Length) ;
        fprintf (stream, "%s   wall       length %7.1f ;\n", prefix, this->Pitch - this->Length) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   wall material %s ;\n", prefix, this->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant flow rate  %.2f ;\n", prefix, FLOW_RATE_FROM_UM3SEC_TO_MLMIN(this->Coolant.FlowRate)) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant heat transfer coefficient top    %.4e ,\n", prefix, this->Coolant.HTCTop) ;
        fprintf (stream, "%s                                     bottom %.4e ;\n",  prefix, this->Coolant.HTCBottom) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, this->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, this->Coolant.TIn ) ;
    }

    if (this->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE)
    {
        fprintf (stream, "%spinfin :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, this->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin diameter %7.1f ;\n",   prefix, DIAMETER(this->Porosity, this->Pitch)) ;
        fprintf (stream, "%s   pin pitch    %7.1f ;\n", prefix, this->Pitch) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin distribution inline ;\n", prefix) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin material %s ;\n", prefix, this->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   darcy velocity  %.4e ;\n", prefix, this->Coolant.DarcyVelocity) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, this->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, this->Coolant.TIn ) ;
    }

    if (this->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
    {
        fprintf (stream, "%spinfin :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, this->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin diameter %7.1f ;\n",   prefix, DIAMETER(this->Porosity, this->Pitch)) ;
        fprintf (stream, "%s   pin pitch    %7.1f ;\n", prefix, this->Pitch) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin distribution staggered ;\n", prefix) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin material %s ;\n", prefix, this->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   darcy velocity  %.4e ;\n", prefix, this->Coolant.DarcyVelocity) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, this->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, this->Coolant.TIn ) ;
    }
}

/******************************************************************************/

void print_detailed_channel
(
  Channel_t *this,
  FILE      *stream,
  String_t   prefix
)
{
    fprintf (stream,
        "%schannel                     = %p\n",
        prefix,   this) ;

    fprintf (stream,
        "%s  ChannelModel              = %d\n",
        prefix, this->ChannelModel) ;

    fprintf (stream,
        "%s  Height                    = %.1f\n",
        prefix,  this->Height) ;

    fprintf (stream,
        "%s  Length                    = %.1f\n",
        prefix,  this->Length) ;

    fprintf (stream,
        "%s  Pitch                     = %.1f\n",
        prefix,  this->Pitch) ;

    fprintf (stream,
        "%s  Porosity                  = %.1f\n",
        prefix,  this->Porosity) ;

    fprintf (stream,
        "%s  NChannels                 = %d\n",
        prefix,  this->NChannels) ;

    fprintf (stream,
        "%s  NLayers                   = %d\n",
        prefix,  this->NLayers) ;

    fprintf (stream,
        "%s  SourceLayerOffset         = %d\n",
        prefix, this->SourceLayerOffset) ;

    fprintf (stream,
        "%s  Coolant.HTCSide           = %.4e\n",
        prefix,  this->Coolant.HTCSide) ;

    fprintf (stream,
        "%s  Coolant.HTCTop            = %.4e\n",
        prefix,  this->Coolant.HTCTop) ;

    fprintf (stream,
        "%s  Coolant.HTCBottom         = %.4e\n",
        prefix,  this->Coolant.HTCBottom) ;

    fprintf (stream,
        "%s  Coolant.VHC               = %.4e\n",
        prefix,  this->Coolant.VHC) ;

    fprintf (stream,
        "%s  Coolant.TIn               = %.2f\n",
        prefix,  this->Coolant.TIn) ;

    fprintf (stream,
        "%s  CoolantFR                 = %.4e\n",
        prefix,  this->Coolant.FlowRate) ;

    fprintf (stream,
        "%s  DarcyVelocity             = %.4e\n",
        prefix,  this->Coolant.DarcyVelocity) ;

    fprintf (stream,
        "%s  WallMaterial              = %p\n",
        prefix,  this->WallMaterial) ;
}

/******************************************************************************/

Cconv_t get_convective_term
(
    Channel_t    *this,
    Dimensions_t *dimensions,
    CellIndex_t   layer_index,
    CellIndex_t   __attribute__ ((unused)) row_index,
    CellIndex_t   column_index
)
{
    Cconv_t C = 0.0 ;

    switch (this->ChannelModel)
    {
        case TDICE_CHANNEL_MODEL_MC_4RM :

            C = CCONV_MC_4RM

                (this->NChannels, this->Coolant.VHC,
                 this->Coolant.FlowRate);

            break ;

        case TDICE_CHANNEL_MODEL_MC_2RM :

            C = CCONV_MC_2RM

                (this->NChannels,        this->Coolant.VHC,
                 this->Coolant.FlowRate, this->Porosity,
                 get_cell_length (dimensions, column_index),
                 this->Length) ;

            break ;

        case TDICE_CHANNEL_MODEL_PF_INLINE :
        case TDICE_CHANNEL_MODEL_PF_STAGGERED :

            C = CCONV_PF

                (this->Coolant.VHC, this->Coolant.DarcyVelocity,
                 get_cell_length (dimensions, column_index),
                 get_cell_height (dimensions, layer_index));

            break ;

        case TDICE_CHANNEL_MODEL_NONE :

            fprintf (stderr, "WARNING: unsert channel model\n") ;

            break ;

        default :

            fprintf (stderr, "ERROR: unknown channel model %d\n",
                this->ChannelModel) ;
    }

    return C ;
}

/******************************************************************************/

Temperature_t get_max_temperature_channel_outlet
(
    Channel_t     *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t max = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(this->ChannelModel, column_index) == true)

            max = MAX (max, *temperatures) ;

        temperatures++ ;
    }

    return max ;
}

/******************************************************************************/

Temperature_t get_min_temperature_channel_outlet
(
    Channel_t     *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t min = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(this->ChannelModel, column_index) == true)

            min = MIN (min, *temperatures) ;

        temperatures++ ;
    }

    return min ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_channel_outlet
(
    Channel_t     *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t avg = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(this->ChannelModel, column_index) == true)

            avg += *temperatures ;

        temperatures++ ;
    }

    return avg / (Temperature_t) this->NChannels ;
}

/******************************************************************************/
