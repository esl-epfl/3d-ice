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

/******************************************************************************/

void                     fill_conductances_bottom_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*            dimensions,
  LayerIndex_t           current_layer,
  RowIndex_t             current_row,
  ColumnIndex_t          current_column,
# endif
  Conductances*          conductances,
  CellDimension_t        cell_length,
  CellDimension_t        cell_width,
  CellDimension_t        cell_height,
  ThermalConductivity_t  thermal_conductivity
)
{
  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Bottom = (Conductance_t) 0 ;

  conductances->Top
    = (thermal_conductivity * cell_length * cell_width) / cell_height ;

#ifdef PRINT_CONDUCTANCES
  fprintf
  (
    stderr,
    "solid  bottom  | l %2d r %4d c %4d [%6d] " \
                   "| l %5.2f w %5.2f h %5.2f " \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions, current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom
  ) ;
#endif
}

/******************************************************************************/

void                     fill_conductances_central_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*            dimensions,
  LayerIndex_t           current_layer,
  RowIndex_t             current_row,
  ColumnIndex_t          current_column,
# endif
  Conductances*          conductances,
  CellDimension_t        cell_length,
  CellDimension_t        cell_width,
  CellDimension_t        cell_height,
  ThermalConductivity_t  thermal_conductivity
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

//  else if (cell_position == TL_LAYER_TOP)
//  {
//    if (heatsink == NULL)
//    {
//      conductances->Top = (Conductance_t) 0 ;
//
//      conductances->Bottom
//        = (thermal_conductivity * cell_length * cell_width)
//          / cell_height ;
//    }
//    else
//    {
//        conductances->Top
//          = ((Conductance_t) 2 * thermal_conductivity * heatsink->HeatTransferC
//             * cell_length * cell_width)
//            /
//            (cell_height * heatsink->HeatTransferC
//             + (Conductance_t) 2 * thermal_conductivity) ;
//
//        conductances->Bottom
//          = (thermal_conductivity * cell_length * cell_width)
//            / (cell_height / (CellDimension_t) 2) ;
//    }
//  }

#ifdef PRINT_CONDUCTANCES
  fprintf
  (
    stderr,
    "solid  central | l %2d r %4d c %4d [%6d] "  \
                   "| l %5.2f w %5.2f h %5.2f "   \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions, current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom
  ) ;
#endif
}

/******************************************************************************/

void                     fill_conductances_top_solid_cell
(
# ifdef PRINT_CONDUCTANCES
  Dimensions*            dimensions,
  LayerIndex_t           current_layer,
  RowIndex_t             current_row,
  ColumnIndex_t          current_column,
# endif
  Conductances*          conductances,
  CellDimension_t        cell_length,
  CellDimension_t        cell_width,
  CellDimension_t        cell_height,
  ThermalConductivity_t  thermal_conductivity
)
{
  conductances->North = conductances->South
    = (thermal_conductivity * cell_length * cell_height )
      / ( cell_width / (CellDimension_t) 2);

  conductances->East = conductances->West
    = (thermal_conductivity * cell_width * cell_height )
      / (cell_length / (CellDimension_t) 2) ;

  conductances->Bottom
    = (thermal_conductivity * cell_length * cell_width) / cell_height ;

  conductances->Top = (Conductance_t) 0 ;

#ifdef PRINT_CONDUCTANCES
  fprintf
  (
    stderr,
    "solid  top     | l %2d r %4d c %4d [%6d] "  \
                   "| l %5.2f w %5.2f h %5.2f "   \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
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

void                     fill_conductances_central_liquid_cell
(
# ifdef PRINT_CONDUCTANCES
  LayerIndex_t           current_layer,
  RowIndex_t             current_row,
  ColumnIndex_t          current_column,
# endif
  Dimensions*            dimensions,
  Conductances*          conductances,
  CellDimension_t        cell_length,
  CellDimension_t        cell_width,
  CellDimension_t        cell_height,
  CoolantHTC_t           coolant_htc,
  CoolantVHC_t           coolant_vhc,
  CoolantFR_t            coolant_fr
)
{
  double C = (coolant_vhc * coolant_fr)
             / (double) ( get_number_of_columns (dimensions) - 1 );

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
    "liquid central | l %2d r %4d c %4d [%6d] "   \
                   "| l %5.2f w %5.2f h %5.2f "   \
                   "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e\n",
    current_layer, current_row, current_column,
    get_cell_offset_in_stack (dimensions, current_layer, current_row, current_column),
    cell_length, cell_width, cell_height,
    conductances->North, conductances->South, conductances->East,
    conductances->West, conductances->Top, conductances->Bottom) ;
#endif
}

/******************************************************************************/
