/******************************************************************************
 * Header file "3D-ICe/include/system_vector.h"                               *
 *                                                                            *
 * This file is part of 3D-ICe (http://esl.epfl.ch/3D-ICe), revision 0.1      *
 *                                                                            *
 * 3D-ICe is free software: you can redistribute it and/or modify it under    *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or any later         *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICe is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with 3D-ICe.  If not, see <http://www.gnu.org/licenses/>.                  *
 *                                                                            *
 * Copyright (C) 2010,                                                        *
 * Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne.    *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * Authors: Alessandro Vincenzi, Arvind Sridhar.                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                          threed-ice@esl.epfl.ch *
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
