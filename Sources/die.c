/******************************************************************************
 *                                                                            *
 * Source file "Sources/die.c"                                                *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "die.h"

void
init_die (Die *die)
{
  if (die != NULL)
  {
    die->Id         = NULL ;
    die->LayersList = NULL ;
    die->NLayers    = 0 ;
    die->SourcesId  = 0 ;
    die->Next       = NULL ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Die *
malloc_die (void)
{
  Die *die = (Die *) malloc ( sizeof(Die) ) ;

  init_die (die) ;

  return die ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_die (Die *die)
{
  free (die->Id) ;
  free_layers_list (die->LayersList) ;
  free (die) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_dies_list (Die *list)
{
  Die *next_die ;

  for ( ; list != NULL ; list = next_die->Next)
  {
    next_die = list->Next ;

    free_die(list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_die (FILE *stream, char* prefix, Die *die)
{
  fprintf (stream, "%sDie %s:\n", prefix, die->Id) ;
  fprintf (stream, "%s  Number of layers  %d\n", prefix, die->NLayers);
  fprintf (stream, "%s  Sources on layer  %d\n", prefix,die->SourcesId ) ;

  char *new_prefix = (char *) malloc (sizeof(char)*(strlen(prefix) + 2));

  strcpy (new_prefix, prefix) ;
  strcat (new_prefix, "  ") ;

  print_layers_list (stream, new_prefix, die->LayersList) ;

  free (new_prefix) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_dies_list (FILE *stream, char* prefix, Die *list)
{
  for ( ; list != NULL ; list = list->Next)
  {
    print_die (stream, prefix, list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Die *
find_die_in_list (Die* list, char *id)
{
  for ( ; list != NULL ; list = list->Next)
  {
    if (strcmp(list->Id, id) == 0)
    {
      break ;
    }
  }

  return list ;
}
