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

#ifndef _3DICE_MACROS_H_
#define _3DICE_MACROS_H_

/*! \file macros.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <math.h> // For the math function sqrt

/******************************************************************************/

    /*! \def PI
     *
     * Returns the constant \f$ \Pi \f$
     */

#   define PI 3.14159265358979323846



    /*! \def MAX(a,b)
     *
     * Returns the maximum value between \a a and \a b
     */

#   define MAX(a,b)  (((a) > (b)) ? (a) : (b))



    /*! \def MIN(a,b)
     *
     * Returns the minimum value between \a a and \a b
     */

#   define MIN(a,b)  (((a) < (b)) ? (a) : (b))



    /*! \def PARALLEL(x,y)
     *
     * Returns the equivalent resistance of \a x and \a y
     * connected in parallel
     */

#   define PARALLEL(x,y) (((x) * (y)) / ((x) + (y)))

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def IS_CHANNEL_COLUMN(channel_model,column)
     *
     * Returns \c TRUE if \a column is the index of a channel column. If
     * \a channel_model is 4RM, then odd indeces are channel columns.
     *  Otherwise, all indexes are channels (2RM microchannel or pin fins).
     */

#   define IS_CHANNEL_COLUMN(channel_model, column) \
                                                    \
        (channel_model == TDICE_CHANNEL_MODEL_MC_4RM ? (column) & 1 : true)

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def CCONV_MC_4RM(nchannels,vhc,fr)
     *
     * Returns the \c C convective term for the 4RM model of microchannels
     *
     * FlowRatePerChannel [ um3 / sec ] = FlowRate             [ um3 / sec ]
     *                                    / \#ChannelColumns   [ ]
     *
     * CoolantVelocity     [ m / sec ]  = FlowRatePerChannel   [ um3 / sec ]
     *                                      * ( 1 / Ay )         [   1 / um2 ]
     *
     * Cconv         [ J / ( K . sec) ]  = CoolantVHC          [ J / ( um3 . K ) ]
     *                                       * CoolantVelocity   [ m / sec ]
     *                                       * ( Ay / 2 )        [ um2 ]
     * [ J / ( K . sec) ] = [ W / K ]
     *
     * CoolantVelocity = FlowRate / (\#ChannelColumns * Ay )
     *
     * Cconv           = (CoolantVHC * FlowRate) / (\#ChannelColumns * 2)
     */

#   define CCONV_MC_4RM(nchannels, vhc, fr) \
                                            \
        ((double) ((vhc * fr) / ((double) (nchannels * 2.0))))



    /*! \def CCONV_MC_2RM(nchannels,vhc,fr,porosity,cell_l,channel_l)
     *
     * Returns the \c C convective term for the 2RM model of microchannels
     *
     *
     * FlowRatePerChannel [ um3 / sec ] = FlowRate                            [ um3 / sec ]
     *                                    / \#Channels                        [ ]
     *
     * CoolantVelocity      [ m / sec ] = FlowRatePerChannel                  [ um3 / sec ]
     *                                    / (CavityHeight * ChannelLength)    [ um2 ]
     *
     * Cconv         [ J / ( K . sec) ] = CoolantVHC                          [ J / ( um3 . K ) ]
     *                                      * CoolantVelocity                   [ m / sec ]
     *                                      * ( Cavityheight * CellLength / 2 ) [ um2 ]
     *                                      * Porosity                          [ ]
     * [ J / ( K . sec) ] = [ W / K ]
     *
     * CoolantVelocity = FlowRate / (\#Channels * CavityHeight * ChannelLength)
     *
     * Cconv           = (CoolantVHC * FlowRate * Porosity * CellLength)
     *                   / (\#Channels * 2 * ChannelLength)
     */

#   define CCONV_MC_2RM(nchannels, vhc, fr, porosity, cell_l, channel_l)     \
                                                                             \
        ((double) (  (vhc * fr * porosity * cell_l)                          \
                   / ( (double) nchannels * 2.0 * channel_l) ))



    /*! \def CCONV_PF(vhc,darcy_velocity,cell_l,cavity_h)
     *
     * Returns the \c C convective term for the 2RM model of pin fins
     *
     * Cconv     [ J / ( K . sec) ] = CoolantVHC                             [ J / ( um3 . K ) ]
     *                                  * DarcyVelosity                        [ m / sec ]
     *                                  * ( CavityHeight * CellLength / 2 )    [ um2 ]
     *                                  * Porosity                             [ ]
     */

#   define CCONV_PF(vhc, darcy_velocity, cell_l, cavity_h) \
                                                           \
        ((double) ((vhc * darcy_velocity * cell_l * cavity_h) / 2.0))

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



    /*! \def EFFECTIVE_HTC_PF_INLINE(darcy_velocity)
     *
     * Returns the Effective HTC for inline pin fins with 2RM model
     *
     * H_eff_inline = ((2.527E-5 / (DarcyVelosity + 1.35)^0.64 )+1.533E-6)^(-1) * 1E-12
     */

#   define EFFECTIVE_HTC_PF_INLINE(darcy_velocity)                             \
                                                                               \
        ((double) (  1e-12                                                     \
                   / (  2.527e-05                                              \
                      / pow((darcy_velocity/1e+06 + 1.35), 0.64) + 1.533e-06)))



    /*! \def EFFECTIVE_HTC_PF_STAGGERED(darcy_velocity)
     *
     * Returns the Effective HTC for staggered pin fins with 2RM model
     *
     * H_eff_stag = ((2.527E-5 / (DarcyVelosity + 1.35)^1.52 )+1.533E-6)^(-1) * 1E-12
     */

#   define EFFECTIVE_HTC_PF_STAGGERED(darcy_velocity)                          \
                                                                               \
        ((double) (  1e-12                                                     \
                   / (  2.527e-05                                              \
                      / pow((darcy_velocity/1e+06 + 1.35), 1.52) + 1.533e-06)))



    /*! \def FLOW_RATE_FROM_MLMIN_TO_UM3SEC(fr)
     *
     *  Converts the flow rate from \f$ \frac{ml}{min} \f$ to
     *  \f$ \frac{\mu m^3}{sec} \f$
     */

#   define FLOW_RATE_FROM_MLMIN_TO_UM3SEC(fr) (( fr * 1e+12 ) / 60.0)



    /*! \def FLOW_RATE_FROM_UM3SEC_TO_MLMIN(fr)
     *
     *  Converts the flow rate from \f$ \frac{\mu m^3}{sec} \f$ to
     *  \f$ \frac{ml}{min} \f$
     */

#   define FLOW_RATE_FROM_UM3SEC_TO_MLMIN(fr) (( fr * 60.0 ) / 1e+12)



    /*! \def POROSITY(diameter,pitch)
     *
     *  Returns the porosity of a 2RM pin fins
     */

#   define POROSITY(diameter, pitch) \
                                     \
        (1.0 - (PI * diameter * diameter / 4.0) / (pitch * pitch))



    /*! \def DIAMETER(porosity, pitch)
     *
     *  Returns the diameter of a 2RM pin fins
     */

#   define DIAMETER(porosity, pitch) \
                                     \
        (sqrt (( (1.0 - porosity) * pitch*pitch * 4.0 ) / PI ))

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MACROS_H_ */
