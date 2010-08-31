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

#include <stdlib.h>
#include <stdio.h>

#include "system_vector.h"

/******************************************************************************/

int alloc_system_vector (SystemVector* vector, Quantity_t size)
{
  vector->Size   = size ;
  vector->Values
    = (SystemVectorValue_t*) malloc ( sizeof(SystemVectorValue_t) * size ) ;

  if (vector->Values == NULL)  return 0 ;

  return 1 ;
}

/******************************************************************************/

void fill_system_vector
(
  SystemVector*  vector,
  Source_t*      source,
  Capacity_t*    capacity,
  Temperature_t* temperature
)
{
  int count ;
  SystemVectorValue_t* value = vector->Values ;

#ifdef PRINT_SYSTEM_VECTOR
    fprintf (stderr, "fill_system_vector\n");
#endif

  for(count = 0 ; count < vector->Size ; count++)
  {
    *value++ = *source++ + *capacity++ * *temperature++ ;
#ifdef PRINT_SYSTEM_VECTOR
    fprintf (stderr, "[%5d] b %.5e = s %.5e + c %.5e * t %5.2f\n",
      count, *(value-1), *(source-1), *(capacity-1), *(temperature-1)) ;
#endif
  }
}

/******************************************************************************/

void free_system_vector (SystemVector* vector)
{
  free (vector->Values) ;
}

/******************************************************************************/
