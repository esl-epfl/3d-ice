/******************************************************************************
 *                                                                            *
 * Source file "Sources/stack_description.c"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "stack_description.h"
#include "stack_description_parser.h"
#include "stack_description_scanner.h"

extern int stack_description_parse (StackDescription *stkd, yyscan_t scanner) ;

static int fill_floorplans (StackDescription *stkd) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_stack_description (StackDescription *stkd)
{
  if (stkd == NULL) return ;

  stkd->FileName          = NULL ;
  stkd->MaterialsList     = NULL ;
  stkd->Channel           = NULL ;
  stkd->DiesList          = NULL ;
  stkd->StackElementsList = NULL ;
  stkd->Dimensions        = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_stack_description (StackDescription* stkd , char *filename)
{
  FILE *input;
  int result;
  yyscan_t scanner;

  input = fopen(filename, "r");
  if(input == NULL)
    {
      perror(filename);
      return 1;
    }

  stkd->FileName = strdup(filename);

  stack_description_lex_init(&scanner);

  //stack_set_debug(1 , scanner);

  stack_description_set_in(input, scanner);

  result = stack_description_parse(stkd, scanner);

  stack_description_lex_destroy(scanner);

  fclose(input);

  if (result == 1)
  {
    return result ;
  }

  stkd->Dimensions->Grid.NRows

      = (stkd->Dimensions->Chip.Width * 1000.0) / stkd->Dimensions->Cell.Width ;

  stkd->Dimensions->Grid.NColumns

      = ( ( (stkd->Dimensions->Chip.Length * 1000.0)
            - stkd->Dimensions->Cell.FirstLength
            - stkd->Dimensions->Cell.LastLength )
          / stkd->Dimensions->Cell.Length )
        + 2 ;

  stkd->Dimensions->Grid.NCells

      = stkd->Dimensions->Grid.NLayers
        * stkd->Dimensions->Grid.NRows
        * stkd->Dimensions->Grid.NColumns ;

  stkd->Dimensions->Grid.NNz

      =   stkd->Dimensions->Grid.NLayers
          * (
                stkd->Dimensions->Grid.NRows
                * (3 * stkd->Dimensions->Grid.NColumns - 2)
              + 2 * stkd->Dimensions->Grid.NColumns
                * (stkd->Dimensions->Grid.NRows - 1)
            )
        + (stkd->Dimensions->Grid.NLayers - 1 ) * 2
          * stkd->Dimensions->Grid.NRows * stkd->Dimensions->Grid.NColumns ;

  // Now we know all the dimensions so we can parse the floorplan
  // files and check if the floorplan elements are well positionated.

  return fill_floorplans(stkd) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_stack_description (StackDescription *stkd)
{
  if (stkd == NULL) return ;

  free_materials_list      (stkd->MaterialsList) ;
  free_channel             (stkd->Channel) ;
  free_dies_list           (stkd->DiesList) ;
  free_stack_elements_list (stkd->StackElementsList) ;
  free_dimensions          (stkd->Dimensions) ;
  free                     (stkd->FileName) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_stack_description (FILE *stream, char *prefix, StackDescription *stkd)
{
  fprintf(stream, "%sStack read from file %s\n", prefix, stkd->FileName) ;

  print_materials_list      (stream, prefix, stkd->MaterialsList) ;
  print_channel             (stream, prefix, stkd->Channel);
  print_dies_list           (stream, prefix, stkd->DiesList) ;
  print_stack_elements_list (stream, prefix, stkd->StackElementsList) ;
  print_dimensions          (stream, prefix, stkd->Dimensions) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
int
fill_floorplans (StackDescription *stkd)
{
  int result = 0 ;
  StackElement *stack_element = stkd->StackElementsList ;

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    if (stack_element->Type == TL_STACK_ELEMENT_DIE)
    {
      result += fill_floorplan(stack_element->Floorplan, stkd->Dimensions) ;
    }
  }

  return result ;
 }

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_resistances_stack_description
(
  StackDescription *stkd,
  Resistances *resistances
)
{
  StackElement *stack_element ;
  int current_layer ;

#ifdef DEBUG_FILL_RESISTANCES
  FILE *debug = fopen("fill_resistances_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_resistances_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p fill_resistances_stack_description ( l %d r %d c %d )\n",
    resistances,
    stkd->Dimensions->Grid.NLayers,
    stkd->Dimensions->Grid.NRows,
    stkd->Dimensions->Grid.NColumns) ;
#endif

  for
  (
    stack_element = stkd->StackElementsList,
    current_layer = 0 ;

    stack_element != NULL ;

    current_layer += stack_element->NLayers,
    stack_element = stack_element->Next
  )
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        resistances = fill_resistances_die
                      (
#ifdef DEBUG_FILL_RESISTANCES
                        debug,
#endif
                        stack_element->Pointer.Die,
                        resistances,
                        stkd->Dimensions,
                        current_layer
                      ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        resistances = fill_resistances_layer
                      (
#ifdef DEBUG_FILL_RESISTANCES
                        debug,
#endif
                        stack_element->Pointer.Layer,
                        resistances,
                        stkd->Dimensions,
                        current_layer
                      ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        resistances = fill_resistances_channel
                      (
#ifdef DEBUG_FILL_RESISTANCES
                        debug,
#endif
                        stkd->Channel,
                        resistances,
                        stkd->Dimensions,
                        current_layer
                      ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_elementy->Type */

  } /* stack_element */

#ifdef DEBUG_FILL_RESISTANCES
  fclose (debug) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_capacities_stack_description
(
  StackDescription *stkd,
  double *capacities,
  double delta_time
)
{
  StackElement *stack_element ;

#ifdef DEBUG_FILL_CAPACITIES
  int current_layer ;

  FILE *debug = fopen("fill_capacities_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_capacities_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p fill_capacities_stack_description ( l %d r %d c %d )\n",
    capacities,
    stkd->Dimensions->Grid.NLayers,
    stkd->Dimensions->Grid.NRows,
    stkd->Dimensions->Grid.NColumns) ;
#endif

  for
  (
#ifdef DEBUG_FILL_CAPACITIES
    current_layer = 0 ,
#endif
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

#ifdef DEBUG_FILL_CAPACITIES
    current_layer += stack_element->NLayers,
#endif
    stack_element = stack_element->Next
  )
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        capacities = fill_capacities_die
                     (
#ifdef DEBUG_FILL_CAPACITIES
                       debug,
                       current_layer,
#endif
                       stack_element->Pointer.Die,
                       capacities,
                       stkd->Dimensions,
                       delta_time
                     ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        capacities = fill_capacities_layer
                     (
#ifdef DEBUG_FILL_CAPACITIES
                       debug,
                       current_layer,
#endif
                       stack_element->Pointer.Layer,
                       capacities,
                       stkd->Dimensions,
                       delta_time
                     ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        capacities = fill_capacities_channel
                     (
#ifdef DEBUG_FILL_CAPACITIES
                       debug,
                       current_layer,
#endif
                       stkd->Channel,
                       capacities,
                       stkd->Dimensions,
                       delta_time
                     ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */

  } /* stack_element */

#ifdef DEBUG_FILL_CAPACITIES
  fclose(debug) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_sources_stack_description
(
  StackDescription *stkd,
  double *sources
)
{
  StackElement *stack_element ;

#ifdef DEBUG_FILL_SOURCES
  int current_layer ;

  FILE *debug = fopen("fill_sources_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_sources_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p fill_sources_stack_description ( l %d r %d c %d )\n",
    sources,
    stkd->Dimensions->Grid.NLayers,
    stkd->Dimensions->Grid.NRows,
    stkd->Dimensions->Grid.NColumns) ;
#endif

  for
  (
#ifdef DEBUG_FILL_SOURCES
    current_layer = 0 ,
#endif
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

#ifdef DEBUG_FILL_SOURCES
    current_layer += stack_element->NLayers,
#endif
    stack_element = stack_element->Next
  )
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        sources = fill_sources_die
                  (
#ifdef DEBUG_FILL_SOURCES
                    debug,
                    current_layer,
#endif
                    stack_element->Pointer.Die,
                    stack_element->Floorplan,
                    sources,
                    stkd->Dimensions
                  ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        sources = fill_sources_empty_layer
                  (
#ifdef DEBUG_FILL_SOURCES
                    debug,
                    current_layer,
                    stack_element->Pointer.Layer,
#endif
                    sources,
                    stkd->Dimensions
                  ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        sources = fill_sources_channel
                  (
#ifdef DEBUG_FILL_SOURCES
                    debug,
                    current_layer,
#endif
                    stkd->Channel,
                    sources,
                    stkd->Dimensions
                  ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */

  } /* stack_element */

#ifdef DEBUG_FILL_SOURCES
  fclose(debug) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_system_matrix_stack_description
(
  StackDescription *stkd,
  Resistances      *resistances,
  double           *capacities,
  int              *columns,
  int              *rows,
  double           *values
)
{
  StackElement *stack_element ;
  int current_layer, added, area ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE *debug = fopen("fill_system_matrix_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_system_matrix_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p %p %p %p %p fill_system_matrix_stack_description ( l %d r %d c %d )\n",
    resistances, capacities, columns, rows, values,
    stkd->Dimensions->Grid.NLayers,
    stkd->Dimensions->Grid.NRows,
    stkd->Dimensions->Grid.NColumns) ;
#endif

  *columns++ = 0 ;

  for
  (
    current_layer = 0,
    added         = 0,
    area
      = stkd->Dimensions->Grid.NRows * stkd->Dimensions->Grid.NColumns,
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

    current_layer +=        stack_element->NLayers,
    resistances   += area * stack_element->NLayers,
    capacities    += area * stack_element->NLayers,
    columns       += area * stack_element->NLayers,
    rows          += added,
    values        += added,
    stack_element  = stack_element->Next
  )
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        added = fill_system_matrix_die (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                  debug,
#endif
                  stack_element->Pointer.Die, stkd->Dimensions,
                  resistances, capacities,
                  columns, rows, values,
                  current_layer) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        added = fill_system_matrix_layer (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                  debug, stack_element->Pointer.Layer,
#endif
                  stkd->Dimensions, resistances, capacities,
                  columns, rows, values,
                  current_layer) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        added = fill_system_matrix_channel (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                  debug, stkd->Channel,
#endif
                  stkd->Dimensions, resistances, capacities,
                  columns, rows, values,
                  current_layer) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */

  } /* stack_element */

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fclose (debug) ;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_total_number_of_floorplan_elements
(
  StackDescription *stkd
)
{
  int          total   = 0 ;
  StackElement *stk_el = stkd->StackElementsList;

  for ( ; stk_el != NULL ; stk_el = stk_el->Next)
  {
    if (stk_el->Type == TL_STACK_ELEMENT_DIE && stk_el->Floorplan != NULL)
    {
      total += stk_el->Floorplan->NElements ;
    }
  }

  return total ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_number_of_floorplan_elements_in_floorplan
(
  StackDescription *stkd,
  int stack_element_id
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TL_STACK_ELEMENT_DIE || stk_el->Floorplan == NULL)

    return -2 ;

  return stk_el->Floorplan->NElements ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
insert_power_values
(
  StackDescription *stkd,
  double *power_values
)
{
  StackElement *stk_el = stkd->StackElementsList;

  for ( ; stk_el != NULL ; stk_el = stk_el->Next)
  {
    if (stk_el->Type == TL_STACK_ELEMENT_DIE && stk_el->Floorplan != NULL)
    {
      insert_power_values_floorplan (stk_el->Floorplan, power_values) ;
      power_values += stk_el->Floorplan->NElements ;
    }
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
insert_power_values_in_floorplan
(
  StackDescription *stkd,
  int stack_element_id,
  double *power_values
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TL_STACK_ELEMENT_DIE || stk_el->Floorplan == NULL)

    return -2 ;

  insert_power_values_floorplan (stk_el->Floorplan, power_values) ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
insert_power_value_in_floorplan_element
(
  StackDescription *stkd,
  int stack_element_id,
  char *floorplan_element_id,
  double power_value
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TL_STACK_ELEMENT_DIE || stk_el->Floorplan == NULL)

    return -2 ;

  return insert_power_value_floorplan_element
         (
           stk_el->Floorplan,
           floorplan_element_id,
           power_value
         ) ;
}
