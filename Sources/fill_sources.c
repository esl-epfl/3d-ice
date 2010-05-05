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
#include "floorplan.h"
#include "channel.h"
#include "die.h"
#include "stack_description.h"
#include "data.h"

static int    __layer ;
static double *__sources ;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
static FILE   *debug ;
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_sources_layer (Dimensions *dim, Floorplan *floorplan)
{
  double flp_el_surface, cell_surface ;
  int    row, column ;

  int offset = __layer * (dim->Grid.NRows * dim->Grid.NColumns) ;

  FloorplanElement *flp_el = floorplan->ElementsList ;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
  fprintf (debug, "layer %d\tbuild_source_vector_layer_source\n", __layer) ;
#endif

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)
  {

    flp_el_surface = (double) (flp_el->Length * flp_el->Width) ;

    for (row = flp_el->SW_Row ; row < flp_el->NE_Row ; row++ )
    {
      for (column = flp_el->SW_Column ; column < flp_el->NE_Column ; column++ )
      {

        cell_surface = dim->Cell.Width * get_cell_length(dim, column ) ;

        *(__sources + offset + (row * dim->Grid.NColumns) + column)

          = (flp_el->PowerValue * cell_surface) / flp_el_surface ;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
        fprintf (debug, "cell r %5d c %5d (%6d) using floorplan element %s\n",
          row, column, offset + (row * dim->Grid.NColumns) + column,
          flp_el->Id) ;
#endif
      } /* column */
    } /* row */
  } /* flp_el */
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static void
fill_sources_channel (Dimensions *dim, Channel *channel)
{

  double C   = channel->LiquidSH * 1.62e6 * 0.5
               * (dim->Cell.Length * channel->Height ) ;

  int offset = __layer * (dim->Grid.NRows * dim->Grid.NColumns) ;
  int column ;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
  fprintf (debug, "layer %d\tbuild_source_vector_channel\tC=%.4e\n",
    __layer, C) ;
#endif

  for (column = 1 ; column < dim->Grid.NColumns - 1 ; column += 2)
  {
    *(__sources + offset + column) = 2.0 * C * 300.0 ;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
    fprintf (debug, "cell r 0 c %5d (%5d)\n", column, offset + column) ;
#endif
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_sources_die (Dimensions *dim, Die *die, Floorplan *floorplan)
{
  Layer* layer = die->LayersList;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
  fprintf (debug, "layer %d\tbuild_source_vector_die with floorplan %s\n",
    __layer, floorplan->FileName) ;
#endif

  for ( ; layer != NULL ; layer = layer->Next, __layer++)
  {
    if (layer->Id == die->SourcesId)
    {
      fill_sources_layer (dim, floorplan) ;
    }
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_sources (Data *data, StackDescription *stkd)
{
  StackElement *stack_element = stkd->StackElementsList ;

  __layer   = 0 ;
  __sources = data->Sources ;

#ifdef DEBUG_BUILD_SOURCE_VECTOR
  debug = fopen("fill_sources.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_sources.txt") ;
    return ;
  }
  fprintf (debug, "fill_sources (l %d r %d c %d)\n",
    stkd->Dimensions->Grid.NLayers,
    stkd->Dimensions->Grid.NRows,
    stkd->Dimensions->Grid.NColumns) ;
#endif

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        fill_sources_die (stkd->Dimensions,
                          stack_element->Pointer.Die,
                          stack_element->Floorplan) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        __layer++;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        fill_sources_channel (stkd->Dimensions, stkd->Channel) ;
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

#ifdef DEBUG_BUILD_SOURCE_VECTOR
  fclose(debug) ;
#endif
}
