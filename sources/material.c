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

void init_material (Material *material)
{
    material->Id                     = NULL ;
    material->Used                   = 0u ;
    material->VolumetricHeatCapacity = (SolidVHC_t) 0.0 ;
    material->ThermalConductivity    = (SolidTC_t) 0.0 ;
    material->Next                   = NULL ;
}

/******************************************************************************/

Material *alloc_and_init_material (void)
{
    Material *material = (Material *) malloc (sizeof(Material)) ;

    if (material != NULL)

        init_material (material) ;

    return material ;
}

/******************************************************************************/

void free_material (Material *material)
{
    if (material->Id != NULL)

        FREE_POINTER (free, material->Id) ;

    FREE_POINTER (free, material) ;
}

/******************************************************************************/

void free_materials_list (Material *list)
{
    FREE_LIST (Material, list, free_material) ;
}

/******************************************************************************/

Material *find_material_in_list (Material *list, String_t id)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material, material, list)
    {
        if (strcmp(material->Id, id) == 0)
            break ;
    }
    return material ;
}

/******************************************************************************/

void print_formatted_material
(
    FILE     *stream,
    String_t  prefix,
    Material *material
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

void print_formatted_materials_list
(
    FILE     *stream,
    String_t  prefix,
    Material *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material, material, list)
    {
        if (material->Next == NULL)

            break ;

        print_formatted_material (stream, prefix, material) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_material (stream, prefix, material) ;
}

/******************************************************************************/

void print_detailed_material
(
    FILE     *stream,
    String_t  prefix,
    Material *material
)
{
    fprintf (stream,
             "%smaterial                    = %p\n",
             prefix, material) ;

    fprintf (stream,
             "%s  Id                        = %s\n",
             prefix, material->Id) ;

    fprintf (stream,
             "%s  Used                      = %d\n",
             prefix, material->Used) ;

    fprintf (stream,
             "%s  VolumetricHeatCapacity    = %.4e\n",
             prefix, material->VolumetricHeatCapacity) ;

    fprintf (stream,
             "%s  ThermalConductivity       = %.4e\n",
             prefix, material->ThermalConductivity) ;

    fprintf (stream,
             "%s  Next                      = %p\n",
             prefix, material->Next) ;
}

/******************************************************************************/

void print_detailed_materials_list
(
    FILE     *stream,
    String_t  prefix,
    Material *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material, material, list)
    {
        if (material->Next == NULL)

            break ;

        print_detailed_material (stream, prefix, material) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_material (stream, prefix, material) ;
}

/******************************************************************************/
