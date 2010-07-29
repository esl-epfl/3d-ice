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

extern
int
floorplan_parse (
                 struct Floorplan *floorplan,
                 Dimensions *dimensions,
                 yyscan_t scanner
                ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_floorplan
(
  struct Floorplan *floorplan
)
{
  floorplan->FileName          = NULL ;
  floorplan->NElements         = 0 ;
  floorplan->NPowerValues      = 0 ;
  floorplan->CurrentPowerValue = 0 ;
  floorplan->ElementsList      = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

struct Floorplan *
alloc_and_init_floorplan
(
  void
)
{
  struct Floorplan *floorplan
    = (struct Floorplan *) malloc ( sizeof(struct Floorplan) );

  if (floorplan != NULL) init_floorplan (floorplan) ;

  return floorplan ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_floorplan
(
  struct Floorplan *floorplan
)
{
  free_floorplan_elements_list (floorplan->ElementsList) ;
  free (floorplan->FileName) ;
  free (floorplan) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_floorplan
(
  struct Floorplan *floorplan,
  Dimensions *dimensions
)
{
  FILE     *input ;
  int      result ;
  yyscan_t scanner ;

  input = fopen (floorplan->FileName, "r") ;
  if(input == NULL)
    {
      perror(floorplan->FileName) ;
      return 1;
    }

  floorplan_lex_init (&scanner) ;

  floorplan_set_in (input, scanner) ;

  //floorplan_set_debug (1, scanner) ;

  result = floorplan_parse (floorplan, dimensions, scanner) ;

  floorplan_lex_destroy (scanner) ;

  fclose (input) ;

  return result ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_floorplan
(
  FILE      *stream,
  char      *prefix,
  struct Floorplan *floorplan
)
{
  fprintf(stream,
    "%sFloorplan read from file %s\n", prefix, floorplan->FileName) ;

  print_floorplan_elements_list (stream, prefix, floorplan->ElementsList) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
check_intersections
(
  struct Floorplan        *floorplan,
  struct FloorplanElement *floorplan_element
)
{
  int result = 0 ;
  struct FloorplanElement *tmp = floorplan->ElementsList ;

  for ( ; tmp != NULL; tmp = tmp->Next)

    if (check_intersection (tmp, floorplan_element))
      {
        fprintf (stderr,
          "%s: found intersection between %s and %s.\n",
          floorplan->FileName, tmp->Id, floorplan_element->Id) ;
        result ++ ;
      }

  return result ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
check_location
(
  struct Floorplan        *floorplan,
  struct FloorplanElement *floorplan_element,
  Dimensions       *dimensions
)
{
  if (   (floorplan_element->SW_X <  0)
          || (floorplan_element->SW_X + floorplan_element->Length
              > get_chip_length (dimensions))
      || (floorplan_element->SW_Y <  0)
          || (floorplan_element->SW_Y + floorplan_element->Width
              > get_chip_width (dimensions)) )
  {
    fprintf (stderr,
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
align_to_grid
(
  struct Floorplan        *floorplan,
  struct FloorplanElement *floorplan_element,
  Dimensions       *dimensions
)
{
  double cx, cy ;
  int column, row ;

  // West side

  cx = get_cell_length (dimensions, 0) / 2.0 ;

  for
  (
    column = 0 ;
    cx < floorplan_element->SW_X ;
    column++
  )
    if (column == 0)

      cx += (get_cell_length (dimensions, 0) / 2.0)
            + (get_cell_length (dimensions, 1) / 2.0) ;

    else

      cx += get_cell_length (dimensions, column) ;

  floorplan_element->SW_Column = column ;

  // East side

  for
  (
    ;
    cx <= floorplan_element->SW_X + floorplan_element->Length ;
    column ++
  )
    if (column == 0)

      cx = get_cell_length (dimensions, 0) / 2.0 ;

    else if (column == get_number_of_columns (dimensions) - 1)

      cx = get_chip_length (dimensions)
           - (get_cell_length (dimensions, column) / 2.0) ;

    else

      cx += get_cell_length (dimensions, column) ;

  floorplan_element->NE_Column = column - 1 ;

  // South side

  for
  (
    cy = (get_cell_width (dimensions) / 2.0),
    row = 0 ;
    cy < floorplan_element->SW_Y ;
    cy += get_cell_width (dimensions),
    row++
  ) ;

  floorplan_element->SW_Row = row ;

  // North side

  for
  (
    ;
    cy <= floorplan_element->SW_Y + floorplan_element->Width ;
    cy += get_cell_width (dimensions),
    row++
  ) ;

  floorplan_element->NE_Row = row - 1 ;

  if (floorplan_element->NE_Row - floorplan_element->SW_Row == 0
      && floorplan_element->NE_Column - floorplan_element->SW_Column == 0)
  {
    fprintf (stderr,
      "%s: no cells belong to floorplan element %s.\n",
      floorplan->FileName, floorplan_element->Id) ;
    return 1 ;
  }

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

//void
//insert_power_values_floorplan
//(
//  struct Floorplan *floorplan,
//  double    *power_values
//)
//{
//  struct FloorplanElement *flp_el = floorplan->ElementsList ;
//
//  for ( ; flp_el != NULL ; flp_el = flp_el->Next)
//
//    flp_el->PowerValue = *power_values++ ;
//
//}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

//int
//insert_power_value_floorplan_element
//(
//  struct Floorplan *floorplan,
//  char *floorplan_element_id,
//  double power_value
//)
//{
//  struct FloorplanElement *flp_el = find_floorplan_element_in_list
//                                    (
//                                      floorplan->ElementsList,
//                                      floorplan_element_id
//                                    ) ;
//  if (flp_el == NULL)
//
//    return -3 ;
//
//  flp_el->PowerValue = power_value ;
//
//  return 0 ;
//}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_max_temperature_floorplan
(
  struct Floorplan  *floorplan,
  char       *floorplan_element_id,
  Dimensions *dimensions,
  double     *temperatures,
  double     *max_temperature
)
{
  struct FloorplanElement *flp_el = find_floorplan_element_in_list
                                    (
                                      floorplan->ElementsList,
                                      floorplan_element_id
                                    ) ;
  if (flp_el == NULL)

    return -3 ;

  get_max_temperature_floorplan_element
  (
    flp_el,
    dimensions,
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
  struct Floorplan  *floorplan,
  char       *floorplan_element_id,
  Dimensions *dimensions,
  double     *temperatures,
  double     *min_temperature
)
{
  struct FloorplanElement *flp_el = find_floorplan_element_in_list
                                    (
                                      floorplan->ElementsList,
                                      floorplan_element_id
                                    ) ;

  if (flp_el == NULL)

    return -3 ;

  get_min_temperature_floorplan_element
  (
    flp_el,
    dimensions,
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
  struct Floorplan  *floorplan,
  char       *floorplan_element_id,
  Dimensions *dimensions,
  double     *temperatures,
  double     *avg_temperature
)
{
  struct FloorplanElement *flp_el = find_floorplan_element_in_list
                                    (
                                      floorplan->ElementsList,
                                      floorplan_element_id
                                    ) ;

  if (flp_el == NULL)

    return -3 ;

  get_avg_temperature_floorplan_element
  (
    flp_el,
    dimensions,
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
  struct Floorplan  *floorplan,
  char       *floorplan_element_id,
  Dimensions *dimensions,
  double     *temperatures,
  double     *min_temperature,
  double     *avg_temperature,
  double     *max_temperature
)
{
  struct FloorplanElement *flp_el = find_floorplan_element_in_list
                                    (
                                      floorplan->ElementsList,
                                      floorplan_element_id
                                    ) ;

  if (flp_el == NULL)

    return -3 ;

  get_min_avg_max_temperatures_floorplan_element
  (
    flp_el,
    dimensions,
    temperatures,
    min_temperature, avg_temperature, max_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_max_temperatures_floorplan
(
  struct Floorplan  *floorplan,
  Dimensions *dimensions,
  double     *temperatures,
  double     *max_temperature
)
{
  struct FloorplanElement *flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)

    get_max_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      max_temperature++
    );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_min_temperatures_floorplan
(
  struct Floorplan  *floorplan,
  Dimensions *dimensions,
  double     *temperatures,
  double     *min_temperature
)
{
  struct FloorplanElement *flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)
  {
    get_min_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      min_temperature++
    );
  }

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_avg_temperatures_floorplan
(
  struct Floorplan  *floorplan,
  Dimensions *dimensions,
  double     *temperatures,
  double     *avg_temperature
)
{
  struct FloorplanElement *flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)

    get_avg_temperature_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      avg_temperature++
    );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_min_avg_max_temperatures_floorplan
(
  struct Floorplan  *floorplan,
  Dimensions *dimensions,
  double     *temperatures,
  double     *min_temperature,
  double     *avg_temperature,
  double     *max_temperature
)
{
  struct FloorplanElement *flp_el = floorplan->ElementsList ;

  for ( ; flp_el != NULL ; flp_el = flp_el->Next)

    get_min_avg_max_temperatures_floorplan_element
    (
      flp_el,
      dimensions,
      temperatures,
      min_temperature++, avg_temperature++, max_temperature++
    );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
