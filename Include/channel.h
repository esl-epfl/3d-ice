/******************************************************************************
 *                                                                            *
 * Header file "Include/channel.h"                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_CHANNEL_H_
#define _TL_CHANNEL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "material.h"
#include "conductances.h"

/******************************************************************************
 *                                 "Channel"                                  *
 ******************************************************************************/

  typedef struct channel
  {
                              /*                                              */
    double Height ;           /* The heigh of the channel  [um] (1 cell)      */
                              /*                                              */
    double CoolantHTC ;       /* The heat transfert coefficent of             */
                              /* the cooling liquid  [ (W * K / um2 ]         */
                              /*                                              */
    double CoolantVHC ;       /* The volumetric heat capacity of              */
                              /* the cooling liquid  [ J / ( um3 * K ) ]      */
                              /*                                              */
    double CoolantTIn ;       /* The temperarute of the incoming liquid [K]   */
                              /*                                              */
    double FlowRate ;         /* The flow rate of the incolimg liquid         */
                              /* The user specify a flowrate in [ ml / min ]  */
                              /* We store it as [ um3 / sec ]                 */
                              /* Shared by all the channels in the 3DStack    */
                              /*                                              */
    int FlowRateChanged ;     /* 0 = flow rate hasn't changet since last sim. */
                              /* 1 = user used change_flow_rate               */
                              /*                                              */
    Material *WallMaterial ;  /* A pointer to the Material composing the wall */
                              /*                                              */
  } Channel ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_channel               (Channel *channel) ;

  Channel *
  alloc_and_init_channel     (void) ;

  void
  free_channel               (Channel *channel) ;

  void
  print_channel              (FILE *stream, char *prefix, Channel *channel) ;

  Conductances *
  fill_conductances_channel  (
                              #ifdef DEBUG_FILL_CONDUCTANCES
                              FILE         *debug,
                              #endif
                              Channel      *channel,
                              Conductances *conductances,
                              Dimensions   *dim,
                              int          current_layer
                             ) ;


  double *
  fill_capacities_channel    (
                              #ifdef DEBUG_FILL_CAPACITIES
                              FILE       *debug,
                              int        current_layer,
                              #endif
                              Channel    *channel,
                              double     *capacities,
                              Dimensions *dimensions,
                              double     delta_time
                             ) ;

  double *
  fill_sources_channel       (
                              #ifdef DEBUG_FILL_SOURCES
                              FILE       *debug,
                              int        current_layer,
                              #endif
                              Channel    *channel,
                              double     *sources,
                              Dimensions *dim
                             ) ;

  int
  fill_ccs_system_matrix_channel (
                              #ifdef DEBUG_FILL_SYSTEM_MATRIX
                              FILE         *debug,
                              Channel      *channel,
                              #endif
                              Dimensions   *dimensions,
                              Conductances *conductances,
                              double       *capacities,
                              int          *columns,
                              int          *rows,
                              double       *values,
                              int          current_layer
                             ) ;


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_CHANNEL_H_ */
