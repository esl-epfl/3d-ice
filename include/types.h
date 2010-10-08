/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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

  typedef char*  String_t ;
  typedef int    Bool_t ;
  typedef int    Quantity_t ;
  typedef int    LayerIndex_t ;
  typedef int    RowIndex_t ;
  typedef int    ColumnIndex_t ;
  typedef double Temperature_t ;
  typedef double Power_t ;
  typedef double Time_t ;
  typedef double SystemMatrixValue_t ;
  typedef double CellDimension_t ;
  typedef int    GridDimension_t ;
  typedef double ChipDimension_t ;
  typedef double VolHeatCapacity_t ;
  typedef double ThermalConductivity_t;
  typedef double Conductance_t ;
  typedef double Capacity_t ;
  typedef double Source_t ;
  typedef double Cconv_t ;
  typedef double CoolantHTC_t ;
  typedef double CoolantVHC_t ;
  typedef double CoolantFR_t ;
  typedef double AmbientHTC_t ;

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
