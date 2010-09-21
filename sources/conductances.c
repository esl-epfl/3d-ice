/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                           3d-ice@listes.epfl.ch *
 * 1015 Lausanne, Switzerland                  http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include "conductances.h"

/******************************************************************************/

/*
 * "FlowRate[um3/sec]" = ( "FlowRate[ml/min]" * 1e+12 ) / 60.0
 *
 * FlowRatePerChannel [ um3 / sec ] = FlowRate             [ um3 / sec ]
 *                                    / #ChannelColumns    [ ]
 *
 * CoolantVelocity      [ m / sec ] = FlowRatePerChannel   [ um3 / sec ]
 *                                    * ( 1 / Ay )         [   1 / um2 ]
 *
 * Cconv         [ J / ( K . sec) ]  = CoolantVHC          [ J / ( um3 . K ) ]
 *                                     * CoolantVelocity   [ m / sec ]
 *                                     * ( Ay / 2 )        [ um2 ]
 * [ J / ( K . sec) ] = [ W / K ]
 *
 * CoolantVelocity = FlowRate / (#ChannelColumns * Ay )
 *
 * Cconv           = (CoolantVHC * FlowRate) / (#ChannelColumns * 2)
 */

Cconv_t get_c_conv
(
  ColumnIndex_t number_of_columns,
  CoolantVHC_t  coolant_vhc,
  CoolantFR_t   coolant_fr
)
{
  return (coolant_vhc * coolant_fr)
         /
         (Cconv_t) (number_of_columns - 1 );
}

/******************************************************************************/

void fill_conductances_bottom_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*           dimensions,
  LayerIndex_t          current_layer,
  RowIndex_t            current_row,
  ColumnIndex_t         current_column,
# endif
  Conductances*         conductances,
  CellDimension_t       cell_length,
  CellDimension_t       cell_width,
  CellDimension_t       cell_height,
  ThermalConductivity_t thermal_conductivity,
  EnvironmentHTC_t      environment_heat_transfer_coefficient
)
{
  /* To avoid unused parameter warning message */
  (void) environment_heat_transfer_coefficient;

  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Bottom = (Conductance_t) 0 ;

  conductances->Top
    = (thermal_conductivity * cell_length * cell_width) / cell_height ;

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr,
    "solid  bottom  | l %2d r %4d c %4d [%6d] " \
                   "| l %5.2f w %5.2f h %5.2f " \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions,
                              current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
# endif
}

/******************************************************************************/

void fill_conductances_central_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*           dimensions,
  LayerIndex_t          current_layer,
  RowIndex_t            current_row,
  ColumnIndex_t         current_column,
# endif
  Conductances*         conductances,
  CellDimension_t       cell_length,
  CellDimension_t       cell_width,
  CellDimension_t       cell_height,
  ThermalConductivity_t thermal_conductivity,
  EnvironmentHTC_t      environment_heat_transfer_coefficient
)
{
  /* To avoid unused parameter warning message */
  (void) environment_heat_transfer_coefficient;

  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Top = conductances->Bottom
    = (thermal_conductivity * cell_length * cell_width )
      / (cell_height / (CellDimension_t) 2);

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr,
    "solid  central | l %2d r %4d c %4d [%6d] "  \
                   "| l %5.2f w %5.2f h %5.2f "  \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions,
                              current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
# endif
}

/******************************************************************************/

void fill_conductances_top_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*           dimensions,
  LayerIndex_t          current_layer,
  RowIndex_t            current_row,
  ColumnIndex_t         current_column,
# endif
  Conductances*         conductances,
  CellDimension_t       cell_length,
  CellDimension_t       cell_width,
  CellDimension_t       cell_height,
  ThermalConductivity_t thermal_conductivity,
  EnvironmentHTC_t      environment_heat_transfer_coefficient
)
{
  /* To avoid unused parameter warning message */
  (void) environment_heat_transfer_coefficient;

  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Bottom
    = (thermal_conductivity * cell_length * cell_width) / cell_height ;

  conductances->Top = (Conductance_t) 0 ;

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr,
    "solid  top     | l %2d r %4d c %4d [%6d] "  \
                   "| l %5.2f w %5.2f h %5.2f "  \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions,
                              current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
# endif
}

/******************************************************************************/

void fill_conductances_top_solid_cell_ehtc
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*           dimensions,
  LayerIndex_t          current_layer,
  RowIndex_t            current_row,
  ColumnIndex_t         current_column,
# endif
  Conductances*         conductances,
  CellDimension_t       cell_length,
  CellDimension_t       cell_width,
  CellDimension_t       cell_height,
  ThermalConductivity_t thermal_conductivity,
  EnvironmentHTC_t      environment_heat_transfer_coefficient
)
{
  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Bottom
    = (thermal_conductivity * cell_length * cell_width)
      / (cell_height / (CellDimension_t) 2) ;

  conductances->Top
    = (
        (Conductance_t) 2 * thermal_conductivity
          * environment_heat_transfer_coefficient
          * cell_length * cell_width
      )
      /
      (
        cell_height * environment_heat_transfer_coefficient
        +
        (Conductance_t) 2 * thermal_conductivity
      ) ;

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr,
    "solid top ehtc | l %2d r %4d c %4d [%6d] "  \
                   "| l %5.2f w %5.2f h %5.2f "  \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions,
                              current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
# endif
}

/******************************************************************************/

void fill_conductances_liquid_cell
(
# ifdef PRINT_CONDUCTANCES
  LayerIndex_t    current_layer,
  RowIndex_t      current_row,
  ColumnIndex_t   current_column,
# endif
  Dimensions*     dimensions,
  Conductances*   conductances,
  CellDimension_t cell_length,
  CellDimension_t cell_width,
  CellDimension_t cell_height,
  CoolantHTCs_t   coolant_htcs,
  CoolantVHC_t    coolant_vhc,
  CoolantFR_t     coolant_fr
)
{
  Cconv_t C = get_c_conv (get_number_of_columns (dimensions),
                          coolant_vhc, coolant_fr) ;

  conductances->North =  C ;
  conductances->South = -C ;

  conductances->East = conductances->West
    = coolant_htcs.Side * cell_width * cell_height ;

  conductances->Top = coolant_htcs.Top * cell_width * cell_length ;

  conductances->Bottom = coolant_htcs.Bottom * cell_width * cell_length ;

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr,
    "liquid         | l %2d r %4d c %4d [%6d] "   \
                   "| l %5.2f w %5.2f h %5.2f "   \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions,
                              current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
# endif
}

/******************************************************************************/

void fill_conductances_wall_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*           dimensions,
  LayerIndex_t          current_layer,
  RowIndex_t            current_row,
  ColumnIndex_t         current_column,
# endif
  Conductances*         conductances,
  CellDimension_t       cell_length,
  CellDimension_t       cell_width,
  CellDimension_t       cell_height,
  ThermalConductivity_t thermal_conductivity
)
{
  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Top = conductances->Bottom
    = (thermal_conductivity * cell_length * cell_width )
      / (cell_height / (CellDimension_t) 2);

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr,
    "wall           | l %2d r %4d c %4d [%6d] "  \
                   "| l %5.2f w %5.2f h %5.2f "  \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions,
                              current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
# endif
}

/******************************************************************************/
