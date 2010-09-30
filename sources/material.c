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
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *                                                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "material.h"

/******************************************************************************/

void init_material (Material* material)
{
  material->Id                  = NULL ;
  material->Used                = 0 ;
  material->VolHeatCapacity     = 0.0  ;
  material->ThermalConductivity = 0.0  ;
  material->Next                = NULL ;
}

/******************************************************************************/

Material* alloc_and_init_material (void)
{
  Material* material = (Material*) malloc (sizeof (Material)) ;

  if (material != NULL) init_material (material) ;

  return material ;
}

/******************************************************************************/

void free_material (Material* material)
{
  free (material->Id) ;
  free (material) ;
}

/******************************************************************************/

void free_materials_list (Material* list)
{
  Material* next ;

  for ( ; list != NULL ; list = next)
  {
      next = list->Next ;
      free_material (list) ;
  }
}

/******************************************************************************/

void print_material (FILE* stream, String_t prefix, Material* material)
{
  fprintf (stream,
    "%sMaterial %s:\n",                    prefix,
                                           material->Id) ;
  fprintf (stream,
    "%s  Volumetric Heat Capacity %.3e\n", prefix,
                                           material->VolHeatCapacity) ;
  fprintf (stream,
    "%s  Thermal Conductivity     %.3e\n", prefix,
                                           material->ThermalConductivity) ;
}

/******************************************************************************/

void print_materials_list (FILE* stream, String_t prefix, Material* list)
{
  for ( ; list != NULL ; list = list->Next)

    print_material (stream, prefix, list) ;
}

/******************************************************************************/

Material* find_material_in_list (Material* list, String_t id)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp (list->Id, id) == 0) break ;

  return list ;
}

/******************************************************************************/
