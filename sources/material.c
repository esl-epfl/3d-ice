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

void init_material (Material_t *this)
{
    this->Id                     = NULL ;
    this->Used                   = (Quantity_t) 0u ;
    this->VolumetricHeatCapacity = (SolidVHC_t) 0.0 ;
    this->ThermalConductivity    = (SolidTC_t) 0.0 ;
    this->Next                   = NULL ;
}

/******************************************************************************/

void copy_material (Material_t *dst, Material_t *src)
{
    dst->Id = (src->Id == NULL) ? NULL : strdup (src->Id) ;

    dst->Used                   = src->Used ;
    dst->VolumetricHeatCapacity = src->VolumetricHeatCapacity ;
    dst->ThermalConductivity    = src->ThermalConductivity ;
    dst->Next                   = src->Next ;
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

Material_t *clone_material (Material_t *this)
{
    if (this == NULL)

        return NULL ;

    Material_t *material = calloc_material ( ) ;

    if (material != NULL)

        copy_material (material, this) ;

    return material ;
}

/******************************************************************************/

void free_material (Material_t *this)
{
    if (this == NULL)

        return ;

    if (this->Id != NULL)

        FREE_POINTER (free, this->Id) ;

    FREE_POINTER (free, this) ;
}

/******************************************************************************/

Material_t *clone_materials_list (Material_t *list)
{
    if (list == NULL)

        return NULL ;

    Material_t *new_list = NULL ;
    Material_t *prev     = NULL ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material_t, material, list)
    {
        Material_t *tmp = clone_material (material) ;

        if (tmp == NULL)
        {
            free_materials_list (new_list) ;

            new_list = NULL ;

            break ;
        }

        if (new_list == NULL)    new_list   = tmp ;
        else                     prev->Next = tmp ;

        prev = tmp ;
    }

    return new_list ;
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

void print_formatted_material
(
    Material_t *this,
    FILE       *stream,
    String_t    prefix
)
{
    fprintf (stream,
             "%smaterial %s :\n",
             prefix, this->Id) ;

    fprintf (stream,
             "%s   thermal conductivity     %.4e  ;\n",
             prefix, this->ThermalConductivity) ;

    fprintf (stream,
             "%s   volumetric heat capacity %.4e  ;\n",
             prefix, this->VolumetricHeatCapacity) ;
}

/******************************************************************************/

void print_formatted_materials_list
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

        print_formatted_material (material, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_material (material, stream, prefix) ;
}

/******************************************************************************/

void print_detailed_material
(
    Material_t *this,
    FILE       *stream,
    String_t    prefix
)
{
    fprintf (stream,
             "%smaterial                    = %p\n",
             prefix, this) ;

    fprintf (stream,
             "%s  Id                        = %s\n",
             prefix, this->Id) ;

    fprintf (stream,
             "%s  Used                      = %d\n",
             prefix, this->Used) ;

    fprintf (stream,
             "%s  VolumetricHeatCapacity    = %.4e\n",
             prefix, this->VolumetricHeatCapacity) ;

    fprintf (stream,
             "%s  ThermalConductivity       = %.4e\n",
             prefix, this->ThermalConductivity) ;

    fprintf (stream,
             "%s  Next                      = %p\n",
             prefix, this->Next) ;
}

/******************************************************************************/

void print_detailed_materials_list
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

        print_detailed_material (material, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_material (material, stream, prefix) ;
}

/******************************************************************************/
