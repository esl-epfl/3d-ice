/******************************************************************************
 *                                                                            *
 * Source file "Source/floorplan_element.c"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>

#include "floorplan_element.h"

#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_floorplan_element
(
  struct FloorplanElement *floorplan_element
)
{
  floorplan_element->Id         = NULL ;

  floorplan_element->SW_X       = 0 ;
  floorplan_element->SW_Y       = 0 ;

  floorplan_element->Length     = 0 ;
  floorplan_element->Width      = 0 ;

  floorplan_element->SW_Row     = 0 ;
  floorplan_element->SW_Column  = 0 ;

  floorplan_element->NE_Row     = 0 ;
  floorplan_element->NE_Column  = 0 ;

  floorplan_element->PowerValue = 0.0 ;

  floorplan_element->Next       = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

struct FloorplanElement *
alloc_and_init_floorplan_element
(
  void
)
{
  struct FloorplanElement *floorplan_element
    = (struct FloorplanElement *) malloc ( sizeof(struct FloorplanElement) );

  if (floorplan_element != NULL)

    init_floorplan_element(floorplan_element) ;

  return floorplan_element ;
}

/*****************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_floorplan_element
(
  struct FloorplanElement *floorplan_element
)
{
  free (floorplan_element->Id) ;
  free (floorplan_element) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_floorplan_elements_list
(
  struct FloorplanElement *list
)
{
  struct FloorplanElement *next ;

  for ( ; list != NULL ; list = next)
  {
    next = list->Next ;
    free_floorplan_element (list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_floorplan_element
(
  FILE             *stream,
  char             *prefix,
  struct FloorplanElement *floorplan_element
)
{
  fprintf (stream,
    "%sFloorplan Element %s",  prefix, floorplan_element->Id) ;
  fprintf (stream,
    " ( %d , %d ) %d x %d \n",
    floorplan_element->SW_X, floorplan_element->SW_Y,
    floorplan_element->Length, floorplan_element->Width) ;
  fprintf (stream,
    "%s  Rows (%d - %d)",
    prefix, floorplan_element->SW_Row, floorplan_element->NE_Row) ;
  fprintf (stream,
    " Columns (%d - %d)\n",
    floorplan_element->SW_Column, floorplan_element->NE_Column) ;
  fprintf (stream,
    "%s  Power %.4e\n", prefix, floorplan_element->PowerValue) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_floorplan_elements_list
(
  FILE *stream,
  char *prefix,
  struct FloorplanElement *list
)
{
  for ( ; list != NULL ; list = list->Next)

    print_floorplan_element (stream, prefix, list) ;

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

struct FloorplanElement *
find_floorplan_element_in_list
(
  struct FloorplanElement *list,
  char             *id
)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp (list->Id, id) == 0) break ;

  return list ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
check_intersection
(
  struct FloorplanElement *floorplan_element_a,
  struct FloorplanElement *floorplan_element_b
)
{
  if ((floorplan_element_a->SW_X + floorplan_element_a->Length)
         <= floorplan_element_b->SW_X
      || floorplan_element_a->SW_X
           >= (floorplan_element_b->SW_X + floorplan_element_b->Length))

    return 0;

  if ((floorplan_element_a->SW_Y + floorplan_element_a->Width)
         <= floorplan_element_b->SW_Y
      || floorplan_element_a->SW_Y
           >= (floorplan_element_b->SW_Y + floorplan_element_b->Width))

    return 0;

  return 1;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
get_max_temperature_floorplan_element
(
  struct FloorplanElement *floorplan_element,
  struct Dimensions       *dimensions,
  double           *temperatures,
  double           *max_temperature
)
{
  int row    = floorplan_element->SW_Row ;
  int column = floorplan_element->SW_Column ;

  *max_temperature
    = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  for
  (
    ;
   row <= floorplan_element->NE_Row ;
   row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column < floorplan_element->NE_Column ;
      column++
    )
      *max_temperature
        = MAX
          (
            temperatures [get_cell_offset_in_layer (dimensions, row, column)],
            *max_temperature
          ) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
get_min_temperature_floorplan_element
(
  struct FloorplanElement *floorplan_element,
  struct Dimensions       *dimensions,
  double           *temperatures,
  double           *min_temperature
)
{
  int row    = floorplan_element->SW_Row ;
  int column = floorplan_element->SW_Column ;

  *min_temperature
    = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  for
  (
    ;
    row <= floorplan_element->NE_Row ;
    row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column < floorplan_element->NE_Column ;
      column++
    )
      *min_temperature
        = MIN
          (
            temperatures [get_cell_offset_in_layer (dimensions, row, column)],
            *min_temperature
          ) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
get_avg_temperature_floorplan_element
(
  struct FloorplanElement *floorplan_element,
  struct Dimensions       *dimensions,
  double           *temperatures,
  double           *avg_temperature
)
{
  int    row, column ;
  double counter = 0.0 ;

  *avg_temperature = 0.0 ;

  for
  (
    row =  floorplan_element->SW_Row ;
    row <= floorplan_element->NE_Row ;
    row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column < floorplan_element->NE_Column ;
      column++,
      counter++
    )

      *avg_temperature
        += temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  *avg_temperature /= counter ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
get_min_avg_max_temperatures_floorplan_element
(
  struct FloorplanElement *floorplan_element,
  struct Dimensions       *dimensions,
  double           *temperatures,
  double           *min_temperature,
  double           *avg_temperature,
  double           *max_temperature
)
{
  int row        = floorplan_element->SW_Row ;
  int column     = floorplan_element->SW_Column ;
  double counter = 0.0 ;
  double temp ;

  *max_temperature = *min_temperature
    = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

  *avg_temperature = 0.0 ;

  for
  (
    ;
    row <= floorplan_element->NE_Row ;
    row++
  )

    for
    (
      column = floorplan_element->SW_Column ;
      column < floorplan_element->NE_Column ;
      column++, counter++
    )
    {
      temp = temperatures [get_cell_offset_in_layer (dimensions, row, column)] ;

      *max_temperature = MAX (temp, *max_temperature) ;

      *min_temperature = MIN (temp, *min_temperature) ;

      *avg_temperature += temp ;
    }

  *avg_temperature /= counter ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
