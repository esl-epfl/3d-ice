/******************************************************************************
 *                                                                            *
 * Source file "sources/hotspot2threedice.c"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "stdio.h"

#include "floorplan.h"

#include "../bison/hotspot_ptrace_parser.h"
#include "../flex/hotspot_ptrace_scanner.h"
#include "../bison/hotspot_flp_parser.h"
#include "../flex/hotspot_flp_scanner.h"

extern int hotspot_flp_parse
(
  Floorplan* floorplan,
  yyscan_t   scanner
) ;

extern int hotspot_ptrace_parse
(
  Floorplan* floorplan,
  yyscan_t scanner
) ;

int main (int argc, char** argv)
{
  FILE* file ;
  yyscan_t scanner ;

  Floorplan flp_floorplan ; //, ptrace_floorplan ;
  init_floorplan (&flp_floorplan) ;

  if (argc != 3)
  {
    fprintf (stderr, "Error! use %s file.flp file.ptrace\n", argv[0]) ;
    return 1 ;
  }

  /* Open flp file */

  file = fopen (argv[1], "r") ;
  if (file == NULL)
  {
    perror(argv[1]) ;
    return 1 ;
  }

  flp_floorplan.FileName = strdup (argv[1]) ;

  /* parse flp file */

  hotspot_ptrace_lex_init (&scanner) ;
  hotspot_ptrace_set_in   (file, scanner) ;

  if (hotspot_flp_parse (&flp_floorplan, scanner))
  {
    fprintf(stderr, "parsing error\n") ;
    return 1 ;
  }
  hotspot_ptrace_lex_destroy (scanner) ;
  fclose (file) ;

//   /* Open ptrace file */
//
//   file = fopen (argv[2], "r") ;
//   if (file == NULL)
//   {
//     perror (argv[2]) ;
//     free_floorplan (&flp_floorplan) ;
//     return 1 ;
//   }
//
//   /* parse ptrace file */
//
//   hotspot_ptrace_lex_init (&scanner) ;
//   hotspot_ptrace_set_in (file, scanner) ;
//
//   if (hotspot_ptrace_parse (&ptrace_floorplan, scanner))
//   {
//     fprintf(stderr, "parsing error\n") ;
//     free_floorplan (&flp_floorplan) ;
//     return 1 ;
//   }
//   hotspot_ptrace_lex_destroy (scanner) ;
//   fclose (file) ;
//
//
//   free_floorplan (&flp_floorplan) ;
//   free_floorplan (&ptrace_floorplan) ;

  free_floorplan (&flp_floorplan) ;
  return 0 ;
}
