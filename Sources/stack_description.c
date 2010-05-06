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
