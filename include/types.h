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

  typedef int   Bool_t ;

# define FALSE (0)
# define TRUE  (!FALSE)

  typedef int    Quantity_t ;

  typedef int    LayerIndex_t ;
  typedef int    RowIndex_t ;
  typedef int    ColumnIndex_t ;

  typedef double Temperature_t ;
  typedef double Power_t ;
  typedef double Time_t ;

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

  typedef double Cconv_t ;

  typedef double CoolantHTC_t ;
  typedef double CoolantVHC_t ;
  typedef double CoolantFR_t ;

  typedef double EnvironmentHTC_t ;

  typedef struct
  {
    CoolantHTC_t Side ;
    CoolantHTC_t Top ;
    CoolantHTC_t Bottom ;

  } CoolantHTCs_t ;

#ifdef __cplusplus
}
#endif

#endif /* _TL_TYPES_H_ */
