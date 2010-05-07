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

void
fill_system_matrix
(
  StackDescription *stkd,
  SystemMatrix *matrix,
  Data *data
)
{
  fill_system_matrix_stack_description (stkd,
    data->Resistances, data->Capacities,
    matrix->Columns, matrix->Rows, matrix->Values) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
add_solid_column
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE        *debug,
#endif
  Dimensions  *dim,
  Resistances *resistances,
  double      *capacities,
  int         current_layer,
  int         current_row,
  int         current_column,
  int         *columns,
  int         *rows,
  double      *values
)
{
  double resistance        = 0.0 ;
  double diagonal_value    = 0.0 ;
  double *diagonal_pointer = NULL ;
  int    neighbour         = 0 ;
  int    added             = 0 ;
  int current_cell
    = current_layer * (dim->Grid.NRows * dim->Grid.NColumns)
      + current_row * dim->Grid.NColumns
      + current_column ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (debug,
    "%p %p %p %p %p add_solid_column  (l %2d r %5d c %5d) -> %5d\n",
    resistances, capacities, columns, rows, values,
    current_layer, current_row, current_column, current_cell) ;
#endif

  *columns = *(columns - 1) ;

  if ( current_layer > 0 )   /* BOTTOM */
  {
    *rows++ = neighbour
      = current_cell - (dim->Grid.NRows * dim->Grid.NColumns) ;

    resistance = PARALLEL (resistances->Bottom,
                           (resistances + neighbour)->Top) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  bottom  \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->Bottom, (resistances + neighbour)->Top) ;
#endif
  }

  if ( current_row > 0 )   /* SOUTH */
  {
    *rows++ = neighbour = current_cell - dim->Grid.NColumns ;

    resistance = PARALLEL (resistances->South,
                           (resistances + neighbour)->North) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  south   \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->South, (resistances + neighbour)->North) ;
#endif
  }

  if ( current_column > 0 )   /* WEST */
  {
    *rows++ = neighbour = current_cell - 1 ;

    resistance = PARALLEL (resistances->West,
                           (resistances + neighbour)->East) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  west    \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->West, (resistances + neighbour)->East) ;
#endif
    }

  /* DIAGONAL */

  *rows++          = current_cell ;
  *values          = *capacities ;
  diagonal_pointer = values++ ;

  (*columns)++ ;
  added ++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%d\t ", current_cell) ;
  fgetpos (debug, &diag_fposition) ;
  fprintf (debug, "           \n") ;
#endif

  if ( current_column < dim->Grid.NColumns - 1 )   /* EAST */
  {
    *rows++ = neighbour = current_cell + 1 ;

    resistance = PARALLEL (resistances->East,
                           (resistances + neighbour)->West) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added ++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  east    \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->East, (resistances + neighbour)->West) ;
#endif
  }

  if ( current_row < dim->Grid.NRows - 1 )   /* NORTH */
  {
    *rows++ = neighbour = current_cell + dim->Grid.NColumns ;

    resistance = PARALLEL (resistances->North,
                           (resistances + neighbour)->South) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  north   \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->North, (resistances + neighbour)->South) ;
#endif
  }

  if ( current_layer < dim->Grid.NLayers - 1) /* TOP */
  {
    *rows++ = neighbour
      = current_cell + (dim->Grid.NColumns * dim->Grid.NRows) ;

    resistance = PARALLEL (resistances->Top,
                           (resistances + neighbour)->Bottom) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  top     \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->Top, (resistances + neighbour)->Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fgetpos (debug, &last_fpos) ;
  fsetpos (debug, &diag_fposition) ;
  fprintf (debug, "%.5e", *diagonal_pointer) ;
  fsetpos (debug, &last_fpos) ;

  fprintf (debug, "  %d (+%d)\n", *columns, added) ;
#endif

  return added ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
add_liquid_column
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE        *debug,
#endif
  Dimensions  *dim,
  Resistances *resistances,
  double      *capacities,
  int         current_layer,
  int         current_row,
  int         current_column,
  int         *columns,
  int         *rows,
  double      *values
)
{
  double resistance        = 0.0 ;
  double diagonal_value    = 0.0 ;
  double *diagonal_pointer = NULL ;
  int    neighbour         = 0 ;
  int    added             = 0 ;
  int current_cell
    = current_layer * (dim->Grid.NRows * dim->Grid.NColumns)
      + current_row * dim->Grid.NColumns
      + current_column ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (debug,
    "%p %p %p %p %p add_liquid_column  (l %2d r %5d c %5d) -> %5d\n",
    resistances, capacities, columns, rows, values,
    current_layer, current_row, current_column, current_cell) ;
#endif

  *columns = *(columns - 1) ;

  if ( current_layer > 0 )   /* BOTTOM */
  {
    *rows++ = neighbour
      = current_cell - (dim->Grid.NRows * dim->Grid.NColumns) ;

    resistance = PARALLEL (resistances->Bottom,
                           (resistances + neighbour)->Top) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  bottom  \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->Bottom, (resistances + neighbour)->Top) ;
#endif
  }

  if ( current_row > 0 )   /* SOUTH */
  {
    *rows++   = current_cell - dim->Grid.NColumns ;
    *values++ = resistances->North ; // == (C)

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  south   \t%d\t%.5e\n",
      neighbour, resistances->North) ;
#endif
  }

  if ( current_column > 0 )   /* WEST */
  {
    *rows++ = neighbour = current_cell - 1 ;

    resistance = PARALLEL (resistances->West,
                           (resistances + neighbour)->East) ;

    *values++        = -resistance ;
    diagonal_value  +=  resistance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  west    \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->West, (resistances + neighbour)->East) ;
#endif
  }

  /* DIAGONAL */

  *rows++          = current_cell ;
  *values          = *capacities ;
  diagonal_pointer = values++ ;

  (*columns)++ ;
  added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%d\t ", current_cell) ;
  fgetpos (debug, &diag_fposition) ;
  fprintf (debug, "           \n") ;
#endif

  if ( current_column < dim->Grid.NColumns - 1 )    /* EAST */
  {
    *rows++ = neighbour = current_cell + 1 ;

    resistance = PARALLEL (resistances->East,
                           (resistances + neighbour)->West) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  east    \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->East, (resistances + neighbour)->West) ;
#endif
  }

  if ( current_row < dim->Grid.NRows - 1 )   /* NORTH */
  {
    *rows++   = current_cell + dim->Grid.NColumns ;
    *values++ = resistances->South ; // == -C

    (*columns)++ ;
    added ++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  north   \t%d\t%.5e\n",
      neighbour, resistances->South) ;
#endif
  }

  if ( current_layer < dim->Grid.NLayers - 1)  /* TOP */
  {
    *rows++ = neighbour
      = current_cell + (dim->Grid.NColumns * dim->Grid.NRows) ;

    resistance = PARALLEL (resistances->Top,
                           (resistances + neighbour)->Bottom) ;

    *values++       = -resistance ;
    diagonal_value +=  resistance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  top     \t%d\t%.5e = %.5e || %.5e\n",
      neighbour, *(values-1),
      resistances->Top, (resistances + neighbour)->Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (current_row == 0 || current_row == dim->Grid.NRows - 1)

    *diagonal_pointer += resistances->North ; // == (C)

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fgetpos (debug, &last_fpos) ;
  fsetpos (debug, &diag_fposition) ;
  fprintf (debug, "%.5e", *diagonal_pointer) ;
  fsetpos (debug, &last_fpos) ;

  fprintf (debug, "  %d (+%d)\n", *columns, added) ;
#endif

  return added ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
