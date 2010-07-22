/******************************************************************************
 *                                                                            *
 * Header file "Include/types.h"                                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_TYPES_H_
#define _TL_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

  typedef char* String_t ;

  /* Indexes */

  typedef int LayerIndex_t ;
  typedef int RowIndex_t ;
  typedef int ColumnIndex_t ;

  /* Dimensions */

  typedef double CellDimension_t ;
  typedef int    GridDimension_t ;
  typedef double ChipDimension_t ;

  /* Material */

  typedef double VolHeatCapacity_t ;
  typedef double ThermalConductivity_t;

  typedef double Conductance_t ;
  typedef double Capacity_t ;
  typedef double Source_t ;

  typedef double CoolantHTC_t ;
  typedef double CoolantVHC_t ;
  typedef double CoolantFR_t ;

  typedef double HeatSinkHTC_t ;

  typedef double Temperature_t ;
  typedef double Time_t ;

#ifdef __cplusplus
}
#endif

#endif /* _TL_TYPES_H_ */
