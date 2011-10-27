/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_TYPES_H_
#define _3DICE_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

  /* Boolean value */

  typedef int Bool_t ;
# define      FALSE_V (0)
# define      TRUE_V  (!FALSE_V)

  /* Quantity */

  typedef int Quantity_t ;
# define      QUANTITY_I 0

  /* String */

  typedef char* String_t ;
# define        STRING_I NULL

/******************************************************************************/

  /* Conductance */

  typedef double Conductance_t ;
# define         CONDUCTANCE_I 0.0

  /* Capacity */

  typedef double Capacity_t ;
# define         CAPACITY_I 0.0

  /* Source */

  typedef double Source_t ;
# define         SOURCE_I 0.0

  /* Power */

  typedef double Power_t ;
# define         POWER_I 0.0

  /* Temperature */

  typedef double Temperature_t ;
# define         TEMPERATURE_I 0.0

  /* Time */

  typedef double Time_t ;
# define         TIME_I 0.0

/******************************************************************************/

  /* Solid volumetric heat capacity */

  typedef double SolidVHC_t ;
# define         SOLIDVHC_I 0.0

  /* Solid thermal conductivity */

  typedef double SolidTC_t ;
# define         SOLIDTC_I 0.0

/******************************************************************************/

  /* Ambient heat transfer coefficient */

  typedef double AmbientHTC_t ;
# define         AMBIENTHTC_I 0.0

/******************************************************************************/

  /* Coolant heat transfer coefficient */

  typedef double CoolantHTC_t ;
# define         COOLANTHTC_I 0.0

  /* Coolant volumetric heat capacity */

  typedef double CoolantVHC_t ;
# define         COOLANTVHC_I 0.0

  /* the collection of parameters describing a cooling fluid */

  struct Coolant_t
  {
    /* The heat transfert coefficients in [ (W / ( um2 * K ) ] */

    CoolantHTC_t HTCSide ;
    CoolantHTC_t HTCTop ;
    CoolantHTC_t HTCBottom ;

    /* The volumetric heat capacity in [ J / ( um3 * K ) ] */

    CoolantVHC_t VHC ;

    /* The temperarute at the channel inlet in [K]  */

    Temperature_t TIn ;

  } ;

  typedef struct Coolant_t Coolant_t ;
# define         COOLANT_I (Coolant_t) { COOLANTHTC_I, COOLANTHTC_I, COOLANTHTC_I, \
                                         COOLANTVHC_I, TEMPERATURE_I }

  /* Coolant flow rate */

  typedef double CoolantFR_t ;
# define         COOLANTFR_I 0.0

  /* Darcy Velocity  */

  typedef double DarcyVelocity_t ;
# define         DARCYVELOCITY_I 0.0

  /* C convection coefficient */

  typedef double Cconv_t ;
# define         CCONV_I 0.0

  /* Porosity */

  typedef double Porosity_t ;
# define         POROSITY_I 0.0

/******************************************************************************/

  /* Grid dimension (number and index of layer, row, column) */

  typedef int GridDimension_t ;
# define      GRIDDIMENSION_I 0

  /* Cell dimension (for cell length, height, width) */

  typedef double CellDimension_t ;
# define         CELLDIMENSION_I 0.0

  /* Chip dimension (for chip length, width) */

# define ChipDimension_t CellDimension_t
# define CHIPDIMENSION_I CELLDIMENSION_I

/******************************************************************************/

  /* System matrix coefficients */

  typedef double SystemMatrixValue_t ;
# define         SYSTEMMATRIXVALUE_I 0.0

  /* System matrix column indexes */

  typedef int SystemMatrixColumn_t ;
# define      SYSTEMMATRIXCOLUMN_I 0

  /* System matrix row indexes */

  typedef int SystemMatrixRow_t ;
# define      SYSTEMMATRIXROW_I 0

/******************************************************************************/

  /* The type of a stack element */

  enum StackElement_t
  {
    TDICE_STACK_ELEMENT_NONE = 0,
    TDICE_STACK_ELEMENT_LAYER   ,
    TDICE_STACK_ELEMENT_CHANNEL ,
    TDICE_STACK_ELEMENT_DIE
  } ;

  typedef enum StackElement_t StackElement_t ;
# define                      STACKELEMENT_I TDICE_STACK_ELEMENT_NONE

/******************************************************************************/

  /* The type of a channel model type */

  enum ChannelModel_t
  {
    TDICE_CHANNEL_MODEL_MC_RM4 = 0,
    TDICE_CHANNEL_MODEL_MC_RM2,
    TDICE_CHANNEL_MODEL_PF_INLINE,
    TDICE_CHANNEL_MODEL_PF_STAGGERED
  } ;

  typedef enum ChannelModel_t ChannelModel_t ;

# define                      CHANNELMODEL_I TDICE_CHANNEL_MODEL_MC_RM4
# define                      NUM_LAYERS_4RM 1
# define                      NUM_LAYERS_2RM 4

/******************************************************************************/

  enum Error_t
  {
    TDICE_SUCCESS,
    TDICE_FAILURE
  } ;

  typedef enum Error_t Error_t ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_TYPES_H_ */
