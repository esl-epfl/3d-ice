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

#include "layer.h"
#include "layout_file_parser.h"

/******************************************************************************/

void layer_init (Layer_t *layer)
{
    string_init (&layer->Id) ;
    string_init (&layer->LayoutFileName) ;

    layer->Height = (CellDimension_t) 0.0 ;

    material_init (&layer->Material) ;

    material_element_list_init (&layer->MaterialLayout) ;
}

/******************************************************************************/

void layer_copy (Layer_t *dst, Layer_t *src)
{
    layer_destroy (dst) ;

    string_copy (&dst->Id, &src->Id) ;
    string_copy (&dst->LayoutFileName, &src->LayoutFileName) ;

    dst->Height = src->Height ;

    material_copy (&dst->Material, &src->Material) ;

    material_element_list_copy (&dst->MaterialLayout, &src->MaterialLayout) ;
}

/******************************************************************************/

void layer_destroy (Layer_t *layer)
{
    string_destroy (&layer->Id) ;
    string_destroy (&layer->LayoutFileName) ;

    material_destroy (&layer->Material) ;

    material_element_list_destroy (&layer->MaterialLayout) ;

    layer_init (layer) ;
}

/******************************************************************************/

Layer_t *layer_calloc (void)
{
    Layer_t *layer = (Layer_t *) malloc (sizeof(Layer_t));

    if (layer != NULL)

        layer_init (layer) ;

    return layer ;
}

/******************************************************************************/

Layer_t *layer_clone (Layer_t *layer)
{
    if (layer == NULL)

        return NULL ;

    Layer_t *newl = layer_calloc ( ) ;

    if (newl != NULL)

        layer_copy (newl, layer) ;

    return newl ;
}

/******************************************************************************/

void layer_free (Layer_t *layer)
{
    if (layer == NULL)

        return ;

    layer_destroy (layer) ;

    free (layer) ;
}

/******************************************************************************/

bool layer_same_id (Layer_t *layer, Layer_t *other)
{
    return string_equal (&layer->Id, &other->Id) ;
}

/******************************************************************************/

void layer_print (Layer_t *layer, FILE *stream, String_t prefix)
{
    fprintf (stream,
        "%slayer %s :\n",
        prefix, layer->Id) ;

    fprintf (stream,
        "%s   height %.1f ;\n",
        prefix, layer->Height) ;

    fprintf (stream,
        "%s   material %s ;\n",
        prefix, layer->Material.Id) ;

    fprintf (stream,
        "%s   layout   \"%s\" ;\n",
        prefix, layer->LayoutFileName) ;
}

/******************************************************************************/

Error_t fill_layout
(
    Layer_t        *layer,
    Dimensions_t   *dimensions,
    MaterialList_t *materials,
    String_t        filename
)
{
    Error_t result ;

    result = parse_layout_file (filename, layer, materials, dimensions) ;

    if (result == TDICE_FAILURE)

        return TDICE_FAILURE ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

SolidTC_t get_thermal_conductivity
(
    Layer_t      *layer,
    CellIndex_t   row_index,
    CellIndex_t   column_index,
    Dimensions_t *dimensions
)
{
    Material_t *tmp = NULL ;

    MaterialElementListNode_t *melementn ;

    for (melementn  = material_element_list_begin (&layer->MaterialLayout) ;
         melementn != NULL ;
         melementn  = material_element_list_next (melementn))
    {
        MaterialElement_t *melement = material_element_list_data (melementn) ;

        tmp = get_material_at_location (melement, row_index, column_index, dimensions) ;

        if (tmp != NULL)    return tmp->ThermalConductivity ;
    }

    return layer->Material.ThermalConductivity ;
}

/******************************************************************************/

SolidTC_t get_volumetric_heat_capacity
(
    Layer_t      *layer,
    CellIndex_t   row_index,
    CellIndex_t   column_index,
    Dimensions_t *dimensions
)
{
    Material_t *tmp = NULL ;

    MaterialElementListNode_t *melementn ;

    for (melementn  = material_element_list_begin (&layer->MaterialLayout) ;
         melementn != NULL ;
         melementn  = material_element_list_next (melementn))
    {
        MaterialElement_t *melement = material_element_list_data (melementn) ;

        tmp = get_material_at_location (melement, row_index, column_index, dimensions) ;

        if (tmp != NULL)    return tmp->VolumetricHeatCapacity ;
    }

    return layer->Material.VolumetricHeatCapacity ;
}

/******************************************************************************/
