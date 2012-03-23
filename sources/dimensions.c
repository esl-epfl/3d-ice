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

#include "dimensions.h"
#include "macros.h"

/******************************************************************************/

void init_dimensions (Dimensions *dimensions)
{
    dimensions->Cell.FirstWallLength = 0.0 ;
    dimensions->Cell.LastWallLength  = 0.0 ;
    dimensions->Cell.WallLength      = 0.0 ;
    dimensions->Cell.ChannelLength   = 0.0 ;

    dimensions->Cell.Width = 0.0 ;

    dimensions->Grid.NLayers      = 0u ;
    dimensions->Grid.NRows        = 0u ;
    dimensions->Grid.NColumns     = 0u ;
    dimensions->Grid.NCells       = 0u ;
    dimensions->Grid.NConnections = 0u ;

    dimensions->Chip.Length = 0.0 ;
    dimensions->Chip.Width  = 0.0 ;
}

/******************************************************************************/

Dimensions *alloc_and_init_dimensions (void)
{
  Dimensions *dimensions = malloc (sizeof(Dimensions)) ;

  if (dimensions != NULL) init_dimensions (dimensions) ;

  return dimensions ;
}

/******************************************************************************/

void print_formatted_dimensions
(
  FILE       *stream,
  String_t    prefix,
  Dimensions *dimensions
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

void print_detailed_dimensions
(
  FILE       *stream,
  String_t    prefix,
  Dimensions *dimensions
)
{
  fprintf (stream,
           "%sdimensions                  = %p\n",
           prefix, dimensions) ;

  fprintf (stream,
           "%s  Cell.FirstWallLength      = %.1f\n",
           prefix, dimensions->Cell.FirstWallLength) ;

  fprintf (stream,
           "%s  Cell.WallLength           = %.1f\n",
           prefix, dimensions->Cell.WallLength) ;

  fprintf (stream,
           "%s  Cell.LastWallLength       = %.1f\n",
           prefix, dimensions->Cell.LastWallLength) ;

  fprintf (stream,
           "%s  Cell.ChannelLength        = %.1f\n",
           prefix, dimensions->Cell.ChannelLength) ;

  fprintf (stream,
           "%s  Cell.Width                = %.1f\n",
           prefix, dimensions->Cell.Width) ;

  fprintf (stream,
           "%s  Grid.NLayers              = %d\n",
           prefix, dimensions->Grid.NLayers) ;

  fprintf (stream,
           "%s  Grid.NRows                = %d\n",
           prefix, dimensions->Grid.NRows) ;

  fprintf (stream,
           "%s  Grid.NColumns             = %d\n",
           prefix, dimensions->Grid.NColumns) ;

  fprintf (stream,
           "%s  Grid.NCells               = %d\n",
           prefix, dimensions->Grid.NCells) ;

  fprintf (stream,
           "%s  Grid.Nconnections         = %d\n",
           prefix, dimensions->Grid.NConnections) ;

  fprintf (stream,
           "%s  Chip.Length               = %.1f\n",
           prefix, dimensions->Chip.Length) ;

  fprintf (stream,
           "%s  Chip.Width                = %.1f\n",
           prefix, dimensions->Chip.Width) ;
}

/******************************************************************************/

void free_dimensions (Dimensions *dimensions)
{
  FREE_POINTER (free, dimensions) ;
}

/******************************************************************************/

void print_axes (Dimensions *dimensions)
{
    FILE *file = fopen ("xaxis.txt", "w") ;

    if (file == NULL)
    {
        fprintf (stderr, "Cannot create text file for x axis\n") ;
        return ;
    }

    FOR_EVERY_COLUMN (column_index, dimensions)

        fprintf (file, "%5.2f\n", get_cell_center_x (dimensions, column_index)) ;

    fclose (file) ;

    file = fopen ("yaxis.txt", "w") ;

    if (file == NULL)
    {
        fprintf (stderr, "Cannot create text file for y axis\n") ;
        return ;
    }

    FOR_EVERY_ROW (row_index, dimensions)

        fprintf (file, "%5.2f\n", get_cell_center_y (dimensions, row_index)) ;

    fclose (file) ;
}

/******************************************************************************/

void compute_number_of_connections
(
    Dimensions     *dimensions,
    Quantity_t      num_channels,
    ChannelModel_t  channel_model
)
{
    CellIndex_t nlayers  = dimensions->Grid.NLayers ;
    CellIndex_t nrows    = dimensions->Grid.NRows ;
    CellIndex_t ncolumns = dimensions->Grid.NColumns ;

    CellIndex_t num_layers_for_channel    = num_channels * NUM_LAYERS_CHANNEL_2RM ;
    CellIndex_t num_layers_except_channel = nlayers - num_layers_for_channel ;

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
        case TDICE_CHANNEL_MODEL_PF_INLINE :
        case TDICE_CHANNEL_MODEL_PF_STAGGERED :
        {
            dimensions->Grid.NConnections =

                // For Normal Cells

                // Number of coefficients in the diagonal
                num_layers_except_channel * nrows * ncolumns
                +
                // Number of coefficients Bottom <-> Top
                2 * num_layers_except_channel * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                2 * num_layers_except_channel * (nrows - 1) * ncolumns
                +
                // Number of coefficients East <-> West
                2 * num_layers_except_channel * nrows * (ncolumns - 1)
                +

                // For Channel Cells

                // Number of coefficients in the diagonal
                num_layers_for_channel * nrows * ncolumns
                +
                // Number of coefficients Bottom <-> Top
                2 * (num_layers_for_channel + num_channels) * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                2 * num_channels * (nrows - 1 ) * ncolumns
                +
                // Number of coefficients East <-> West
                0
                ;

            break ;
        }
        case TDICE_CHANNEL_MODEL_MC_2RM :
        {
            dimensions->Grid.NConnections =

                // For Normal Cells

                // Number of coefficients in the diagonal
                num_layers_except_channel * nrows * ncolumns
                +
                // Number of coefficients Bottom <-> Top
                2 * num_layers_except_channel * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                2 * num_layers_except_channel * (nrows - 1) * ncolumns
                +
                // Number of coefficients East <-> West
                2 * num_layers_except_channel * nrows * (ncolumns - 1)
                +

                // For Channel Cells

                // Number of coefficients in the diagonal
                num_layers_for_channel * nrows * ncolumns
                +
                // Number of coefficients Bottom <-> Top
                2 * (num_layers_for_channel + num_channels) * nrows * ncolumns
                +
                // Number of coefficients North <-> South
                4 * num_channels * (nrows - 1) * ncolumns
                +
                // Number of coefficients East <-> West
                0
                ;

            break ;
        }
        default :

            fprintf (stderr, "Error: unknown channel model %d\n", channel_model) ;
    }
}

/******************************************************************************/

CellDimension_t get_cell_length
(
  Dimensions *dimensions,
  CellIndex_t column_index
)
{
  if (IS_FIRST_COLUMN (column_index))

    return dimensions->Cell.FirstWallLength ;

  else if (IS_LAST_COLUMN (column_index, dimensions))

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
    Dimensions *dimensions,
    CellIndex_t __attribute__ ((unused)) row_index
)
{
  return dimensions->Cell.Width ;
}

/******************************************************************************/

ChipDimension_t get_cell_center_x
(
    Dimensions *dimensions,
    CellIndex_t column_index
)
{
    if (IS_FIRST_COLUMN (column_index))

        return dimensions->Cell.FirstWallLength / 2.0 ;

    else if (IS_LAST_COLUMN (column_index, dimensions))

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
    Dimensions *dimensions,
    CellIndex_t row_index
)
{
    return    dimensions->Cell.Width / 2.0
           +  dimensions->Cell.Width * row_index ;
}

/******************************************************************************/

ChipDimension_t get_cell_location_x
(
    Dimensions *dimensions,
    CellIndex_t column_index
)
{
    if (IS_FIRST_COLUMN (column_index))

        return 0.0 ;

    else

        return   (dimensions->Cell.FirstWallLength)
               + (dimensions->Cell.ChannelLength  ) * ((column_index    ) / 2u)
               + (dimensions->Cell.WallLength     ) * ((column_index - 1) / 2u) ;
}

/******************************************************************************/

ChipDimension_t get_cell_location_y
(
    Dimensions *dimensions,
    CellIndex_t row_index
)
{
    return dimensions->Cell.Width * row_index ;
}

/******************************************************************************/

CellIndex_t get_number_of_layers (Dimensions *dimensions)
{
  return dimensions->Grid.NLayers ;
}

/******************************************************************************/

CellIndex_t get_number_of_rows (Dimensions *dimensions)
{
  return dimensions->Grid.NRows ;
}

/******************************************************************************/

CellIndex_t get_number_of_columns (Dimensions *dimensions)
{
  return dimensions->Grid.NColumns ;
}

/******************************************************************************/

CellIndex_t get_number_of_cells (Dimensions *dimensions)
{
  return dimensions->Grid.NCells ;
}

/******************************************************************************/

CellIndex_t get_number_of_connections (Dimensions *dimensions)
{
  return dimensions->Grid.NConnections ;
}

/******************************************************************************/

CellIndex_t get_layer_area (Dimensions *dimensions)
{
  return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

CellIndex_t get_cell_offset_in_layer
(
  Dimensions *dimensions,
  CellIndex_t row_index,
  CellIndex_t column_index
)
{
  return row_index * get_number_of_columns (dimensions) + column_index ;
}

/******************************************************************************/

CellIndex_t get_cell_offset_in_stack
(
  Dimensions *dimensions,
  CellIndex_t layer_index,
  CellIndex_t row_index,
  CellIndex_t column_index
)
{
  return layer_index * get_layer_area (dimensions)
         + get_cell_offset_in_layer (dimensions, row_index, column_index) ;
}

/******************************************************************************/

ChipDimension_t get_chip_length (Dimensions *dimensions)
{
  return dimensions->Chip.Length ;
}

/******************************************************************************/

ChipDimension_t get_chip_width (Dimensions *dimensions)
{
  return dimensions->Chip.Width ;
}

/******************************************************************************/
