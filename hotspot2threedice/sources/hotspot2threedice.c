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
#include "powers_queue.h"

#include "../bison/hotspot_flp_parser.h"
#include "../flex/hotspot_flp_scanner.h"

extern int hotspot_flp_parse
(
  struct FloorplanElement** flp_list,
  yyscan_t scanner
) ;

extern int hotspot_ptrace_parse
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

  if (hotspot_ptrace_parse (&flp_list, scanner))
    fprintf(stderr, "parsing error\n");

  hotspot_flp_lex_destroy (scanner);
  fclose (input);

  print_floorplan_elements_list (stdout,"-> ", flp_list) ;
  free_floorplan_elements_list(flp_list);

//   PowersQueue* tmp = alloc_and_init_powers_queue();
// 
//   if (tmp == NULL)
//   {
//     fprintf(stderr, "memory error\n");
//     return 1;
//   }
//
//   int counter;
// 
//   printf("begin ...\n" ) ;
//   print_powers_queue(stdout, "", tmp) ;
// 
//   for (counter = 1; counter < 16; counter++)
//   {
//     printf("inserting value %4.1f\n", (Power_t) counter * 2.0 ) ;
// 
//     put_into_powers_queue(tmp, (Power_t) counter * 2.0 );
// 
//     print_powers_queue(stdout, "->", tmp) ;
// 
//     printf("-------------------------------------------\n");
// 
//     if (counter % 3 == 0)
//     {
//       printf("removed value %4.1f\n", get_from_powers_queue(tmp));
//       pop_from_powers_queue(tmp) ;
//       print_powers_queue(stdout, "->", tmp) ;
//       printf("-------------------------------------------\n");
//     }
//   }
// 
//   while (!is_empty_powers_queue(tmp))
//   {
//     printf("removed value %4.1f\n", get_from_powers_queue(tmp));
//     pop_from_powers_queue(tmp) ;
//     print_powers_queue(stdout, "=>", tmp) ;
//   }
// 
//   printf("end ...\n" ) ;
// 
//   free_powers_queue(tmp);

  return 0 ;
}
