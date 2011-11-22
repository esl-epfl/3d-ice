/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <time.h>

#include "stack_description.h"
#include "analysis.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd ;
  Analysis         analysis ;
  ThermalData      tdata ;

  char filename [50] ;
  int  counter = 0 ;

  // Checks if there are the all the arguments
  ////////////////////////////////////////////////////////////////////////////

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]) ;
    return EXIT_FAILURE ;
  }

  // Init StackDescription and parse the input file
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing stk data ... ") ;  fflush (stdout) ;

  init_stack_description (&stkd) ;

  init_analysis (&analysis) ;

  if (fill_stack_description (&stkd, &analysis, argv[1]) != 0)

    return EXIT_FAILURE ;

  fprintf (stdout, "done !\n") ;

  // Init thermal data and fill it using the StackDescription
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing thermal data ... ") ;  fflush (stdout) ;

  init_thermal_data (&tdata) ;

  if (fill_thermal_data (&tdata, &stkd, &analysis) != 0)
  {
    fprintf(stderr, "fill thermal data failed\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  fprintf (stdout, "done !\n") ;

  // Consume all time slots printing time by time the thermal map
  ////////////////////////////////////////////////////////////////////////////

  clock_t Time = clock() ;

  do
  {
     printf("%5.3fs ", get_simulated_time(&analysis)) ; fflush (stdout) ;

     // Print thermal map for die1
     /////////////////////////////////////////////////////////////////////////

     sprintf (filename, "die1_%05d.tmap", counter) ;

     if ( print_thermal_map (&stkd, &tdata, "die2", filename) != 0 )

       printf ("error die1 thermal map\n") ;

     printf("%s ", filename) ; fflush (stdout) ;

     // Print thermal map for channel1
     /////////////////////////////////////////////////////////////////////////

     sprintf (filename, "channel_%05d.tmap", counter) ;

     if ( print_thermal_map (&stkd, &tdata, "channel1", filename) != 0 )

       printf ("error channel thermal map\n") ;

     printf("%s ", filename) ; fflush (stdout) ;

     // Print thermal map for die2
     /////////////////////////////////////////////////////////////////////////

     sprintf (filename, "die2_%05d.tmap", counter) ;

     if ( print_thermal_map (&stkd, &tdata, "die2", filename) != 0 )

       printf ("error die2 thermal map\n") ;

     printf("%s\n", filename) ; fflush (stdout) ;

     /////////////////////////////////////////////////////////////////////////

     counter++ ;

  }  while (emulate_step (&tdata, &stkd, &analysis) != 1) ;


  fprintf (stdout, "emulation took %.3f sec\n",
           ( (double)clock() - Time ) / CLOCKS_PER_SEC ) ;

  // free all data
  ////////////////////////////////////////////////////////////////////////////

  free_thermal_data      (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS ;
}
