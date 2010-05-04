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

/******************************************************************************
 *                                                                            *
 * "Channel" : the representation of a single chennel.                        *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double Height ;          /* The heigh of the channel in um */
                             /* (corresponding to 1 cell).     */

    double LiquidHTC ;       /* The heat transfert coefficent of */
                             /* the cooling liquid               */

    double LiquidSH ;        /* The specific heat of the cooling liquid */

    Material *WallMaterial ; /* A pointer to the Material composing the wall */

  } Channel ;

/******************************************************************************/

  void init_channel  (Channel *channel) ;

  Channel *alloc_and_init_channel (void) ;

  void free_channel (Channel *channel) ;

  void print_channel (FILE *stream, char *prefix, Channel *channel) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_CHANNEL_H_ */
