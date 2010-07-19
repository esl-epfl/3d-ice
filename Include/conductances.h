/******************************************************************************
 *                                                                            *
 * Header file "Include/conductances.h"                                       *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_CONDUCTANCES_H_
#define _TL_CONDUCTANCES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "dimensions.h"
#include "layer.h"

/******************************************************************************
 *                                "Conductances"                              *
 ******************************************************************************/

  struct Conductances
  {
    Conductance_t North;
    Conductance_t South;

    Conductance_t East;
    Conductance_t West;

    Conductance_t Top;
    Conductance_t Bottom;

  };

  typedef struct Conductances Conductances;

/******************************************************************************/

  void
  fill_conductances_solid_cell
  (
#   ifdef PRINT_CONDUCTANCES
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    Dimensions*            dimensions,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity,
    LayerIndex_t           current_layer
  ) ;

  void
  fill_conductances_liquid_cell
  (
#   ifdef PRINT_CONDUCTANCES
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    Dimensions*            dimensions,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    CoolantHTC_t           coolant_htc,
    CoolantVHC_t           coolant_vhc,
    CoolantFR_t            coolant_fr,
    LayerIndex_t           current_layer
  ) ;

#ifdef __cplusplus
}
#endif

#endif    /* _TL_CONDUCTANCES_H_ */
