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

#include "die.h"
#include "macros.h"

/******************************************************************************/

void init_die (Die *die)
{
    die->Id                = NULL ;
    die->Used              = 0u ;
    die->NLayers           = 0u ;
    die->SourceLayerOffset = 0u ;
    die->TopLayer          = NULL ;
    die->SourceLayer       = NULL ;
    die->BottomLayer       = NULL ;
    die->Next              = NULL ;
}

/******************************************************************************/

Die *alloc_and_init_die (void)
{
    Die *die = (Die *) malloc (sizeof(Die)) ;

    if (die != NULL)

        init_die (die) ;

    return die ;
}

/******************************************************************************/

void free_die (Die *die)
{
    if (die->Id != NULL)

        FREE_POINTER (free, die->Id) ;

    die->TopLayer = NULL ;
    FREE_POINTER (free_layers_list, die->BottomLayer) ;

    FREE_POINTER (free, die) ;
}

/******************************************************************************/

void free_dies_list (Die *list)
{
    FREE_LIST (Die, list, free_die) ;
}

/******************************************************************************/

Die *find_die_in_list (Die *list, String_t id)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die, die, list)
    {
        if (strcmp(die->Id, id) == 0) break ;
    }
    return die ;
}

/******************************************************************************/

void print_formatted_die (FILE  *stream, String_t prefix, Die *die)
{
    String_t new_prefix_layer = (String_t)

        malloc (sizeof (*new_prefix_layer) * (10 + strlen(prefix))) ;

    String_t new_prefix_source = (String_t)

        malloc (sizeof (*new_prefix_source) * (10 + strlen(prefix))) ;

    if (new_prefix_layer == NULL) return ;

    if (new_prefix_source == NULL) return ;

    sprintf (new_prefix_layer,  "%s    layer", prefix) ;
    sprintf (new_prefix_source, "%s   source", prefix) ;

    fprintf (stream, "%sdie %s :\n", prefix, die->Id) ;

    FOR_EVERY_ELEMENT_IN_LIST_PREV (Layer, layer, die->TopLayer)
    {
        if (layer == die->SourceLayer)

            print_formatted_layer (stream, new_prefix_source, layer) ;

        else

            print_formatted_layer (stream, new_prefix_layer, layer) ;
    }

    FREE_POINTER (free, new_prefix_layer) ;
    FREE_POINTER (free, new_prefix_source) ;
}

/******************************************************************************/

void print_formatted_dies_list (FILE  *stream, String_t prefix, Die *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_formatted_die (stream, prefix, die) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_die (stream, prefix, die) ;
}

/******************************************************************************/

void print_detailed_die (FILE  *stream, String_t prefix, Die *die)
{
    String_t new_prefix = (String_t)

        malloc (sizeof (*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
        "%sdie                         = %p\n",
        prefix, die) ;

    fprintf (stream,
        "%s  Id                        = %s\n",
        prefix, die->Id) ;

    fprintf (stream,
        "%s  Used                      = %d\n",
        prefix, die->Used) ;

    fprintf (stream,
        "%s  NLayers                   = %d\n",
        prefix, die->NLayers) ;

    fprintf (stream,
        "%s  SourceLayerOffset         = %d\n",
        prefix, die->SourceLayerOffset) ;

    fprintf (stream,
        "%s  TopLayer                  = %p\n",
        prefix, die->TopLayer) ;

    fprintf (stream,
        "%s  SourceLayer               = %p\n",
        prefix, die->SourceLayer) ;

    fprintf (stream,
        "%s  BottomLayer               = %p\n",
        prefix, die->BottomLayer) ;

    fprintf (stream, "%s\n", prefix) ;

    print_detailed_layers_list (stream, new_prefix, die->BottomLayer) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream,
        "%s  Next                      = %p\n",
        prefix, die->Next) ;

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_detailed_dies_list (FILE  *stream, String_t prefix, Die *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_detailed_die (stream, prefix, die) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_die (stream, prefix, die) ;
}

/******************************************************************************/

void fill_thermal_cell_die
(
    ThermalCell *thermal_cells,
    Time_t       delta_time,
    Dimensions  *dimensions,
    CellIndex_t  layer_index,
    Die         *die
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer, layer, die->BottomLayer)

        fill_thermal_cell_layer

            (thermal_cells, delta_time, dimensions, layer_index++, layer) ;
}

/******************************************************************************/

Error_t fill_sources_die
(
    Source_t   *sources,
    Dimensions *dimensions,
    CellIndex_t layer_index,
    Floorplan  *floorplan,
    Die        *die
)
{
#ifdef PRINT_SOURCES
    fprintf (stderr,
        "layer_index = %d\tfill_sources_die %s floorplan %s\n",
        layer_index, die->Id, floorplan->FileName) ;
#endif

    layer_index += die->SourceLayerOffset ;

    sources += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

    return fill_sources_floorplan (sources, dimensions, floorplan) ;
}

/******************************************************************************/

SystemMatrix fill_system_matrix_die
(
    Die          *die,
    Dimensions   *dimensions,
    ThermalCell  *thermal_cells,
    CellIndex_t   layer_index,
    SystemMatrix  system_matrix
)
{
# ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "(l %2d) fill_system_matrix_die\n", layer_index) ;
# endif

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer, layer, die->BottomLayer)

        system_matrix = fill_system_matrix_layer

            (dimensions, thermal_cells, layer_index++, system_matrix) ;

    return system_matrix ;
}

/******************************************************************************/
