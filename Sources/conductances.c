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
#include "layer.h"

void
fill_conductances_solid_cell
(
#ifdef DEBUG_FILL_CONDUCTANCES
  FILE *debug,
  int row,
  int column,
#endif
  struct Conductances *conductances,
  struct Dimensions *dimensions,
  double cell_length,
  double cell_width,
  double cell_height,
  double thermal_conductivity,
  int current_layer
)
{
  LayerPosition_t position
    = get_layer_position(dimensions, current_layer) ;

  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height ) / (cell_width / 2.0) ;

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height ) / (cell_length / 2.0) ;

  if (position == TL_LAYER_BOTTOM)
  {
    conductances->Bottom = 0.0 ;

    conductances->Top
      = (thermal_conductivity * cell_length * cell_width) / cell_height ;

  }
  else if (position == TL_LAYER_TOP)
  {
#ifdef TL_NO_CHANNELS
    conductances->Top
      = 1/ (
              1/((thermal_conductivity * cell_length * cell_width) / (cell_height/2.0))
              +
              1/(1e-07 * cell_length * cell_width)
           );

    conductances->Bottom
      = (thermal_conductivity * cell_length * cell_width) / (cell_height/2.0) ;
#else
    conductances->Top = 0.0 ;

    conductances->Bottom
      = (thermal_conductivity * cell_length * cell_width) / cell_height ;
#endif
  }
  else
  {
    conductances->Top = conductances->Bottom
     = (thermal_conductivity * cell_length * cell_width) / (cell_height / 2.0) ;
  }

#ifdef DEBUG_FILL_CONDUCTANCES
  fprintf (debug,
    "%p fill_conductances_solid_cell\tl %2d (%d)  r %4d  c %4d" \
    "\tl %5.2f  w %5.2f h %5.2f\ttc   %.4e"                    \
    "\t N %.5e\t S %.5e\t E %.5e\t W %.5e\t T %.5e\t B %.5e\n",
    conductances, current_layer, position, row, column,
    cell_length, cell_width, cell_height, thermal_conductivity,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
#endif

}

/******************************************************************************/
/******************************************************************************/
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
#ifdef DEBUG_FILL_CONDUCTANCES
  FILE *debug,
  int row,
  int column,
#endif
  struct Conductances *conductances,
  struct Dimensions *dimensions,
  double cell_length,
  double cell_width,
  double cell_height,
  double coolant_htc,
  double coolant_vhc,
  double flow_rate,
  int current_layer
)
{
  LayerPosition_t position
    = get_layer_position(dimensions, current_layer) ;

  double C = (coolant_vhc * flow_rate)
             / (double) ( get_number_of_columns (dimensions) - 1 );

  if (position == TL_LAYER_BOTTOM)
  {
    fprintf (stderr, "Warning: channel on bottom layer not supported\n") ;
  }
  else if (position == TL_LAYER_TOP)
  {
    fprintf (stderr, "Warning: channel on top layer not supported\n") ;
  }

  conductances->North =  C ;
  conductances->South = -C ;

  conductances->East = conductances->West
    = coolant_htc * cell_width * cell_height ;

  conductances->Top = conductances->Bottom
    = coolant_htc * cell_width * cell_length ;

#ifdef DEBUG_FILL_CONDUCTANCES
  fprintf (debug,
    "%p fill_conductances_liquid_cell\tl %2d (%d)  r %4d  c %4d" \
    "\tl %5.2f  w %5.2f  h %5.2f\tlhtc %.4e lsh %.4e"           \
    "\t N %.5e\t S %.5e\t E %.5e\t W %.5e\t T %.5e\t B %.5e\n",
    conductances, current_layer, position, row, column,
    cell_length, cell_width, cell_height, coolant_htc, coolant_vhc,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
