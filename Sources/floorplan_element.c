/******************************************************************************
 *                                                                            *
 * Source file "Source/floorplan_element.c"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "floorplan_element.h"

void
init_floorplan_element (FloorplanElement *floorplan_element)
{
  if (floorplan_element != NULL)
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
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

FloorplanElement *
alloc_and_init_floorplan_element (void)
{
  FloorplanElement *floorplan_element
    = (FloorplanElement *) malloc ( sizeof(FloorplanElement) );

  init_floorplan_element(floorplan_element) ;

  return floorplan_element ;
}

/*****************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_floorplan_element (FloorplanElement *flp_el)
{
  free (flp_el->Id) ;
  free (flp_el) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_floorplan_elements_list (FloorplanElement *list)
{
  FloorplanElement *next_floorplan_element ;

  for ( ; list != NULL ; list = next_floorplan_element)
  {
    next_floorplan_element = list->Next ;

    free_floorplan_element (list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_floorplan_element (FILE *stream,
                         char *prefix,
                         FloorplanElement *floorplan_element)
{
  fprintf(stream, "%sFloorplan Element %s",  prefix, floorplan_element->Id) ;

  fprintf(stream, " ( %d , %d ) %d x %d \n",
    floorplan_element->SW_X, floorplan_element->SW_Y,
    floorplan_element->Length, floorplan_element->Width) ;

  fprintf(stream, "%s  Rows (%d - %d)",
    prefix, floorplan_element->SW_Row, floorplan_element->NE_Row) ;

  fprintf(stream, " Columns (%d - %d)\n",
    floorplan_element->SW_Column, floorplan_element->NE_Column) ;

  fprintf(stream, "%s  Power %.4e\n", prefix, floorplan_element->PowerValue) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_floorplan_elements_list (FILE *stream,
                               char *prefix,
                               FloorplanElement *list)
{
  for ( ; list != NULL ; list = list->Next)
  {
    print_floorplan_element(stream, prefix, list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
check_intersection (FloorplanElement *floorplan_element_a,
                    FloorplanElement *floorplan_element_b)
{
  if ((floorplan_element_a->SW_X + floorplan_element_a->Length)
         <= floorplan_element_b->SW_X
      || floorplan_element_a->SW_X
           >= (floorplan_element_b->SW_X + floorplan_element_b->Length))
  {
    return 0;
  }
  if ((floorplan_element_a->SW_Y + floorplan_element_a->Width)
         <= floorplan_element_b->SW_Y
      || floorplan_element_a->SW_Y
           >= (floorplan_element_b->SW_Y + floorplan_element_b->Width))
  {
    return 0;
  }

  return 1;
}
