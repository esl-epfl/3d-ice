/******************************************************************************
 * This file is part of 3D-ICE, version 1.0 .                                 *
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

# define FALSE_V (0)
# define TRUE_V  (!FALSE_V)

  /* Quantity */

  typedef int Quantity_t ;

  /* String */

  typedef char* String_t ;

  /* Solid volumetric heat capacity */

  typedef double SolidVHC_t ;

  /* Solid thermal conductivity */

  typedef double SolidTC_t ;

  /* Grid dimension (number and index of layer, row, column) */

  typedef int GridDimension_t ;

  /* Cell dimension (for cell length, height, width) */

  typedef double CellDimension_t ;

  /* Chip dimension (for chip length, width) */

# define ChipDimension_t CellDimension_t

  /* Capacity */

  typedef double Capacity_t ;

  /* Source */

  typedef double Source_t ;

  /* Temperature */

  typedef double Temperature_t ;

  /* Power */

  typedef double Power_t ;

  /* Ambient heat transfer coefficient */

  typedef double AmbientHTC_t ;

  /* Time */

  typedef double Time_t ;

  /* Conductance */

  typedef double Conductance_t ;

  /* Coolant heat transfer coefficient */

  typedef double CoolantHTC_t ;

  /* Coolant volumetric heat capacity */

  typedef double CoolantVHC_t ;

  /* Coolant flow rate */

  typedef double CoolantFR_t ;

  /* C convection coefficient */

  typedef double Cconv_t ;

  /* boolean Value */

  typedef int Bool_t ;

  /* System matrix coefficients */

  typedef double SystemMatrixValue_t ;

  /* System matrix column indexes */

  typedef int SystemMatrixColumn_t ;

  /* System matrix row indexes */

  typedef int SystemMatrixRow_t ;

  /* The type of a stack element */

  enum StackElement_t
  {
    TDICE_STACK_ELEMENT_NONE = 0,
    TDICE_STACK_ELEMENT_LAYER   ,
    TDICE_STACK_ELEMENT_CHANNEL ,
    TDICE_STACK_ELEMENT_DIE
  } ;

  typedef enum StackElement_t StackElement_t ;

  /* The collection of the coolant heat transfer coefficient */

  typedef struct
  {
    CoolantHTC_t Side ;
    CoolantHTC_t Top ;
    CoolantHTC_t Bottom ;

  } CoolantHTCs_t ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_TYPES_H_ */
