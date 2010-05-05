/******************************************************************************
 *                                                                            *
 * Source file "Sources/fill_sources.c"                                       *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdio.h>

#include "dimensions.h"
#include "channel.h"
#include "die.h"
#include "stack_description.h"
#include "data.h"

static int    __layer ;
static double *__capacities ;
static double __delta_time ;

#ifdef DEBUG_FILL_CAPACITIES
static FILE   *debug ;
#endif

static
double
capacity (double length, double width, double height, double specificheat)
{
  return (specificheat * length * width * height) / __delta_time ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_capacities_layer (Dimensions *dim, Layer *layer)
{
  int row, column ;
  int offset = __layer * (dim->Grid.NRows * dim->Grid.NColumns) ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug, "layer %d\tfill_capacities_layer\n", __layer) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++ )
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++ )
    {

      __capacities [offset + (row * dim->Grid.NColumns) + column]

        = capacity (get_cell_length(dim, column), dim->Cell.Width,
                    layer->Height, layer->Material->SpecificHeat ) ;

#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug, "cell r %5d c %5d (%6d) using %s layer\n",
        row, column, offset + (row * dim->Grid.NColumns) + column,
        layer->Material->Id) ;
#endif
    } /* column */
  } /* row */
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_capacities_channel (Dimensions *dim, Channel *channel)
{
  int row, column ;
  int offset = __layer * (dim->Grid.NRows * dim->Grid.NColumns) ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug, "layer %d\tfill_capacities_channel\n", __layer) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++ )
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++ )
    {

      if (column % 2 == 0 ) /* Even -> Wall */
      {
        __capacities [offset + (row * dim->Grid.NColumns) + column]

          = capacity (get_cell_length(dim, column), dim->Cell.Width,
                      channel->Height, channel->WallMaterial->SpecificHeat) ;

#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug, "wall   cell r %5d c %5d (%6d) using %s layer\n",
        row, column, offset + (row * dim->Grid.NColumns) + column,
        channel->WallMaterial->Id) ;
#endif

      }
      else                 /* Odd -> liquid */
      {
        __capacities [offset + (row * dim->Grid.NColumns) + column]

          = capacity (get_cell_length(dim, column), dim->Cell.Width,
                      channel->Height, channel->LiquidSH) ;

#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug, "liquid cell r %5d c %5d (%6d)\n",
        row, column, offset + (row * dim->Grid.NColumns) + column) ;
#endif
      }

    } /* column */
  } /* row */

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_capacities_die (Dimensions *dim, Die *die)
{
  Layer* layer = die->LayersList;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug, "layer %d\tfill_capacities_die %s\n", __layer, die->Id) ;
#endif

  for ( ; layer != NULL ; layer = layer->Next, __layer++)
  {
    fill_capacities_layer (dim, layer) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_capacities (Data *data, StackDescription *stkd, double delta_time)
{
  StackElement *stack_element = stkd->StackElementsList ;

  __layer      = 0 ;
  __delta_time = delta_time ;
  __capacities = data->Capacities ;

#ifdef DEBUG_FILL_CAPACITIES
  debug = fopen("fill_capacities.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_capacities.txt") ;
    return ;
  }
  fprintf (debug, "fill_capacities (l %d r %d c %d)\n",
    stkd->Dimensions->Grid.NLayers,
    stkd->Dimensions->Grid.NRows,
    stkd->Dimensions->Grid.NColumns) ;
#endif

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        fill_capacities_die (stkd->Dimensions,
                             stack_element->Pointer.Die) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        fill_capacities_layer (stkd->Dimensions,
                               stack_element->Pointer.Layer) ;
        __layer++;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        fill_capacities_channel (stkd->Dimensions, stkd->Channel) ;
        __layer++;

        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr, "Error! Found stack element with no type\n") ;
        break ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;

    } /* stack_element->Type */

  } /* stack_element */

#ifdef DEBUG_FILL_CAPACITIES
  fclose(debug) ;
#endif
}
