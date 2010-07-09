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

#include <stdio.h>

#include "material.h"
#include "conductances.h"

/******************************************************************************
 *                                 "Channel"                                  *
 ******************************************************************************/

  struct Channel
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
    double FlowRate ;         /* The flow rate per channel layer of the incolimg liquid         */
                              /* The user specify a flowrate in [ ml / min ]  */
                              /* We store it as [ um3 / sec ]                 */
                              /* Shared by all the channels for each layer in the 3DStack    */
                              /*                                              */
    int FlowRateChanged ;     /* 0 = flow rate hasn't changet since last sim. */
                              /* 1 = user used change_flow_rate               */
                              /*                                              */
    struct Material *Wall ;   /* A pointer to the Material composing the wall */
                              /*                                              */
  } ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_channel               (struct Channel *channel) ;

  struct Channel *
  alloc_and_init_channel     (void) ;

  void
  free_channel               (struct Channel *channel) ;

  void
  print_channel              (FILE *stream, char *prefix, struct Channel *channel) ;

  struct Conductances *
  fill_conductances_channel  (
                              #ifdef DEBUG_FILL_CONDUCTANCES
                              FILE         *debug,
                              #endif
                              struct Channel      *channel,
                              struct Conductances *conductances,
                              struct Dimensions   *dim,
                              int          current_layer
                             ) ;


  double *
  fill_capacities_channel    (
                              #ifdef DEBUG_FILL_CAPACITIES
                              FILE       *debug,
                              int        current_layer,
                              #endif
                              struct Channel    *channel,
                              double     *capacities,
                              struct Dimensions *dimensions,
                              double     delta_time
                             ) ;

  double *
  fill_sources_channel       (
                              #ifdef DEBUG_FILL_SOURCES
                              FILE       *debug,
                              int        current_layer,
                              #endif
                              struct Channel    *channel,
                              double     *sources,
                              struct Dimensions *dim
                             ) ;

  int
  fill_ccs_system_matrix_channel (
                              #ifdef DEBUG_FILL_SYSTEM_MATRIX
                              FILE         *debug,
                              struct Channel      *channel,
                              #endif
                              struct Dimensions   *dimensions,
                              struct Conductances *conductances,
                              double       *capacities,
                              int          *columns,
                              int          *rows,
                              double       *values,
                              int          current_layer
                             ) ;

  int
  fill_crs_system_matrix_channel (
                              #ifdef DEBUG_FILL_SYSTEM_MATRIX
                              FILE         *debug,
                              struct Channel      *channel,
                              #endif
                              struct Dimensions   *dimensions,
                              struct Conductances *conductances,
                              double       *capacities,
                              int          *rows,
                              int          *columns,
                              double       *values,
                              int          current_layer
                             ) ;


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#endif /* _TL_CHANNEL_H_ */
