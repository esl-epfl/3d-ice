/******************************************************************************
 *                                                                            *
 * Source file "Sources/resistances.c"                                        *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "resistances.h"
#include "layer.h"

void
fill_resistances_solid_cell
(
#ifdef DEBUG_FILL_RESISTANCES
  FILE *debug,
  int row,
  int column,
#endif
  Resistances *resistances,
  Dimensions *dimensions,
  double cell_length,
  double cell_width,
  double cell_height,
  double thermal_conductivity,
  int current_layer
)
{
  LayerPosition_t position
    = get_layer_position(&dimensions->Grid, current_layer) ;

  resistances->North = resistances->South
    = (thermal_conductivity * cell_length * cell_height ) / (cell_width / 2.0) ;

  resistances->East = resistances->West
    = (thermal_conductivity * cell_width * cell_height ) / (cell_length / 2.0) ;

  if (position == TL_LAYER_BOTTOM)
  {
    resistances->Bottom = 0.0 ;

    resistances->Top
      = (thermal_conductivity * cell_length * cell_width) / cell_height ;

  }
  else if (position == TL_LAYER_TOP)
  {
    resistances->Top = 0.0 ;

    resistances->Bottom
      = (thermal_conductivity * cell_length * cell_width) / cell_height ;
  }
  else
  {
    resistances->Top = resistances->Bottom
     = (thermal_conductivity * cell_length * cell_width) / (cell_height / 2.0) ;
  }

#ifdef DEBUG_FILL_RESISTANCES
  fprintf (debug,
    "%p fill_resistances_solid_cell\tl %2d (%d)  r %4d  c %4d" \
    "\tl %5.2f  w %5.2f h %5.2f\ttc   %.4e"                    \
    "\t N %.5e\t S %.5e\t E %.5e\t W %.5e\t T %.5e\t B %.5e\n",
    resistances, current_layer, position, row, column,
    cell_length, cell_width, cell_height, thermal_conductivity,
    resistances->North, resistances->South, resistances->East,
    resistances->West, resistances->Top, resistances->Bottom) ;
#endif

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

// FlowRate = 1.667e+12;
// FlowArea = #Channels * Ay = 11 * (50 * layer->height);
//
// Velocity = FlowRate / FlowArea
//          = 1.667e+12 / (11 * (50 * layer->height));
//
// C = SpecificHeatH2O * Velocity * Ay * 0.5;
//   = SpecificHeatH2O * FlowRate * ( 1.0 / (  FlowArea    ) ) * Ay * 0.5;
//   = SpecificHeatH2O * FlowRate * ( 1.0 / (#Channels * Ay) ) * Ay * 0.5;
//   = SpecificHeatH2O * FlowRate * ( 1.0 / (#Channels) ) * 0.5;
//   =   4.1692e-12    * 1.667e12 * ( 1.0 /    11.0     ) * 0.5;

void
fill_resistances_liquid_cell
(
#ifdef DEBUG_FILL_RESISTANCES
  FILE *debug,
  int row,
  int column,
#endif
  Resistances *resistances,
  Dimensions *dimensions,
  double cell_length,
  double cell_width,
  double cell_height,
  double liquid_htc,
  double liquid_sh,
  int current_layer
)
{
  LayerPosition_t position
    = get_layer_position(&dimensions->Grid, current_layer) ;

  double C = liquid_sh * 1.62e6 * (cell_length * cell_height ) * 0.5;

  if (position == TL_LAYER_BOTTOM)
  {
    fprintf (stderr, "Warning: channel on bottom layer not supported\n") ;
  }
  else if (position == TL_LAYER_TOP)
  {
    fprintf (stderr, "Warning: channel on top layer not supported\n") ;
  }

  resistances->North =  C ;
  resistances->South = -C ;

  resistances->East = resistances->West
    = liquid_htc * cell_width * cell_height ;

  resistances->Top = resistances->Bottom
    = liquid_htc * cell_width * cell_length ;

#ifdef DEBUG_FILL_RESISTANCES
  fprintf (debug,
    "%p fill_resistances_liquid_cell\tl %2d (%d)  r %4d  c %4d" \
    "\tl %5.2f  w %5.2f  h %5.2f\tlhtc %.4e lsh %.4e"           \
    "\t N %.5e\t S %.5e\t E %.5e\t W %.5e\t T %.5e\t B %.5e\n",
    resistances, current_layer, position, row, column,
    cell_length, cell_width, cell_height, liquid_htc, liquid_sh,
    resistances->North, resistances->South, resistances->East,
    resistances->West, resistances->Top, resistances->Bottom) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
