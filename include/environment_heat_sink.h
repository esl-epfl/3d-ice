/******************************************************************************
 *                                                                            *
 * Header file "Include/environment_heat_sink.h"                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_ENVIRONMENT_HEAT_SINK_H_
#define _TL_ENVIRONMENT_HEAT_SINK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"
#include "conductances.h"

/******************************************************************************
 *                      "EnvironmentHeatSink"                                 *
 ******************************************************************************/

  struct EnvironmentHeatSink
  {
    HeatSinkHTC_t HeatTransferC ; /* The heat transfert coefficent */

    Temperature_t EnvironmentT ; /* The temperarute of the environment [K] */
  } ;

  typedef struct EnvironmentHeatSink EnvironmentHeatSink ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void init_environment_heat_sink (EnvironmentHeatSink* environmentheatsink) ;

  EnvironmentHeatSink* alloc_and_init_environment_heat_sink (void) ;

  void free_environment_heat_sink (EnvironmentHeatSink* environmentheatsink) ;

  void print_environment_heat_sink (FILE* stream,
                                    String_t prefix,
                                    EnvironmentHeatSink* environmentheatsink) ;

  void                   add_sources_enviroment_heat_sink
  (
    EnvironmentHeatSink* environmentheatsink,
    Dimensions*          dimensions,
    Source_t*            sources,
    Conductances*        conductances,
    LayerIndex_t         layer
  ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_ENVIRONMENT_HEAT_SINK_H_ */
