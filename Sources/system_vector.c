/******************************************************************************
 *                                                                            *
 * Source file "Sources/system_vector.c"                                      *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "system_vector.h"

int
alloc_system_vector (struct SystemVector *vector, int size)
{
  if (vector == NULL) return 0 ;

  vector->Values = (double *) malloc ( sizeof(double) * size ) ;
  vector->Size   = size ;

  if (vector->Values == NULL)
  {
    return 0 ;
  }

  return 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_system_vector (struct SystemVector *vector,
                    double *source,
                    double *capacity,
                    double *temperature)
{
  int count ;
  double *value = vector->Values ;

#ifdef DEBUG_FILL_SYSTEM_VECTOR
  FILE *debug = fopen("fill_system_vector.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_system_vector.txt") ;
    return ;
  }
#endif

  for(count = 0 ; count < vector->Size ; count++)
  {
    *value++ = *source++ + *capacity++ * *temperature++ ;
#ifdef DEBUG_FILL_SYSTEM_VECTOR
    fprintf (debug, "%10.7f = %.7f + %.7f * %.7f\n",
      *(value-1), *(source-1), *(capacity-1), *(temperature-1));
#endif
  }

#ifdef DEBUG_FILL_SYSTEM_VECTOR
  fclose (debug);
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void free_system_vector (struct SystemVector *vector)
{
  if (vector == NULL) return ;

  free (vector->Values) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_system_vector (struct SystemVector *vector, char *file_name)
{
  int counter ;
  FILE *file = fopen (file_name, "w") ;

  if (file == NULL) return ;

  fprintf (file, "%d\n", vector->Size);

  for (counter = 0 ; counter < vector->Size ; counter++)
    fprintf (file, "%.5e\n", vector->Values[counter]);

  fclose (file) ;
}
