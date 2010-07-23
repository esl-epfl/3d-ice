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

  struct Channel
  {
                              /*                                              */
    CellDimension_t Height ;  /* The heigh of the channel  [um] (1 cell)      */
                              /*                                              */
    CoolantHTC_t CoolantHTC ; /* The heat transfert coefficent of             */
                              /* the cooling liquid  [ (W / ( um2 * K ) ]     */
                              /*                                              */
    CoolantVHC_t CoolantVHC ; /* The volumetric heat capacity of              */
                              /* the cooling liquid  [ J / ( um3 * K ) ]      */
                              /*                                              */
    Temperature_t CoolantTIn ;/* The temperarute of the incoming liquid [K]   */
                              /*                                              */
    CoolantFR_t CoolantFR ;   /* The flow rate per channel layer of the incolimg liquid         */
                              /* The user specify a flowrate in [ ml / min ]  */
                              /* We store it as [ um3 / sec ]                 */
                              /* Shared by all the channels for each layer in the 3DStack    */
                              /*                                              */
    int FlowRateChanged ;     /* 0 = flow rate hasn't changet since last sim. */
                              /* 1 = user used change_flow_rate               */
                              /*                                              */
    Material *Wall ;          /* A pointer to the Material composing the wall */
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

  Conductances*     fill_conductances_channel
  (
    struct Channel* channel,
    Conductances*   conductances,
    Dimensions*     dimensions,
    LayerIndex_t    current_layer
  ) ;

  Capacity_t*       fill_capacities_channel
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t    current_layer,
#   endif
    struct Channel* channel,
    Capacity_t*     capacities,
    Dimensions*     dimensions,
    double          delta_time
  ) ;

  Source_t*         fill_sources_channel
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t    current_layer,
#   endif
    struct Channel* channel,
    Source_t*       sources,
    Dimensions*     dimensions
  ) ;

  int                    fill_ccs_system_matrix_channel
  (
#   ifdef PRINT_SYSTEM_MATRIX
    struct Channel*      channel,
#   endif
    Dimensions*          dimensions,
    struct Conductances* conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    int*                 columns,
    int*                 rows,
    double*              values
  ) ;

  int  fill_crs_system_matrix_channel
  (
#   ifdef PRINT_SYSTEM_MATRIX
    struct Channel*      channel,
#   endif
    Dimensions*  dimensions,
    struct Conductances* conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    int*                 rows,
    int*                 columns,
    double*              values
  ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_CHANNEL_H_ */
