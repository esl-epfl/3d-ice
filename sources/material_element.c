/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#include <stdlib.h> // For the memory functions malloc/free

#include "material_element.h"
#include "ic_element.h"

/******************************************************************************/

void material_element_init (MaterialElement_t *melement)
{
    material_init (&melement->Material) ;

    melement->NMElements = (Quantity_t) 0u ;

    ic_element_list_init (&melement->MElements) ;
}

/******************************************************************************/

void material_element_copy
(
    MaterialElement_t *dst,
    MaterialElement_t *src
)
{
    material_element_destroy (dst) ;

    material_copy (&dst->Material, &src->Material) ;

    dst->NMElements = src->NMElements ;

    ic_element_list_copy (&dst->MElements, &src->MElements) ;
}

/******************************************************************************/

void material_element_destroy (MaterialElement_t *melement)
{
    material_destroy (&melement->Material) ;

    ic_element_list_destroy (&melement->MElements) ;

    material_element_init (melement) ;
}

/******************************************************************************/

MaterialElement_t *material_element_calloc (void)
{
    MaterialElement_t *melement =

        (MaterialElement_t *) malloc (sizeof(MaterialElement_t));

    if (melement != NULL)

        material_element_init (melement) ;

    return melement ;
}

/*****************************************************************************/

MaterialElement_t *material_element_clone (MaterialElement_t *melement)
{
    if (melement == NULL)

        return NULL ;

    MaterialElement_t *newm = material_element_calloc ( ) ;

    if (newm != NULL)

        material_element_copy (newm, melement) ;

    return newm ;
}

/*****************************************************************************/

void material_element_free (MaterialElement_t *melement)
{
    if (melement == NULL)

        return ;

    material_element_destroy (melement) ;

    free (melement) ;
}

/******************************************************************************/

bool material_element_same_material
(
    MaterialElement_t *melement,
    MaterialElement_t *other
)
{
    return material_same_id (&melement->Material, &other->Material) ;
}

/******************************************************************************/

void material_element_print
(
    MaterialElement_t *melement,
    FILE              *stream,
    String_t           prefix
)
{
    fprintf (stream,
        "%s%s:\n",
        prefix, melement->Material.Id) ;

    ic_element_list_print

        (&melement->MElements, stream, prefix) ;

    fprintf (stream, " ;\n%s\n", prefix) ;
}

/******************************************************************************/

Material_t *get_material_at_location
(
    MaterialElement_t *melement,
    CellIndex_t        row_index,
    CellIndex_t        column_index,
    Dimensions_t      *dimensions
)
{
    CellDimension_t cellx = get_cell_center_x (dimensions, column_index) ;
    CellDimension_t celly = get_cell_center_y (dimensions, row_index) ;

    ICElementListNode_t *icelementn ;

    for (icelementn  = ic_element_list_begin (&melement->MElements) ;
         icelementn != NULL ;
         icelementn  = ic_element_list_next (icelementn))
    {
         ICElement_t *icelement = ic_element_list_data (icelementn) ;

         if (ic_element_has_center (icelement, cellx, celly) == true)

             return &melement->Material ;
    }

    return NULL ;
}