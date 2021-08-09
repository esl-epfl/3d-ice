/******************************************************************************
 * This file is part of 3D-ICE, version 3.0.0 .                               *
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
 *          Federico Terraneo                                                 *
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
#include <string.h> // For the memory function memcpy
#include <assert.h>

#include "dimensions.h"
#include "heat_sink.h"

/******************************************************************************/

void cell_dimensions_init (CellDimensions_t *celld)
{
    celld->FirstWallLength = (CellDimension_t) 0.0 ;
    celld->LastWallLength  = (CellDimension_t) 0.0 ;
    celld->WallLength      = (CellDimension_t) 0.0 ;
    celld->ChannelLength   = (CellDimension_t) 0.0 ;
    celld->Width           = (CellDimension_t) 0.0 ;
    celld->NHeights        = (Quantity_t) 0u ;
    celld->Heights         = NULL ;
}

/******************************************************************************/

void cell_dimensions_copy (CellDimensions_t *dst, CellDimensions_t *src)
{
    cell_dimensions_destroy (dst) ;

    dst->FirstWallLength = src->FirstWallLength ;
    dst->LastWallLength  = src->LastWallLength ;
    dst->WallLength      = src->WallLength ;
    dst->ChannelLength   = src->ChannelLength ;
    dst->Width           = src->Width ;
    dst->NHeights        = src->NHeights ;

    if (src->Heights == NULL)
    {
        dst->Heights = NULL ;

        return ;
    }

    dst->Heights = (CellDimension_t *)

        malloc (src->NHeights * sizeof (CellDimension_t)) ;

    if (dst->Heights == NULL)
    {
        fprintf (stderr, "Malloc heights error\n") ;

        return ;
    }

    memcpy (dst->Heights, src->Heights,
            src->NHeights * sizeof (CellDimension_t) ) ;
}

/******************************************************************************/

void cell_dimensions_destroy (CellDimensions_t *celld)
{
    if (celld->Heights != NULL)

        free (celld->Heights) ;

    cell_dimensions_init (celld) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void grid_dimensions_init (GridDimensions_t *gridd)
{
    gridd->NLayers      = (CellIndex_t) 0u ;
    gridd->NRows        = (CellIndex_t) 0u ;
    gridd->NColumns     = (CellIndex_t) 0u ;
    gridd->NCells       = (CellIndex_t) 0u ;
    gridd->NConnections = (CellIndex_t) 0u ;
}

/******************************************************************************/

void grid_dimensions_copy (GridDimensions_t *dst, GridDimensions_t *src)
{
    grid_dimensions_destroy (dst) ;

    dst->NLayers      = src->NLayers ;
    dst->NRows        = src->NRows ;
    dst->NColumns     = src->NColumns ;
    dst->NCells       = src->NCells ;
    dst->NConnections = src->NConnections ;
}

/******************************************************************************/

void grid_dimensions_destroy (GridDimensions_t *gridd)
{
    // Nothing to do ...

    grid_dimensions_init (gridd) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void chip_dimensions_init (ChipDimensions_t *chipd)
{
    chipd->Length = (CellDimension_t) 0.0 ;
    chipd->Width  = (CellDimension_t) 0.0 ;
}

/******************************************************************************/

void chip_dimensions_copy (ChipDimensions_t *dst, ChipDimensions_t *src)
{
    chip_dimensions_destroy (dst) ;

    dst->Length = src->Length ;
    dst->Width  = src->Width ;
}

/******************************************************************************/

void chip_dimensions_destroy (ChipDimensions_t *chipd)
{
    // Nothing to do ...

    chip_dimensions_init (chipd) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void dimensions_init (Dimensions_t *dimensions)
{
    cell_dimensions_init (&dimensions->Cell) ;
    grid_dimensions_init (&dimensions->Grid) ;
    chip_dimensions_init (&dimensions->Chip) ;

    dimensions->NonUniform = 0;
    dimensions->Discr_X = 0;
    dimensions->Discr_Y = 0;

    non_uniform_cell_list_init(&dimensions->Cell_list);
    connection_list_init(&dimensions->connections_list);
}

/******************************************************************************/

void dimensions_copy (Dimensions_t *dst, Dimensions_t *src)
{
    dimensions_destroy (dst) ;
    dst->NonUniform = src->NonUniform;
    dst->Discr_X = src->Discr_X;
    dst->Discr_Y = src->Discr_Y;
    cell_dimensions_copy (&dst->Cell, &src->Cell) ;
    grid_dimensions_copy (&dst->Grid, &src->Grid) ;
    chip_dimensions_copy (&dst->Chip, &src->Chip) ;

    non_uniform_cell_list_copy(&dst->Cell_list, &src->Cell_list) ;
    connection_list_copy(&dst->connections_list, &src->connections_list) ;
}

/******************************************************************************/

void dimensions_destroy (Dimensions_t *dimensions)
{
    cell_dimensions_destroy (&dimensions->Cell) ;
    grid_dimensions_destroy (&dimensions->Grid) ;
    chip_dimensions_destroy (&dimensions->Chip) ;

    non_uniform_cell_list_destroy(&dimensions->Cell_list);
    connection_list_destroy(&dimensions->connections_list);
    
    dimensions_init (dimensions) ;
}

/******************************************************************************/

Dimensions_t *dimensions_calloc (void)
{
    Dimensions_t *dimensions = (Dimensions_t *) malloc (sizeof(Dimensions_t)) ;

    if (dimensions != NULL)

        dimensions_init (dimensions) ;

    return dimensions ;
}

/******************************************************************************/

Dimensions_t *dimensions_clone (Dimensions_t *dimensions)
{
    if (dimensions == NULL)

        return NULL ;

    Dimensions_t *newd = dimensions_calloc ( ) ;

    if (newd != NULL)

        dimensions_copy (newd, dimensions) ;

    return newd ;
}

/******************************************************************************/

void dimensions_free (Dimensions_t *dimensions)
{
    if (dimensions == NULL)

        return ;

    dimensions_destroy (dimensions) ;

    free (dimensions) ;
}

/******************************************************************************/

void dimensions_print
(
    Dimensions_t *dimensions,
    FILE         *stream,
    String_t      prefix
)
{
    fprintf (stream,
            "%sdimensions : \n",
            prefix) ;

    fprintf (stream,
            "%s   chip length %7.1f , width %7.1f ;\n",
            prefix, dimensions->Chip.Length, dimensions->Chip.Width) ;

    fprintf (stream,
            "%s   cell length %7.1f , width %7.1f ;\n",
            prefix, dimensions->Cell.WallLength, dimensions->Cell.Width) ;
}

/******************************************************************************/

void print_axes (Dimensions_t *dimensions)
{
    FILE *file = fopen ("xaxis.txt", "w") ;

    if (file == NULL)
    {
        fprintf (stderr, "Cannot create text file for x axis\n") ;
        return ;
    }

    CellIndex_t column ;

    for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)

        fprintf (file, "%5.2f\n", get_cell_center_x (dimensions, column)) ;

    fclose (file) ;

    file = fopen ("yaxis.txt", "w") ;

    if (file == NULL)
    {
        fprintf (stderr, "Cannot create text file for y axis\n") ;
        return ;
    }

    CellIndex_t row ;

    for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)

        fprintf (file, "%5.2f\n", get_cell_center_y (dimensions, row)) ;

    fclose (file) ;
}

/******************************************************************************/

void compute_number_of_connections
(
    Dimensions_t   *dimensions,
    Quantity_t      num_channels,
    ChannelModel_t  channel_model,
    HeatSink_t     *topSink
)
{
    CellIndex_t nlayers  = dimensions->Grid.NLayers ;
    CellIndex_t nrows    = dimensions->Grid.NRows ;
    CellIndex_t ncolumns = dimensions->Grid.NColumns ;

    CellIndex_t nlayers_for_channel    = num_channels * NUM_LAYERS_CHANNEL_2RM ;
    CellIndex_t nlayers_except_channel = nlayers - nlayers_for_channel ;

    CellIndex_t tmp = 2u ;

    switch (channel_model)
    {
        case TDICE_CHANNEL_MODEL_NONE :
        case TDICE_CHANNEL_MODEL_MC_4RM :
        {
            dimensions->Grid.NConnections =

                // All Normal Cells

                // number of coefficients in the diagonal
                nlayers * nrows * ncolumns
                +
                // number of coefficients bottom <-> top
                2 * (nlayers - 1) * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                2 * nlayers * (nrows - 1) * ncolumns
                +
                // Number of coefficients East <-> West
                2 * nlayers * nrows * (ncolumns - 1) ;

            break ;
        }
        case TDICE_CHANNEL_MODEL_MC_2RM :

            tmp += 2 ;
            // fallthrough

        case TDICE_CHANNEL_MODEL_PF_INLINE :
        case TDICE_CHANNEL_MODEL_PF_STAGGERED :
        {
            dimensions->Grid.NConnections =

                // For Normal Cells

                // Number of coefficients in the diagonal
                nlayers_except_channel * nrows * ncolumns
                +
                // Number of coefficients Bottom <-> Top
                2 * nlayers_except_channel * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                2 * nlayers_except_channel * (nrows - 1) * ncolumns
                +
                // Number of coefficients East <-> West
                2 * nlayers_except_channel * nrows * (ncolumns - 1)
                +

                // For Channel Cells

                // Number of coefficients in the diagonal
                nlayers_for_channel * nrows * ncolumns
                +
                // Number of coefficients Bottom <-> Top
                2 * (nlayers_for_channel + num_channels) * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                tmp * num_channels * (nrows - 1) * ncolumns
                +
                // Number of coefficients East <-> West
                0
                ;

            break ;
        }
        default :

            fprintf (stderr, "Error: unknown channel model %d\n", channel_model) ;
    }
    
    if(topSink && topSink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)
    {
        // Add the heat spreader, which has a different size and is thus not
        // counted among the layers
        dimensions->Grid.NConnections +=

                // number of coefficients in the diagonal
                topSink->NRows * topSink->NColumns
                +
                // Number of coefficients North <-> South
                2 * (topSink->NRows - 1) * topSink->NColumns
                +
                // Number of coefficients East <-> West
                2 * topSink->NRows * (topSink->NColumns - 1) ;
        
        
        //Then add the connections between the last layer of the stack and
        //the middle of the spreader
        dimensions->Grid.NConnections +=

                // number of coefficients bottom <-> top
                2 * nrows * ncolumns ;
    }
}

/******************************************************************************/

CellIndex_t first_row (Dimensions_t __attribute__ ((unused)) *dimensions)
{
    return 0u ;
}

/******************************************************************************/

CellIndex_t last_row (Dimensions_t *dimensions)
{
    return get_number_of_rows (dimensions) - 1 ;
}

/******************************************************************************/

CellIndex_t first_column (Dimensions_t __attribute__ ((unused)) *dimensions)
{
    return 0u ;
}

/******************************************************************************/

CellIndex_t last_column (Dimensions_t *dimensions)
{
    return get_number_of_columns (dimensions) - 1 ;
}

/******************************************************************************/

CellIndex_t first_layer (Dimensions_t __attribute__ ((unused)) *dimensions)
{
    return 0u ;
}

/******************************************************************************/

CellIndex_t last_layer (Dimensions_t *dimensions)
{
    return get_number_of_layers (dimensions) - 1 ;
}

/******************************************************************************/

CellDimension_t get_cell_length
(
  Dimensions_t *dimensions,
  CellIndex_t   column_index
)
{
    // column_index < 0 not tested since CellIndex_t is unsigned

    if (column_index > last_column (dimensions))
    {
        fprintf (stderr,
            "ERROR: column index %d is out of range\n", column_index) ;

        return 0.0 ;
    }

    if (column_index == first_column (dimensions))

        return dimensions->Cell.FirstWallLength ;

    else if (column_index == last_column (dimensions))

        return dimensions->Cell.LastWallLength ;

    else

        if (column_index & 1)

        return dimensions->Cell.ChannelLength ;

        else

        return dimensions->Cell.WallLength ;
}

/******************************************************************************/

CellDimension_t get_cell_width
(
    Dimensions_t *dimensions,
    CellIndex_t   row_index
)
{
    // column_index < 0 not tested since CellIndex_t is unsigned

    if (row_index > last_row (dimensions))
    {
        fprintf (stderr,
            "ERROR: row index %d is out of range\n", row_index) ;

        return 0.0 ;
    }

    return dimensions->Cell.Width ;
}

/******************************************************************************/

CellDimension_t get_cell_height
(
    Dimensions_t *dimensions,
    CellIndex_t   layer_index
)
{
    if (dimensions->Cell.Heights == NULL)
    {
        fprintf (stderr, "ERROR: vector of heigths does not exist in memory\n") ;

        return 0.0 ;
    }

    // layer_index < 0 not tested since CellIndex_t is unsigned

    if (layer_index > dimensions->Cell.NHeights)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    return dimensions->Cell.Heights [ layer_index ] ;
}

/******************************************************************************/

ChipDimension_t get_cell_center_x
(
    Dimensions_t *dimensions,
    CellIndex_t   column_index
)
{
    if (column_index == first_column (dimensions))

        return dimensions->Cell.FirstWallLength / 2.0 ;

    else if (column_index == last_column (dimensions))

        return   (dimensions->Cell.FirstWallLength      )
               + (dimensions->Cell.ChannelLength   / 2.0) * (column_index    )
               + (dimensions->Cell.WallLength      / 2.0) * (column_index - 2)
               + (dimensions->Cell.LastWallLength  / 2.0) ;

    else

        return   (dimensions->Cell.FirstWallLength      )
               + (dimensions->Cell.ChannelLength   / 2.0) * (column_index    )
               + (dimensions->Cell.WallLength      / 2.0) * (column_index - 1) ;
}

/******************************************************************************/

ChipDimension_t get_cell_center_y
(
    Dimensions_t *dimensions,
    CellIndex_t   row_index
)
{
    return    dimensions->Cell.Width / 2.0
           +  dimensions->Cell.Width * row_index ;
}

/******************************************************************************/

ChipDimension_t get_cell_location_x
(
    Dimensions_t *dimensions,
    CellIndex_t   column_index
)
{
    if (column_index == first_column (dimensions))

        return 0.0 ;

    else

        return   (dimensions->Cell.FirstWallLength)
               + (dimensions->Cell.ChannelLength  ) * ((column_index    ) / 2u)
               + (dimensions->Cell.WallLength     ) * ((column_index - 1) / 2u) ;
}

/******************************************************************************/

ChipDimension_t get_cell_location_y
(
    Dimensions_t *dimensions,
    CellIndex_t   row_index
)
{
    return dimensions->Cell.Width * row_index ;
}

/******************************************************************************/

CellIndex_t get_number_of_layers (Dimensions_t *dimensions)
{
    return dimensions->Grid.NLayers ;
}

/******************************************************************************/

CellIndex_t get_number_of_rows (Dimensions_t *dimensions)
{
    return dimensions->Grid.NRows ;
}

/******************************************************************************/

CellIndex_t get_number_of_columns (Dimensions_t *dimensions)
{
    return dimensions->Grid.NColumns ;
}

/******************************************************************************/

CellIndex_t get_number_of_cells (Dimensions_t *dimensions)
{
    return dimensions->Grid.NCells ;
}

/******************************************************************************/

CellIndex_t get_number_of_connections (Dimensions_t *dimensions)
{
    return dimensions->Grid.NConnections ;
}

/******************************************************************************/

CellIndex_t get_layer_area (Dimensions_t *dimensions)
{
    return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

CellIndex_t get_cell_offset_in_layer
(
    Dimensions_t *dimensions,
    CellIndex_t   row_index,
    CellIndex_t   column_index
)
{
    return row_index * get_number_of_columns (dimensions) + column_index ;
}

/******************************************************************************/

CellIndex_t get_cell_offset_in_stack
(
    Dimensions_t *dimensions,
    CellIndex_t   layer_index,
    CellIndex_t   row_index,
    CellIndex_t   column_index
)
{
    return layer_index * get_layer_area (dimensions)
           + get_cell_offset_in_layer (dimensions, row_index, column_index) ;
}

/******************************************************************************/

ChipDimension_t get_chip_length (Dimensions_t *dimensions)
{
    return dimensions->Chip.Length ;
}

/******************************************************************************/

ChipDimension_t get_chip_width (Dimensions_t *dimensions)
{
    return dimensions->Chip.Width ;
}

/******************************************************************************/

ChipDimension_t get_chip_area (Dimensions_t *dimensions)
{
    return get_chip_length (dimensions) * get_chip_width (dimensions) ;
}

/******************************************************************************/

CellIndex_t get_spreader_cell_offset
(
    Dimensions_t *dimensions,
    HeatSink_t   *hsink,
    CellIndex_t   row_index,
    CellIndex_t   column_index
)
{
    assert(hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE);
    
    if(dimensions->NonUniform == 1) //recaculate spreder offset in the non-uniform scenario
    {
        return dimensions->Grid.NCells - hsink->NColumns*hsink->NRows;
    }
    else
    {
        return (   get_number_of_layers (dimensions)
                * get_number_of_rows (dimensions)
                * get_number_of_columns (dimensions))
                + row_index * hsink->NColumns + column_index;
    }

}

/******************************************************************************/

CellIndex_t get_spreader_cell_offset_from_layer_coordinates
(
    Dimensions_t *dimensions,
    HeatSink_t   *hsink,
    CellIndex_t   row_index,
    CellIndex_t   column_index
)
{
    return get_spreader_cell_offset(
                dimensions,
                hsink,
                row_index + hsink->NumRowsBorder,
                column_index + hsink->NumColumnsBorder);
}

/******************************************************************************/

bool has_layer_underneath
(
    Dimensions_t *dimensions,
    HeatSink_t   *hsink,
    CellIndex_t   row_index,
    CellIndex_t   column_index
)
{
    assert(hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE);
    
    if(    row_index    >= hsink->NumRowsBorder
        && row_index    <  hsink->NumRowsBorder    + get_number_of_rows (dimensions)
        && column_index >= hsink->NumColumnsBorder
        && column_index <  hsink->NumColumnsBorder + get_number_of_columns (dimensions))
        return true;
    else
        return false;
}

CellIndex_t get_layer_cell_offset_from_spreader_coordinates
(
    Dimensions_t *dimensions,
    HeatSink_t   *hsink,
    CellIndex_t   layer_index,
    CellIndex_t   row_index,
    CellIndex_t   column_index
)
{
    assert(has_layer_underneath(dimensions, hsink, row_index, column_index));
    
    return get_cell_offset_in_stack(
                dimensions,
                layer_index,
                row_index    - hsink->NumRowsBorder,
                column_index - hsink->NumColumnsBorder);
}

/******************************************************************************/

