/******************************************************************************
 *                                                                            *
 * Source file "Sources/heatsink.c"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "environment_heat_sink.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_environment_heat_sink (EnvironmentHeatSink* environmentheatsink)
{
  environmentheatsink->HeatTransferC = 0.0 ;
  environmentheatsink->EnvironmentT  = 0.0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

EnvironmentHeatSink* alloc_and_init_environment_heat_sink (void)
{
  EnvironmentHeatSink* environmentheatsink

    = (EnvironmentHeatSink*) malloc ( sizeof(EnvironmentHeatSink) ) ;

  if (environmentheatsink != NULL)

    init_environment_heat_sink (environmentheatsink) ;

  return environmentheatsink ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void free_environment_heat_sink (EnvironmentHeatSink* environmentheatsink)
{
  free (environmentheatsink) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_environment_heat_sink
(
  FILE*                stream,
  String_t             prefix,
  EnvironmentHeatSink* environmentheatsink
)
{
  fprintf(stream,
    "%sEnvironment Heat Sink\n",             prefix) ;
  fprintf(stream,
    "%s  Heat Transfert Coefficent %.5e\n",  prefix,
                                             environmentheatsink->HeatTransferC) ;
  fprintf(stream,
    "%s  Environment temperature   %.4e\n",  prefix,
                                             environmentheatsink->EnvironmentT) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void                   add_sources_enviroment_heat_sink
(
  EnvironmentHeatSink* environmentheatsink,
  Dimensions*          dimensions,
  Source_t*            sources,
  Conductances*        conductances,
  LayerIndex_t         layer
)
{
  int counter   = get_layer_area (dimensions) ;
  sources      += get_cell_offset_in_stack (dimensions, layer, 0, 0) ;
  conductances += get_cell_offset_in_stack (dimensions, layer, 0, 0) ;

  while (counter-- > 0)
    *sources++ += environmentheatsink->EnvironmentT
                  * (conductances++)->Top ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
