/******************************************************************************
 *                                                                            *
 * Source file "Sources/conductances.c"                                       *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "conductances.h"

void
fill_conductances_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  RowIndex_t             current_row,
  ColumnIndex_t          current_column,
# endif
  Conductances*          conductances,
  Dimensions*            dimensions,
  CellDimension_t        cell_length,
  CellDimension_t        cell_width,
  CellDimension_t        cell_height,
  ThermalConductivity_t  thermal_conductivity,
  LayerIndex_t           current_layer
)
{
  LayerPosition_t cell_position
    = get_layer_position(dimensions, current_layer) ;

  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  if (cell_position == TL_LAYER_BOTTOM)
  {
    conductances->Bottom = (Conductance_t) 0 ;

    conductances->Top
      = (thermal_conductivity * cell_length * cell_width)
        / cell_height ;
  }
  else if (cell_position == TL_LAYER_TOP)
  {
#ifdef TL_NO_CHANNELS
    conductances->Top
      = 1/ (
              1/((thermal_conductivity * cell_length * cell_width) / (cell_height/2.0))
              +
              1/(1e-07 * cell_length * cell_width)
           );

    conductances->Bottom
      = (thermal_conductivity * cell_length * cell_width)
        / (cell_height / (CellDimension_t) 2) ;
#else
    conductances->Top = (Conductance_t) 0 ;

    conductances->Bottom
      = (thermal_conductivity * cell_length * cell_width)
        / cell_height ;
#endif
  }
  else
  {
    conductances->Top = conductances->Bottom
     = (thermal_conductivity * cell_length * cell_width)
       / (cell_height / (CellDimension_t) 2);
  }

#ifdef PRINT_CONDUCTANCES
  fprintf
  (
    stderr,
    "solid cell   |  l %2d r %4d c %4d [%6d] | l %5.2f w %5.2f h %5.2f "   \
                " |  N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions, current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom
  ) ;
#endif
}

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

void
fill_conductances_liquid_cell
(
# ifdef PRINT_CONDUCTANCES
  RowIndex_t             current_row,
  ColumnIndex_t          current_column,
# endif
  Conductances*          conductances,
  Dimensions*            dimensions,
  CellDimension_t        cell_length,
  CellDimension_t        cell_width,
  CellDimension_t        cell_height,
  CoolantHTC_t           coolant_htc,
  CoolantVHC_t           coolant_vhc,
  CoolantFR_t            coolant_fr,
  LayerIndex_t           current_layer
)
{
  enum LayerPosition_t cell_position
    = get_layer_position(dimensions, current_layer) ;

  double C = (coolant_vhc * coolant_fr)
             / (double) ( get_number_of_columns (dimensions) - 1 );

  if (cell_position == TL_LAYER_BOTTOM)
  {
    fprintf (stderr, "Warning: channel on bottom layer not supported\n") ;
  }
  else if (cell_position == TL_LAYER_TOP)
  {
    fprintf (stderr, "Warning: channel on top layer not supported\n") ;
  }

  conductances->North =  C ;
  conductances->South = -C ;

  conductances->East = conductances->West
    = coolant_htc * cell_width * cell_height ;

  conductances->Top = conductances->Bottom
    = coolant_htc * cell_width * cell_length ;

#ifdef PRINT_CONDUCTANCES
  fprintf
  (
    stderr,
    "liquid cell  |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "   \
                " |  N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions, current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
#endif
}

/******************************************************************************/
