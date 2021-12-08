/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#include <stdlib.h> // For the memory functions malloc/free

#include "material.h"

/******************************************************************************/

void material_init (Material_t *material)
{
    string_init (&material->Id) ;

    material->VolumetricHeatCapacity = (SolidVHC_t) 0.0 ;
    material->ThermalConductivity    = (SolidTC_t) 0.0 ;
}

/******************************************************************************/

void material_copy (Material_t *dst, Material_t *src)
{
    material_destroy (dst) ;

    string_copy (&dst->Id, &src->Id) ;

    dst->VolumetricHeatCapacity = src->VolumetricHeatCapacity ;
    dst->ThermalConductivity    = src->ThermalConductivity ;
}

/******************************************************************************/

void material_destroy (Material_t *material)
{
    string_destroy (&material->Id) ;

    material_init (material) ;
}

/******************************************************************************/

Material_t *material_calloc ( void )
{
    Material_t *material = (Material_t *) malloc (sizeof(Material_t)) ;

    if (material != NULL)

        material_init (material) ;

    return material ;
}

/******************************************************************************/

Material_t *material_clone (Material_t *material)
{
    if (material == NULL)

        return NULL ;

    Material_t *newm = material_calloc ( ) ;

    if (newm != NULL)

            material_copy (newm, material) ;

    return newm ;
}

/******************************************************************************/

void material_free (Material_t *material)
{
    if (material == NULL)

        return ;

    material_destroy (material) ;

    free (material) ;
}

/******************************************************************************/

bool material_same_id (Material_t *material, Material_t *other)
{
    return string_equal (&material->Id, &other->Id) ;
}

/******************************************************************************/

void material_print (Material_t *material, FILE *stream, String_t prefix)
{
    fprintf (stream,
             "%smaterial %s :\n",
             prefix, material->Id) ;

    fprintf (stream,
             "%s   thermal conductivity     %.4e ;\n",
             prefix, material->ThermalConductivity) ;

    fprintf (stream,
             "%s   volumetric heat capacity %.4e ;\n",
             prefix, material->VolumetricHeatCapacity) ;
}

/******************************************************************************/
