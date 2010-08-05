/******************************************************************************
 *                                                                            *
 * Source file "sources/hotspot2threedice.c"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "stdio.h"

#include "floorplan_element.h"

#include "../bison/hotspot_flp_parser.h"
#include "../flex/hotspot_flp_scanner.h"

extern int hotspot_flp_parse
(
  struct FloorplanElement** flp_list,
  yyscan_t scanner
) ;

int main (int argc, char** argv)
{
  FILE     *input;
  yyscan_t scanner;

  struct FloorplanElement* flp_list = NULL;

  if (argc != 2)
  {
    fprintf (stderr, "Error! use %s hotspotfile.flp\n", argv[0]);
    return 1;
  }

  input = fopen (argv[1], "r");
  if(input == NULL)
  {
    perror(argv[1]);
    return 1;
  }

  hotspot_flp_lex_init (&scanner);
  hotspot_flp_set_in (input, scanner);

  if (hotspot_flp_parse (&flp_list, scanner))
    fprintf(stderr, "parsing error\n");

  hotspot_flp_lex_destroy (scanner);
  fclose (input);

  print_floorplan_elements_list (stdout,"-> ", flp_list) ;
  free_floorplan_elements_list(flp_list);

  return 0 ;
}
