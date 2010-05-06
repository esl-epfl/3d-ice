/******************************************************************************
 *                                                                            *
 * Header file "Include/resistances.h"                                        *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_RESISTANCES_H_
#define _TL_RESISTANCES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "dimensions.h"

/******************************************************************************
 *                                                                            *
 * "Resistances" : the six resistances inside a cell.                         *
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

  } Resistances ;

/******************************************************************************/

  void fill_resistances_solid_cell (
#ifdef DEBUG_FILL_RESISTANCES
                                    FILE *debug,
                                    int row,
                                    int column,
#endif
                                    Resistances *resistance,
                                    Dimensions *dim,
                                    double cell_length,
                                    double cell_width,
                                    double cell_height,
                                    double thermal_conductivity,
                                    int current_layer) ;

  void fill_resistances_liquid_cell (
#ifdef DEBUG_FILL_RESISTANCES
                                    FILE *debug,
                                    int row,
                                    int column,
#endif
                                    Resistances *resistance,
                                    Dimensions *dim,
                                    double cell_length,
                                    double cell_width,
                                    double cell_height,
                                    double liquid_htc,
                                    double liquid_sh,
                                    int current_layer) ;

#ifdef __cplusplus
}
#endif
#endif /* _TL_RESISTANCES_H_ */
