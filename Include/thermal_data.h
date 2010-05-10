/******************************************************************************
 *                                                                            *
 * Header file "Include/data.h"                                               *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_DATA_H_
#define _TL_DATA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "resistances.h"
#include "stack_description.h"

/******************************************************************************
 *                                                                            *
 * "Data" : the collection of temperature, source and capacity.               *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double      *Temperatures ;
    double      *Sources ;
    double      *Capacities ;
    Resistances *Resistances ;

    int Size ;

  } ThermalData ;

/******************************************************************************/

  int alloc_and_init_thermal_data (ThermalData *tdata, int size,
                           double temperature, double source, double capacity) ;

  void free_thermal_data (ThermalData *tdata) ;

  void fill_resistances (StackDescription *stkd, ThermalData *tdata) ;

  void fill_capacities (StackDescription *stkd,
                        ThermalData *tdata, double delta_time) ;

  void fill_sources (StackDescription *stkd, ThermalData *tdata) ;

#ifdef __cplusplus
}
#endif
#endif /* _TL_DATA_H_ */
