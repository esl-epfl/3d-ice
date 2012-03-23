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

#include "layer.h"
#include "macros.h"

/******************************************************************************/

void init_layer (Layer *layer)
{
    layer->Height   = 0.0 ;
    layer->Material = NULL ;
    layer->Next     = NULL ;
    layer->Prev     = NULL ;
}

/******************************************************************************/

Layer *alloc_and_init_layer (void)
{
    Layer *layer = (Layer *) malloc (sizeof(Layer));

    if (layer != NULL)

        init_layer (layer) ;

    return layer ;
}

/******************************************************************************/

void free_layer (Layer *layer)
{
    FREE_POINTER (free, layer) ;
}

/******************************************************************************/

void free_layers_list (Layer *list)
{
    FREE_LIST (Layer, list, free_layer) ;
}

/******************************************************************************/

void print_formatted_layer (FILE *stream, String_t prefix, Layer *layer)
{
    fprintf (stream,
        "%s %7.1f  %s ;\n",
        prefix, layer->Height, layer->Material->Id) ;
}

/******************************************************************************/

void print_detailed_layer (FILE *stream, String_t prefix, Layer *layer)
{
    fprintf (stream,
        "%slayer                   = %p\n",
        prefix, layer) ;

    fprintf (stream,
        "%s  Height                = %.1f\n",
        prefix, layer->Height) ;

    fprintf (stream,
        "%s  Material              = %p\n",
        prefix, layer->Material) ;

    fprintf (stream,
        "%s  Next                  = %p\n",
        prefix, layer->Next) ;

    fprintf (stream,
        "%s  Prev                  = %p\n",
        prefix, layer->Prev) ;
}

/******************************************************************************/

void print_formatted_layers_list (FILE *stream, String_t prefix, Layer *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer, layer, list)

        print_formatted_layer (stream, prefix, layer) ;
}

/******************************************************************************/

void print_detailed_layers_list (FILE *stream, String_t prefix, Layer *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer, layer, list)
    {
        if (layer->Next == NULL)

            break ;

        print_detailed_layer (stream, prefix, layer) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_layer (stream, prefix, layer) ;
}

/******************************************************************************/

void fill_thermal_cell_layer
(
    ThermalCell *thermal_cells,
    Time_t       delta_time,
    Dimensions  *dimensions,
    CellIndex_t  layer_index,
    Layer       *layer
)
{
    void (*fill_cell)

        (ThermalCell *, Time_t,
         CellDimension_t, CellDimension_t, CellDimension_t,
         SolidTC_t, SolidVHC_t) ;

    if (IS_FIRST_LAYER (layer_index))

        fill_cell = fill_solid_cell_bottom ;

    else if (IS_LAST_LAYER (layer_index, dimensions))

        fill_cell = fill_solid_cell_top ;

    else

        fill_cell = fill_solid_cell_central ;

#ifdef PRINT_THERMAL_CELLS
    CellIndex_t cell_index

        = get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;
#endif

    thermal_cells += layer_index * get_number_of_columns (dimensions) ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
#ifdef PRINT_THERMAL_CELLS
        fprintf (stderr,
            "  l %2d c %4d [%7d] ",
            layer_index, column_index, cell_index++) ;
#endif

        fill_cell (thermal_cells, delta_time,

                   get_cell_length(dimensions, column_index),
                   get_cell_width(dimensions, 0),
                   layer->Height,

                   layer->Material->ThermalConductivity,
                   layer->Material->VolumetricHeatCapacity) ;

        thermal_cells ++ ;
    }
}

/******************************************************************************/

SystemMatrix fill_system_matrix_layer
(
    Dimensions   *dimensions,
    ThermalCell  *thermal_cells,
    CellIndex_t   layer_index,
    SystemMatrix  system_matrix
)
{
#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "(l %2d) fill_system_matrix_layer \n", layer_index) ;
#endif

    FOR_EVERY_ROW (row_index, dimensions)
    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            system_matrix = add_solid_column

                (dimensions, thermal_cells,
                 layer_index, row_index, column_index, system_matrix) ;

        } // FOR_EVERY_COLUMN
    } // FOR_EVERY_ROW

    return system_matrix ;
}

/******************************************************************************/
