/******************************************************************************
 *                                                                            *
 * Source file "Source/build_system_matrix.c"                                 *
 *                                                                            *
 *                                          -- alessandro.vincenzi@epfl.ch -- *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "conductances.h"
#include "system_matrix.h"

#define PARALLEL(x,y)      ( (x * y) / ( x + y) )
#define LAYER_OFFSET(dim)  (dim->Grid.NRows * dim->Grid.NColumns)
#define ROW_OFFSET(dim)    (dim->Grid.NColumns)
#define COLUMN_OFFSET(dim) (1)

int
alloc_system_matrix
(
  SystemMatrix    *matrix,
  MatrixStorage_t storage,
  int             nvalues,
  int             nnz
)
{
  matrix->Size    = nvalues ;
  matrix->NNz     = nnz ;
  matrix->Storage = storage ;

  if (storage == TL_CCS_MATRIX)
  {
    matrix->columns_size = nvalues + 1 ;
    matrix->rows_size    = nnz ;
  }
  else
  {
    fprintf (stderr, "Matrix storage type %d not supported\n", storage) ;
    return 0 ;
  }

  matrix->Columns = (int *) malloc (sizeof(int) * matrix->columns_size ) ;

  if (matrix->Columns == NULL)
  {
    return 0 ;
  }

  matrix->Rows = (int *) malloc (sizeof(int) * matrix->rows_size ) ;

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
  free (matrix->Columns) ;
  free (matrix->Rows) ;
  free (matrix->Values) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_system_matrix_columns
(
  SystemMatrix *matrix,
  char *file_name
)
{
  int counter ;

  FILE * file = fopen (file_name, "w") ;

  if (file == NULL) return ;

  for (counter = 0 ; counter < matrix->columns_size ; counter++)
    fprintf (file, "%d\n", matrix->Columns[counter]);

  fclose (file) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_system_matrix_rows
(
  SystemMatrix *matrix,
  char *file_name
)
{
  int counter ;
  FILE *file = fopen (file_name, "w") ;

  if (file == NULL) return ;

  if (matrix->Storage == TL_CCS_MATRIX)

    for (counter = 0 ; counter < matrix->rows_size ; counter++)
      fprintf (file, "%d\n", matrix->Rows[counter] + 1);

  fclose (file) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_system_matrix_values
(
  SystemMatrix *matrix,
  char *file_name
)
{
  int counter ;
  FILE *file = fopen (file_name, "w") ;

  if (file == NULL) return ;

  for (counter = 0 ; counter < matrix->NNz ; counter++)
    fprintf (file, "%.6e\n", matrix->Values[counter]);

  fclose (file) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_system_matrix
(
  StackDescription *stkd,
  SystemMatrix     *matrix,
  Conductances     *conductances,
  double           *capacities
)
{
  if (matrix->Storage == TL_CCS_MATRIX)

    fill_ccs_system_matrix_stack_description
    (
      stkd,
      conductances,
      capacities,
      matrix->Columns,
      matrix->Rows,
      matrix->Values
    ) ;

  else

    fprintf (stderr, "Unsupported matrix type\n") ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
add_ccs_solid_column
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE         *debug,
#endif
  Dimensions   *dim,
  Conductances *conductances,
  double       *capacities,
  int          current_layer,
  int          current_row,
  int          current_column,
  int          *columns,
  int          *rows,
  double       *values
)
{
  double conductance        = 0.0 ;
  double diagonal_value    = 0.0 ;
  double *diagonal_pointer = NULL ;
  int    added             = 0 ;

  int current_cell
    = current_layer * LAYER_OFFSET(dim)
      + current_row * ROW_OFFSET(dim)
      + current_column ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (debug,
    "%p %p %p %p %p add_CCS_solid_column  (l %2d r %5d c %5d) -> %5d\n",
    conductances, capacities, columns, rows, values,
    current_layer, current_row, current_column, current_cell) ;
#endif

  *columns = *(columns - 1) ;

  if ( current_layer > 0 )   /* BOTTOM */
  {
    *rows++ = current_cell - LAYER_OFFSET(dim) ;

    conductance = PARALLEL (conductances->Bottom,
                           (conductances - LAYER_OFFSET(dim))->Top) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  bottom  \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->Bottom, (conductances - LAYER_OFFSET(dim))->Top) ;
#endif
  }

  if ( current_row > 0 )   /* SOUTH */
  {
    *rows++ = current_cell - ROW_OFFSET(dim) ;

    conductance = PARALLEL (conductances->South,
                           (conductances - ROW_OFFSET(dim))->North) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  south   \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->South, (conductances - ROW_OFFSET(dim))->North) ;
#endif
  }

  if ( current_column > 0 )   /* WEST */
  {
    *rows++ = current_cell - COLUMN_OFFSET(dim) ;

    conductance = PARALLEL (conductances->West,
                           (conductances - COLUMN_OFFSET(dim))->East) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  west    \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->West, (conductances - COLUMN_OFFSET(dim))->East) ;
#endif
    }

  /* DIAGONAL */

  *rows++          = current_cell ;
  *values          = *capacities ;
  diagonal_pointer = values++ ;

  (*columns)++ ;
  added ++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%d\t ", *(rows-1)) ;
  fgetpos (debug, &diag_fposition) ;
  fprintf (debug, "           \n") ;
#endif

  if ( current_column < dim->Grid.NColumns - 1 )   /* EAST */
  {
    *rows++ = current_cell + COLUMN_OFFSET(dim) ;

    conductance = PARALLEL (conductances->East,
                           (conductances + COLUMN_OFFSET(dim))->West) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added ++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  east    \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->East, (conductances + COLUMN_OFFSET(dim))->West) ;
#endif
  }

  if ( current_row < dim->Grid.NRows - 1 )   /* NORTH */
  {
    *rows++ = current_cell + ROW_OFFSET(dim) ;

    conductance = PARALLEL (conductances->North,
                           (conductances + ROW_OFFSET(dim))->South) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  north   \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->North, (conductances + ROW_OFFSET(dim))->South) ;
#endif
  }

  if ( current_layer < dim->Grid.NLayers - 1) /* TOP */
  {
    *rows++ = current_cell + LAYER_OFFSET(dim) ;

    conductance = PARALLEL (conductances->Top,
                           (conductances + LAYER_OFFSET(dim))->Bottom) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  top     \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->Top, (conductances + LAYER_OFFSET(dim))->Bottom) ;
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
add_ccs_liquid_column
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE         *debug,
#endif
  Dimensions   *dim,
  Conductances *conductances,
  double       *capacities,
  int          current_layer,
  int          current_row,
  int          current_column,
  int          *columns,
  int          *rows,
  double       *values
)
{
  double conductance        = 0.0 ;
  double diagonal_value    = 0.0 ;
  double *diagonal_pointer = NULL ;
  int    added             = 0 ;
  int current_cell
    = current_layer * LAYER_OFFSET(dim)
      + current_row * ROW_OFFSET(dim)
      + current_column ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (debug,
    "%p %p %p %p %p add_CCS_liquid_column  (l %2d r %5d c %5d) -> %5d\n",
    conductances, capacities, columns, rows, values,
    current_layer, current_row, current_column, current_cell) ;
#endif

  *columns = *(columns - 1) ;

  if ( current_layer > 0 )   /* BOTTOM */
  {
    *rows++ = current_cell - LAYER_OFFSET(dim) ;

    conductance = PARALLEL (conductances->Bottom,
                           (conductances - LAYER_OFFSET(dim))->Top) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  bottom  \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->Bottom, (conductances - LAYER_OFFSET(dim))->Top) ;
#endif
  }

  if ( current_row > 0 )   /* SOUTH */
  {
    *rows++   = current_cell - ROW_OFFSET(dim) ;
    *values++ = conductances->North ; // == (C)

    (*columns)++ ;
    added++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  south   \t%d\t%.5e\n", *(rows-1), *(values-1)) ;
#endif
  }

  if ( current_column > 0 )   /* WEST */
  {
    *rows++ = current_cell - COLUMN_OFFSET(dim) ;

    conductance = PARALLEL (conductances->West,
                           (conductances - COLUMN_OFFSET(dim))->East) ;

    *values++        = -conductance ;
    diagonal_value  +=  conductance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  west    \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->West, (conductances - COLUMN_OFFSET(dim))->East) ;
#endif
  }

  /* DIAGONAL */

  *rows++          = current_cell ;
  *values          = *capacities ;
  diagonal_pointer = values++ ;

  (*columns)++ ;
  added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug, "  diagonal\t%d\t ", *(rows-1)) ;
  fgetpos (debug, &diag_fposition) ;
  fprintf (debug, "           \n") ;
#endif

  if ( current_column < dim->Grid.NColumns - 1 )    /* EAST */
  {
    *rows++ = current_cell + COLUMN_OFFSET(dim) ;

    conductance = PARALLEL (conductances->East,
                           (conductances + COLUMN_OFFSET(dim))->West) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  east    \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->East, (conductances + COLUMN_OFFSET(dim))->West) ;
#endif
  }

  if ( current_row < dim->Grid.NRows - 1 )   /* NORTH */
  {
    *rows++   = current_cell + ROW_OFFSET(dim) ;
    *values++ = conductances->South ; // == -C

    (*columns)++ ;
    added ++ ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  north   \t%d\t%.5e\n", *(rows-1), *(values-1)) ;
#endif
  }

  if ( current_layer < dim->Grid.NLayers - 1)  /* TOP */
  {
    *rows++ = current_cell + LAYER_OFFSET(dim) ;

    conductance = PARALLEL (conductances->Top,
                           (conductances + LAYER_OFFSET(dim))->Bottom) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*columns)++ ;
    added++;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
    fprintf (debug,
      "  top     \t%d\t%.5e = %.5e || %.5e\n",
      *(rows-1), *(values-1),
      conductances->Top, (conductances + LAYER_OFFSET(dim))->Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (current_row == 0 || current_row == dim->Grid.NRows - 1)

    *diagonal_pointer += conductances->North ; // == (C)

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
