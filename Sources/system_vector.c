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

#include "system_vector.h"

int
alloc_system_vector (SystemVector *vector, int size)
{
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
fill_system_vector (SystemVector *vector,
                    double *source,
                    double *capacity,
                    double *temperature)
{
  int count ;
  double *value = vector->Values ;

  for(count = 0 ; count < vector->Size ; count++)
  {
    *value++ = *source++ + *capacity++ * *temperature++ ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void free_system_vector (SystemVector *vector)
{
  free (vector->Values) ;
}
