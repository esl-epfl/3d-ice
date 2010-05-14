/******************************************************************************
 *                                                                            *
 * Header file "Include/conductances.h"                                        *
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

/******************************************************************************
 *                                                                            *
 * "Conductances" : the six conductances of the six resistor in a cell.       *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double North ;
    double South ;

    double East ;
    double West ;

    double Top ;
    double Bottom ;

  } Conductances ;

/******************************************************************************/

  void fill_conductances_solid_cell (
#ifdef DEBUG_FILL_CONDUCTANCES
                                    FILE *debug,
                                    int row,
                                    int column,
#endif
                                    Conductances *conductances,
                                    Dimensions *dim,
                                    double cell_length,
                                    double cell_width,
                                    double cell_height,
                                    double thermal_conductivity,
                                    int current_layer) ;

  void fill_conductances_liquid_cell (
#ifdef DEBUG_FILL_CONDUCTANCES
                                    FILE *debug,
                                    int row,
                                    int column,
#endif
                                    Conductances *conductances,
                                    Dimensions   *dimensions,
                                    double       cell_length,
                                    double       cell_width,
                                    double       cell_height,
                                    double       coolant_htc,
                                    double       coolant_vhc,
                                    double       flow_rate,
                                    int          current_layer) ;

#ifdef __cplusplus
}
#endif
#endif /* _TL_CONDUCTANCES_H_ */
