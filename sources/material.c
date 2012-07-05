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
#include <string.h>

#include "material.h"
#include "macros.h"

/******************************************************************************/

void init_material (Material_t *material)
{
    material->Id                     = NULL ;
    material->Used                   = (Quantity_t) 0u ;
    material->VolumetricHeatCapacity = (SolidVHC_t) 0.0 ;
    material->ThermalConductivity    = (SolidTC_t) 0.0 ;
    material->Next                   = NULL ;
}

/******************************************************************************/

Material_t *calloc_material ( void )
{
    Material_t *material = (Material_t *) malloc (sizeof(Material_t)) ;

    if (material != NULL)

        init_material (material) ;

    return material ;
}

/******************************************************************************/

void free_material (Material_t *material)
{
    if (material == NULL)

        return ;

    if (material->Id != NULL)

        FREE_POINTER (free, material->Id) ;

    FREE_POINTER (free, material) ;
}

/******************************************************************************/

void free_materials_list (Material_t *list)
{
    FREE_LIST (Material_t, list, free_material) ;
}

/******************************************************************************/

Material_t *find_material_in_list (Material_t *list, String_t id)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material_t, material, list)
    {
        if (strcmp(material->Id, id) == 0)
            break ;
    }
    return material ;
}

/******************************************************************************/

void print_material
(
    Material_t *material,
    FILE       *stream,
    String_t    prefix
)
{
    fprintf (stream,
             "%smaterial %s :\n",
             prefix, material->Id) ;

    fprintf (stream,
             "%s   thermal conductivity     %.4e  ;\n",
             prefix, material->ThermalConductivity) ;

    fprintf (stream,
             "%s   volumetric heat capacity %.4e  ;\n",
             prefix, material->VolumetricHeatCapacity) ;
}

/******************************************************************************/

void print_materials_list
(
    Material_t *list,
    FILE       *stream,
    String_t    prefix
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material_t, material, list)
    {
        if (material->Next == NULL)

            break ;

        print_material (material, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_material (material, stream, prefix) ;
}

/******************************************************************************/
