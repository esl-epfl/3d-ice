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

SystemVector *
alloc_system_vector (int size)
{
  SystemVector *vector ;

  vector->Values = malloc ( sizeof(double) * size ) ;

  if (vector->Values == NULL)
  {
    return NULL ;
  }

  vector->Size = size ;

  return vector ;
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
