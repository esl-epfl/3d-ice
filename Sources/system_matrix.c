/******************************************************************************
 *                                                                            *
 * Source file "Source/build_system_matrix.c"                                 *
 *                                                                            *
 *                                          -- alessandro.vincenzi@epfl.ch -- *
 ******************************************************************************/

#include <stdlib.h>

#include "resistances.h"
#include "system_matrix.h"

#define PARALLEL(x,y) ( (x * y) / ( x + y) )

static int    *__columns ;
static int    *__rows ;
static double *__values ;
static Resistances *__cell_grid ;
static double *__capacities ;
static int    __layer ;
static int    __cell_index ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
static FILE   *debug ;
#endif

int
alloc_system_matrix (SystemMatrix *matrix, int nvalues, int nnz)
{
  if (matrix == NULL) return 0 ;

  matrix->Size = nvalues ;
  matrix->NNz  = nnz ;

  matrix->Columns = (int *) malloc (sizeof(int) * nvalues + 1 ) ;

  if (matrix->Columns == NULL)
  {
    return 0 ;
  }

  matrix->Rows = (int *) malloc (sizeof(int) * nnz ) ;

  if (matrix->Rows == NULL)
  {
    free (matrix->Columns) ;
    return 0 ;
  }

  matrix->Values = (double *) malloc (sizeof(double) * nnz ) ;

  if (matrix->Values == NULL)
  {
    free (matrix->Columns) ;
    free (matrix->Rows) ;
    return 0 ;
  }

  return 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void free_system_matrix (SystemMatrix *matrix)
{
  if (matrix == NULL) return ;

  free (matrix->Columns) ;
  free (matrix->Rows) ;
  free (matrix->Values) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
add_solid_column (GridDimensions *gd, int row, int column)
{
  double resistance        = 0.0 ;
  double diagonal_value    = 0.0 ;
  double *diagonal_pointer = NULL ;
  int    neighbour         = 0 ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "add_solid_column l %d r %d c %d (%d)\n",
    __layer, row, column, __cell_index) ;
#endif

  *__columns = *(__columns - 1) ;

  if ( __layer > 0 )   /* BOTTOM */
  {
    *__rows++ = neighbour = __cell_index - (gd->NRows * gd->NColumns) ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceBottom,
                           __cell_grid[neighbour].ResistanceTop) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  %7d bottom  \t%d\t%.4e = %.4e || %.4e\n",
      *__rows, neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceBottom,
      __cell_grid[neighbour].ResistanceTop) ;
#endif
  }

  if ( row > 0 )   /* SOUTH */
  {
    *__rows++ = neighbour = __cell_index - gd->NColumns ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceSouth,
                           __cell_grid[neighbour].ResistanceNorth) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  south   \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceSouth,
      __cell_grid[neighbour].ResistanceNorth) ;
#endif
  }

  if ( column > 0 )   /* WEST */
  {
    *__rows++ = neighbour = __cell_index - 1 ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceWest,
                           __cell_grid[neighbour].ResistanceEast) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  west    \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceWest,
      __cell_grid[neighbour].ResistanceEast) ;
#endif
    }

  /* DIAGONAL */

  *__rows++        = __cell_index ;
  *__values        = __capacities[__cell_index] ;
  diagonal_pointer = __values++ ;

  (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf
  (
    debug,
    "  diagonal\t%d\n",
    __cell_index
  ) ;
#endif

  if ( column < gd->NColumns - 1 )   /* EAST */
  {
    *__rows++ = neighbour = __cell_index + 1 ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceEast,
                           __cell_grid[neighbour].ResistanceWest) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  east    \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceEast,
      __cell_grid[neighbour].ResistanceWest) ;
#endif
  }

  if ( row < gd->NRows - 1 )   /* NORTH */
  {
    *__rows++ = neighbour = __cell_index + gd->NColumns ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceNorth,
                           __cell_grid[neighbour].ResistanceSouth) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  north   \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceNorth,
      __cell_grid[neighbour].ResistanceSouth) ;
#endif
  }

  if ( __layer < gd->NLayers - 1) /* TOP */
  {
    *__rows++ = neighbour = __cell_index + (gd->NColumns * gd->NRows) ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceTop,
                           __cell_grid[neighbour].ResistanceBottom) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  top     \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceTop,
      __cell_grid[neighbour].ResistanceBottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%.4e\n  columns \t%d\n",
    *diagonal_pointer, *__columns) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
add_liquid_column (GridDimensions *gd, int row, int column)
{
  double resistance        = 0.0;
  double diagonal_value    = 0.0 ;
  double *diagonal_pointer = NULL ;
  int    neighbour         = 0 ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "add_liquid_column l %d r %d c %d (%d)\n",
    __layer, row, column, __cell_index) ;
#endif

  *__columns = *(__columns - 1) ;

  if ( __layer > 0 )   /* BOTTOM */
  {
    *__rows++ = neighbour = __cell_index - (gd->NRows * gd->NColumns) ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceBottom,
                           __cell_grid[neighbour].ResistanceTop) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  bottom  \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceBottom,
      __cell_grid[neighbour].ResistanceTop) ;
#endif
  }

  if ( row > 0 )   /* SOUTH */
  {
    *__rows++   = __cell_index - gd->NColumns ;
    *__values++ = __cell_grid[__cell_index].ResistanceNorth ; // == (C)

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  south   \t%d\t%.4e\n",
      neighbour, __cell_grid[__cell_index].ResistanceNorth) ;
#endif
  }

  if ( column > 0 )   /* WEST */
  {
    *__rows++ = neighbour = __cell_index - 1 ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceWest,
                           __cell_grid[neighbour].ResistanceEast) ;

    *__values++      = -resistance ;
    diagonal_value  +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  west    \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceWest,
      __cell_grid[neighbour].ResistanceEast) ;
#endif
  }

  /* DIAGONAL */

  *__rows++        = __cell_index ;
  *__values        = __capacities[__cell_index] ;
  diagonal_pointer = __values++ ;

  (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%d\n", __cell_index) ;
#endif

  if ( column < gd->NColumns - 1 )    /* EAST */
  {
    *__rows++ = neighbour = __cell_index + 1 ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceEast,
                           __cell_grid[neighbour].ResistanceWest) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  east    \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceEast,
      __cell_grid[neighbour].ResistanceWest) ;
#endif
  }

  if ( row < gd->NRows - 1 )   /* NORTH */
  {
    *__rows++   = __cell_index + gd->NColumns ;
    *__values++ = __cell_grid[__cell_index].ResistanceSouth ; // == -C

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  north   \t%d\t%.4e\n",
      neighbour, __cell_grid[__cell_index].ResistanceSouth) ;
#endif
  }

  if ( __layer < gd->NLayers - 1)  /* TOP */
  {
    *__rows++ = neighbour = __cell_index + (gd->NColumns * gd->NRows) ;

    resistance = PARALLEL (__cell_grid[__cell_index].ResistanceTop,
                           __cell_grid[neighbour].ResistanceBottom) ;

    *__values++     = -resistance ;
    diagonal_value +=  resistance ;

    (*__columns)++ ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
    fprintf (debug, "  top     \t%d\t%.4e = %.4e || %.4e\n",
      neighbour, *(__values-1),
      __cell_grid[__cell_index].ResistanceTop,
      __cell_grid[neighbour].ResistanceBottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (row == 0 || row == gd->NRows - 1)

    *diagonal_pointer += __cell_grid[__cell_index].ResistanceNorth ; // == (C)

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%.4e\n  columns \t%d\n",
    *diagonal_pointer, *__columns) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_system_matrix_layer (GridDimensions *gd)
{
  int row, column;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "build_system_matrix_layer       layer %d  cell %d\n",
    __layer, __cell_index) ;
#endif

  for (row = 0 ; row < gd->NRows ; row++)
  {
    for (column = 0 ; column < gd->NColumns ;
         column++, __columns++, __cell_index++)
    {
      add_solid_column (gd, row, column) ;
    }
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_system_matrix_die (GridDimensions *gd, Die *die)
{
  Layer* layer;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "build_system_matrix_die         layer %d  cell %d\n",
    __layer, __cell_index) ;
#endif

  for (layer = die->LayersList ; layer != NULL ;
       layer = layer->Next, __layer++)
  {
    fill_system_matrix_layer (gd) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
fill_system_matrix_channel (GridDimensions *gd)
{
  int row, column;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fprintf (debug, "build_system_matrix_channel     layer %d  cell %d\n",
    __layer, __cell_index) ;
#endif

  for (row = 0 ; row < gd->NRows ; row++)
  {
    for (column = 0 ; column < gd->NColumns ;
         column++, __cell_index++, __columns++)
    {
      if (column % 2 == 0 ) // Even -> Wall
      {
        add_solid_column (gd, row, column) ;
      }
      else                  // Odd -> channel
      {
        add_liquid_column (gd, row, column) ;
      }
    }
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_system_matrix (SystemMatrix *matrix, StackDescription *stkd, Data *data)
{
  StackElement *stack_element = stkd->StackElementsList ;

  __columns    = matrix->Columns ;
  __rows       = matrix->Rows    ;
  __values     = matrix->Values  ;
  __cell_grid  = malloc(sizeof(Resistances) * stkd->Dimensions->Grid.NCells);
  __capacities = data->Capacities ;
  __layer      = 0 ;
  __cell_index = 0 ;

  build_resistances_grid (__cell_grid, stkd) ;

  *__columns++ = 0 ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  debug = fopen("build_system_matrix.txt", "w") ;
  if (debug == NULL)
  {
    perror("build_system_matrix.txt") ;
    return ;
  }
  fprintf (debug, "build_system_matrix (l %d r %d c %d)\n",
    stkd->Dimensions.Grid.NLayers,
    stkd->Dimensions.Grid.NRows,
    stkd->Dimensions.Grid.NColumns) ;
#endif

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        fill_system_matrix_die (&(stkd->Dimensions->Grid),
                                stack_element->Pointer.Die) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        fill_system_matrix_layer (&(stkd->Dimensions->Grid)) ;
        __layer++;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        fill_system_matrix_channel (&(stkd->Dimensions->Grid)) ;
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

  free(__cell_grid) ;

#ifdef DEBUG_BUILD_SYSTEM_MATRIX
  fclose(debug) ;
#endif

}
