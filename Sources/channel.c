/******************************************************************************
 *                                                                            *
 * Source file "Sources/channel.c"                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "channel.h"

void
init_channel (Channel* channel)
{
  if (channel == NULL) return ;

  channel->Height       = 0.0 ;
  channel->LiquidHTC    = 0.0 ;
  channel->LiquidSH     = 0.0 ;
  channel->WallMaterial = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Channel *
alloc_and_init_channel (void)
{
  Channel *channel = (Channel *) malloc ( sizeof(Channel) ) ;

  init_channel (channel) ;

  return channel ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_channel (Channel *channel)
{
  if (channel == NULL) return ;

  free (channel) ;
}

/******************************************************************************/

void print_channel (FILE *stream, char *prefix, Channel *channel)
{
  fprintf(stream, "%sChannel\n", prefix) ;
  fprintf(stream, "%s  Height                           %5.2f\n",
    prefix, channel->Height) ;
  fprintf(stream, "%s  Liquid heat transfert coefficent %.4e\n",
    prefix, channel->LiquidHTC) ;
  fprintf(stream, "%s  Liquid specific heat             %.4e\n",
    prefix, channel->LiquidSH) ;

  Material *wall = channel->WallMaterial ;

  if (wall != NULL)
  {
    fprintf(stream, "%s  Wall thermal conductivity        %.4e (%s)\n",
      prefix, wall->ThermalConductivity, wall->Id) ;
    fprintf(stream, "%s  Wall specific heat               %.4e (%s)\n",
      prefix, wall->SpecificHeat, wall->Id) ;
  }
  else
    fprintf(stream, "%s  Wall material not specified\n", prefix) ;
}
