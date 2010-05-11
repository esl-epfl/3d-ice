/******************************************************************************
 *                                                                            *
 * Source file "Source/floorplan.c"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "floorplan.h"
#include "floorplan_parser.h"
#include "floorplan_scanner.h"

extern int floorplan_parse (Floorplan *floorplan,
                            Dimensions *dim,
                            yyscan_t scanner) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_floorplan (Floorplan *floorplan)
{
  if (floorplan == NULL) return ;

  floorplan->FileName     = NULL ;
  floorplan->NElements    = 0 ;
  floorplan->ElementsList = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Floorplan *
alloc_and_init_floorplan (void)
{
  Floorplan *floorplan = (Floorplan *) malloc ( sizeof(Floorplan) );

  init_floorplan (floorplan) ;

  return floorplan ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_floorplan (Floorplan *floorplan)
{
  if (floorplan == NULL) return ;

  free(floorplan->FileName) ;
  free_floorplan_elements_list(floorplan->ElementsList) ;
  free (floorplan) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_floorplan (Floorplan *floorplan, Dimensions *dim)
{
  FILE *input ;
  int result ;
  yyscan_t scanner ;

  input = fopen(floorplan->FileName, "r") ;
  if(input == NULL)
    {
      perror(floorplan->FileName) ;
      return 1;
    }

  floorplan_lex_init(&scanner) ;

  floorplan_set_in(input, scanner) ;

  result = floorplan_parse(floorplan, dim, scanner) ;

  floorplan_lex_destroy(scanner) ;
  fclose(input) ;

  return result ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_floorplan (FILE *stream, char *prefix, Floorplan *floorplan)
{
  fprintf(stream,
    "%sFloorplan read from file %s\n", prefix, floorplan->FileName) ;

  print_floorplan_elements_list (stream, prefix, floorplan->ElementsList) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
check_intersections (Floorplan *floorplan, FloorplanElement *floorplan_element)
{
  int result = 0 ;
  FloorplanElement *tmp ;

  for (tmp = floorplan->ElementsList ; tmp != NULL; tmp = tmp->Next)
  {
    if (check_intersection(tmp, floorplan_element))
      {
        fprintf(stderr, "%s: found intersection between %s and %s.\n",
                floorplan->FileName, tmp->Id, floorplan_element->Id) ;
        result ++ ;
      }
  }
  return result ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
check_location (Floorplan *floorplan,
                FloorplanElement *floorplan_element,
                ChipDimensions *cd)
{
  if (   (floorplan_element->SW_X <  0)
          || (floorplan_element->SW_X + floorplan_element->Length
              > cd->Length * 1000.0)
      || (floorplan_element->SW_Y <  0)
          || (floorplan_element->SW_Y + floorplan_element->Width
              > cd->Width * 1000.0) )
  {
    fprintf(stderr,
      "%s: floorplan element %s is outside of the floorplan.\n",
      floorplan->FileName, floorplan_element->Id) ;
    return 1 ;
  }

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
align_to_grid (Floorplan *floorplan,
               FloorplanElement *floorplan_element,
               Dimensions *dim)
{
  double cx, cy ;
  int column, row ;

  // West side

  cx = (dim->Cell.FirstLength / 2.0) ;

  for (column = 0 ; cx < floorplan_element->SW_X ; column++)
    {
      if (column == 0)
        cx += (dim->Cell.FirstLength / 2.0) + (dim->Cell.Length / 2.0) ;
      else
        cx += dim->Cell.Length ;
    }
  floorplan_element->SW_Column = column ;

  // East side

  for ( ;
       cx <= floorplan_element->SW_X + floorplan_element->Length ;
       column ++)
    {
      if (column == 0)
        cx = dim->Cell.FirstLength / 2.0 ;
      else if (column == dim->Grid.NColumns - 1)
        cx = (dim->Chip.Length * 1000.0) - (dim->Cell.LastLength / 2.0) ;
      else
        cx += dim->Cell.Length ;
    }
  floorplan_element->NE_Column = column - 1 ;

  // South side

  for (cy = (dim->Cell.Width / 2.0), row = 0 ;
       cy < floorplan_element->SW_Y ;
       cy += dim->Cell.Width, row++) ;

  floorplan_element->SW_Row = row ;

  // North side

  for ( ;
       cy <= floorplan_element->SW_Y + floorplan_element->Width ;
       cy += dim->Cell.Width, row++) ;

  floorplan_element->NE_Row = row - 1 ;

  if (floorplan_element->NE_Row - floorplan_element->SW_Row == 0
      && floorplan_element->NE_Column - floorplan_element->SW_Column == 0)
  {
    fprintf(stderr, "%s: no cells belong to floorplan element %s.\n",
      floorplan->FileName, floorplan_element->Id) ;
    return 1 ;
  }

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
insert_power_values_floorplan
(
  Floorplan *floorplan,
  double *power_values
)
{
  FloorplanElement *flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)
  {
    flp_el->PowerValue = *power_values++ ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
insert_power_value_floorplan_element
(
  Floorplan *floorplan,
  char *floorplan_element_id,
  double power_value
)
{
  FloorplanElement *flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  flp_el->PowerValue = power_value ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_max_temperature_floorplan
(
  Floorplan *floorplan,
  char *floorplan_element_id,
  Dimensions *dim,
  double *temperatures,
  double *max_temperature
)
{
  FloorplanElement *flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_max_temperature_floorplan_element
  (
    flp_el,
    dim,
    temperatures,
    max_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_min_temperature_floorplan
(
  Floorplan *floorplan,
  char *floorplan_element_id,
  Dimensions *dim,
  double *temperatures,
  double *min_temperature
)
{
  FloorplanElement *flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_min_temperature_floorplan_element
  (
    flp_el,
    dim,
    temperatures,
    min_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_avg_temperature_floorplan
(
  Floorplan *floorplan,
  char *floorplan_element_id,
  Dimensions *dim,
  double *temperatures,
  double *avg_temperature
)
{
  FloorplanElement *flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_avg_temperature_floorplan_element
  (
    flp_el,
    dim,
    temperatures,
    avg_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_min_avg_max_temperatures_floorplan
(
  Floorplan *floorplan,
  char *floorplan_element_id,
  Dimensions *dim,
  double *temperatures,
  double *min_temperature,
  double *avg_temperature,
  double *max_temperature
)
{
  FloorplanElement *flp_el = find_floorplan_element_in_list
                             (
                               floorplan->ElementsList,
                               floorplan_element_id
                             ) ;

  if (flp_el == NULL)

    return -3 ;

  get_min_avg_max_temperatures_floorplan_element
  (
    flp_el,
    dim,
    temperatures,
    min_temperature,
    avg_temperature,
    max_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
