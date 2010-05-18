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
#include "layer.h"

extern int  stack_description_parse (StackDescription *stkd, yyscan_t scanner) ;
static int  fill_floorplans         (StackDescription *stkd) ;
static void align_layers_in_die     (StackDescription *stkd) ;
static void align_stack_elements    (StackDescription *stkd) ;


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_stack_description
(
  StackDescription *stkd
)
{
  stkd->FileName           = NULL ;
  stkd->MaterialsList      = NULL ;
  stkd->Channel            = NULL ;
  stkd->DiesList           = NULL ;
  stkd->StackElementsList  = NULL ;
  stkd->Dimensions         = NULL ;
  stkd->PowerValuesChanged = 0    ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_stack_description
(
  StackDescription* stkd ,
  char *filename
)
{
  FILE     *input;
  int      result;
  yyscan_t scanner;

  input = fopen (filename, "r");
  if(input == NULL)
  {
    perror(filename);
    return 1;
  }

  stkd->FileName = strdup (filename);

  stack_description_lex_init (&scanner);

  stack_description_set_in (input, scanner);

  result = stack_description_parse (stkd, scanner);

  stack_description_lex_destroy (scanner);

  fclose (input);

  if (result == 1) return result ;

  align_stack_elements (stkd) ;
  align_layers_in_die  (stkd) ;

  return fill_floorplans (stkd) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_stack_description
(
  StackDescription *stkd
)
{
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
print_stack_description
(
  FILE             *stream,
  char             *prefix,
  StackDescription *stkd
)
{
  fprintf(stream, "%sStack read from file %s\n", prefix, stkd->FileName) ;

  print_materials_list      (stream, prefix, stkd->MaterialsList) ;
  print_channel             (stream, prefix, stkd->Channel);
  print_dies_list           (stream, prefix, stkd->DiesList) ;

  fprintf(stream, "%sStack:\n", prefix) ;

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

    if (stack_element->Type == TL_STACK_ELEMENT_DIE)

      result += fill_floorplan(stack_element->Floorplan, stkd->Dimensions) ;

  return result ;
 }

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
align_layers_in_die
(
  StackDescription *stkd
)
{
  Layer *layer ;
  int layer_offset ;

  Die *die = stkd->DiesList ;
  for ( ; die != NULL ; die = die->Next)
  {
    layer_offset = 0 ;
    layer = die->LayersList ;
    for ( ; layer != NULL ; layer = layer->Next)
    {
      layer->LayersOffset = layer_offset++ ;
    }
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
align_stack_elements (StackDescription *stkd)
{
  int layer_counter = 0 ;
  StackElement *stack_element = stkd->StackElementsList ;

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    stack_element->LayersOffset = layer_counter ;

    layer_counter += stack_element->NLayers ;
  }

 }

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_conductances_stack_description
(
  StackDescription *stkd,
  Conductances     *conductances
)
{
  StackElement *stack_element ;

#ifdef DEBUG_FILL_CONDUCTANCES
  FILE *debug = fopen("fill_conductances_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_conductances_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p fill_conductances_stack_description ( l %d r %d c %d )\n",
    conductances,
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions));
#endif

  for
  (
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

    stack_element = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        conductances = fill_conductances_die
                       (
#ifdef DEBUG_FILL_CONDUCTANCES
                         debug,
#endif
                         stack_element->Pointer.Die,
                         conductances, stkd->Dimensions,
                         stack_element->LayersOffset
                       ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        conductances = fill_conductances_layer
                       (
#ifdef DEBUG_FILL_CONDUCTANCES
                         debug,
#endif
                         stack_element->Pointer.Layer,
                         conductances, stkd->Dimensions,
                         stack_element->LayersOffset
                       ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        conductances = fill_conductances_channel
                       (
#ifdef DEBUG_FILL_CONDUCTANCES
                         debug,
#endif
                         stkd->Channel,
                         conductances, stkd->Dimensions,
                         stack_element->LayersOffset
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

#ifdef DEBUG_FILL_CONDUCTANCES
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
  double           *capacities,
  double           delta_time
)
{
  StackElement *stack_element ;

#ifdef DEBUG_FILL_CAPACITIES
  FILE *debug = fopen("fill_capacities_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_capacities_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p fill_capacities_stack_description ( l %d r %d c %d )\n",
    capacities,
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions));
#endif

  for
  (
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

    stack_element = stack_element->Next
  )
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        capacities = fill_capacities_die
                     (
#ifdef DEBUG_FILL_CAPACITIES
                       debug,
                       stack_element->LayersOffset,
#endif
                       stack_element->Pointer.Die,
                       capacities, stkd->Dimensions, delta_time) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        capacities = fill_capacities_layer
                     (
#ifdef DEBUG_FILL_CAPACITIES
                       debug,
                       stack_element->LayersOffset,
#endif
                       stack_element->Pointer.Layer,
                       capacities, stkd->Dimensions, delta_time) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        capacities = fill_capacities_channel
                     (
#ifdef DEBUG_FILL_CAPACITIES
                       debug,
                       stack_element->LayersOffset,
#endif
                       stkd->Channel,
                       capacities, stkd->Dimensions, delta_time) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */

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
  double           *sources
)
{
  StackElement *stack_element ;

#ifdef DEBUG_FILL_SOURCES
  FILE *debug = fopen("fill_sources_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_sources_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p fill_sources_stack_description ( l %d r %d c %d )\n",
    sources,
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions));
#endif

  for
  (
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

    stack_element = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        sources = fill_sources_die
                  (
#ifdef DEBUG_FILL_SOURCES
                    debug,
                    stack_element->LayersOffset,
#endif
                    stack_element->Pointer.Die,
                    stack_element->Floorplan,
                    sources, stkd->Dimensions) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        sources = fill_sources_empty_layer
                  (
#ifdef DEBUG_FILL_SOURCES
                    debug,
                    stack_element->LayersOffset,
                    stack_element->Pointer.Layer,
#endif
                    sources, stkd->Dimensions) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        sources = fill_sources_channel
                  (
#ifdef DEBUG_FILL_SOURCES
                    debug,
                    stack_element->LayersOffset,
#endif
                    stkd->Channel,
                    sources, stkd->Dimensions) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */

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
  Conductances     *conductances,
  double           *capacities,
  int              *columns,
  int              *rows,
  double           *values
)
{
  StackElement *stack_element ;
  int added, area ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE *debug = fopen("fill_system_matrix_stack_description.txt", "w") ;
  if (debug == NULL)
  {
    perror("fill_system_matrix_stack_description.txt") ;
    return ;
  }
  fprintf (debug,
    "%p %p %p %p %p fill_system_matrix_stack_description ( l %d r %d c %d )\n",
    conductances, capacities, columns, rows, values,
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions));
#endif

  *columns++ = 0 ;

  for
  (
    added         = 0,
    area          = get_layer_area (stkd->Dimensions),
    stack_element = stkd->StackElementsList ;

    stack_element != NULL ;

    conductances  += area * stack_element->NLayers,
    capacities    += area * stack_element->NLayers,
    columns       += area * stack_element->NLayers,
    rows          += added,
    values        += added,
    stack_element  = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        added = fill_system_matrix_die (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                  debug,
#endif
                  stack_element->Pointer.Die, stkd->Dimensions,
                  conductances, capacities,
                  columns, rows, values,
                  stack_element->LayersOffset) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        added = fill_system_matrix_layer (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                  debug, stack_element->Pointer.Layer,
#endif
                  stkd->Dimensions, conductances, capacities,
                  columns, rows, values,
                  stack_element->LayersOffset) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        added = fill_system_matrix_channel (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                  debug, stkd->Channel,
#endif
                  stkd->Dimensions, conductances, capacities,
                  columns, rows, values,
                  stack_element->LayersOffset) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */

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

    if (stk_el->Type == TL_STACK_ELEMENT_DIE && stk_el->Floorplan != NULL)

      total += stk_el->Floorplan->NElements ;

  return total ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_number_of_floorplan_elements_in_floorplan
(
  StackDescription *stkd,
  char             *stack_element_id
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
change_coolant_flow_rate
(
  StackDescription *stkd,
  double flow_rate
)
{
  stkd->Channel->FlowRate = ( flow_rate * 1e+12 ) / 60.0 ;
  stkd->Channel->FlowRateChanged = 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
insert_all_power_values
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

  stkd->PowerValuesChanged = 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
insert_power_values_in_floorplan
(
  StackDescription *stkd,
  char             *stack_element_id,
  double           *power_values
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  insert_power_values_floorplan (stk_el->Floorplan, power_values) ;

  stkd->PowerValuesChanged = 1 ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
insert_power_value_in_floorplan_element
(
  StackDescription *stkd,
  char             *stack_element_id,
  char             *floorplan_element_id,
  double           power_value
)
{
  int result ;

  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  result = insert_power_value_floorplan_element
           (
             stk_el->Floorplan,
             floorplan_element_id,
             power_value
           ) ;

  if (result == 0 ) stkd->PowerValuesChanged = 1 ;

  return result ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_max_temperature_in_floorplan_element
(
  StackDescription *stkd,
  char             *stack_element_id,
  char             *floorplan_element_id,
  double           *temperatures,
  double           *max_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  return get_max_temperature_floorplan
         (
           stk_el->Floorplan, floorplan_element_id,
           stkd->Dimensions,
           temperatures
           + ((stk_el->LayersOffset
               + stk_el->Pointer.Die->SourceLayer->LayersOffset)
              * get_layer_area (stkd->Dimensions)),
           max_temperature
         );
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_min_temperature_in_floorplan_element
(
  StackDescription *stkd,
  char             *stack_element_id,
  char             *floorplan_element_id,
  double           *temperatures,
  double           *min_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  return get_min_temperature_floorplan
         (
           stk_el->Floorplan, floorplan_element_id,
           stkd->Dimensions,
           temperatures
           + ((stk_el->LayersOffset
               + stk_el->Pointer.Die->SourceLayer->LayersOffset)
              * get_layer_area (stkd->Dimensions)),
           min_temperature
         );
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_avg_temperature_in_floorplan_element
(
  StackDescription *stkd,
  char             *stack_element_id,
  char             *floorplan_element_id,
  double           *temperatures,
  double           *avg_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  return get_avg_temperature_floorplan
         (
           stk_el->Floorplan, floorplan_element_id,
           stkd->Dimensions,
           temperatures
           + ((stk_el->LayersOffset
               + stk_el->Pointer.Die->SourceLayer->LayersOffset)
              * get_layer_area (stkd->Dimensions)),
           avg_temperature
         );
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_min_avg_max_temperatures_in_floorplan_element
(
  StackDescription *stkd,
  char             *stack_element_id,
  char             *floorplan_element_id,
  double           *temperatures,
  double           *min_temperature,
  double           *avg_temperature,
  double           *max_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  return get_min_avg_max_temperatures_floorplan
         (
           stk_el->Floorplan, floorplan_element_id,
           stkd->Dimensions,
           temperatures
           + ((stk_el->LayersOffset
               + stk_el->Pointer.Die->SourceLayer->LayersOffset)
              * get_layer_area (stkd->Dimensions)),
           min_temperature, avg_temperature, max_temperature
         );
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_max_temperatures_in_floorplan
(
  StackDescription *stkd,
  char             *stack_element_id,
  double           *temperatures,
  double           *max_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  get_all_max_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    temperatures
    + ((stk_el->LayersOffset
        + stk_el->Pointer.Die->SourceLayer->LayersOffset)
       * get_layer_area (stkd->Dimensions)),
    max_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_min_temperature_in_floorplan
(
  StackDescription *stkd,
  char             *stack_element_id,
  double           *temperatures,
  double           *min_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  get_all_min_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    temperatures
    + ((stk_el->LayersOffset
        + stk_el->Pointer.Die->SourceLayer->LayersOffset)
       * get_layer_area (stkd->Dimensions)),
    min_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_avg_temperatures_in_floorplan
(
  StackDescription *stkd,
  char             *stack_element_id,
  double           *temperatures,
  double           *avg_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  get_all_avg_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    temperatures
    + ((stk_el->LayersOffset
        + stk_el->Pointer.Die->SourceLayer->LayersOffset)
       * get_layer_area (stkd->Dimensions)),
    avg_temperature
  );

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_all_min_avg_max_temperatures_in_floorplan
(
  StackDescription *stkd,
  char             *stack_element_id,
  double           *temperatures,
  double           *min_temperature,
  double           *avg_temperature,
  double           *max_temperature
)
{
  StackElement *stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  get_all_min_avg_max_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    temperatures
    + ((stk_el->LayersOffset
        + stk_el->Pointer.Die->SourceLayer->LayersOffset)
       * get_layer_area (stkd->Dimensions)),
    min_temperature, avg_temperature, max_temperature
  );

 return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
