/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
 ******************************************************************************/

#ifndef _3DICE_SYSTEM_VECTOR_
#define _3DICE_SYSTEM_VECTOR_

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

  typedef struct
  {
    SystemVectorValue_t*    Values ;
    Quantity_t              Size ;

  } SystemVector ;

/******************************************************************************/

  int alloc_system_vector  (SystemVector* vector, Quantity_t size) ;

/******************************************************************************/

  void free_system_vector (SystemVector* vector) ;

/******************************************************************************/

  void fill_system_vector
  (
    SystemVector*  vector,
    Source_t*      source,
    Capacity_t*    capacity,
    Temperature_t* temperature
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
