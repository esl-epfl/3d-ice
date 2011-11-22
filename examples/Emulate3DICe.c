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
#include "thermal_data.h"
#include "analysis.h"

int
main(int argc, char** argv)
{
  StackDescription stkd ;
  Analysis         analysis ;
  ThermalData      tdata ;

  // Checks if there are the all the arguments
  ////////////////////////////////////////////////////////////////////////////

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]) ;
    return EXIT_FAILURE ;
  }

  // Init StackDescription and parse the input file
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing stk data ...\n") ;

  init_stack_description (&stkd) ;

  init_analysis (&analysis) ;

  if (fill_stack_description (&stkd, &analysis, argv[1]) != 0)

    return EXIT_FAILURE ;

  // Generate output files
  ////////////////////////////////////////////////////////////////////////////

  if (initialize_print_output_instructions (&analysis, stkd.BottomStackElement) != TDICE_SUCCESS)
  {
    fprintf(stderr, "error in initializing output files \n ");
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Init thermal data and fill it using the StackDescription
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing thermal data ...\n") ;

  init_thermal_data (&tdata) ;

  if (fill_thermal_data (&tdata, &stkd, &analysis) != 0)
  {
    fprintf(stderr, "fill thermal data failed\n") ;
    free_thermal_data (&tdata) ;
    free_analysis (&analysis) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Alloc memory to store 1 temperature value for each floorplan element
  // in the floorplan on die "die2"
  ////////////////////////////////////////////////////////////////////////////

  uint8_t counter ;
  uint32_t nfloorplanelements
    = get_number_of_floorplan_elements (&stkd, "die2") ;

  if (nfloorplanelements < 0)
  {
    printf ("no element die2 found\n") ;
    free_thermal_data (&tdata) ;
    free_analysis (&analysis) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  double* max_results
    = malloc (sizeof(double) * nfloorplanelements) ;

  if (max_results == NULL)
  {
    printf ("alloc memory failed\n") ;
    free_thermal_data      (&tdata) ;
    free_analysis (&analysis) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Consume all time slots printing time by time the max temperature
  // for every floorplan element composing die "die2" and the temperature
  // of the liquid leaving the channel layer at the first channel.
  // The do { } while () ; loop prints the temperatures at all the time points.
  ////////////////////////////////////////////////////////////////////////////

  double cell1, cell2 ;
  clock_t Time = clock() ;

  do
  {
    // get max temperatures

    if ( get_all_max_temperatures_of_floorplan (&stkd, &tdata,
                                                "die2", max_results)
         != 0 )
    {
      printf ("error getting max temps\n") ;
      break ;
    }

/*
    if ( get_cell_temperature (&stkd, &tdata, 2, 11, 4, &cell1) != 0 )
    {
      printf ("error getting cell1 temp\n") ;
      break ;
    }

    if ( get_cell_temperature (&stkd, &tdata, 6, 34, 84, &cell2) != 0 )
    {
      printf ("error getting cell2 temp\n") ;
      break ;
    }
*/
    // Print results

    printf("%5.3fs  ", get_simulated_time(&analysis)) ;
    for (counter = 0 ; counter < nfloorplanelements ; counter++)
      printf("%7.3f  ", max_results[counter]) ;
    /*printf("%7.3f  %7.3f  %7.3f\n", cell1, cell2, outlet) ;*/
    printf("\n") ;
  }
  while (emulate_step (&tdata, &stkd, &analysis) != 1) ;
  //while (emulate_slot (&tdata, &stkd, &analysis) != 1) ;

  fprintf (stdout, "emulation took %.3f sec\n",
           ( (double)clock() - Time ) / CLOCKS_PER_SEC ) ;

  // At the end of emulation, creates the thermal map of the channel layer
  ////////////////////////////////////////////////////////////////////////////

  if ( print_thermal_map (&stkd, &tdata, "channel1", "channel1_map.txt") != 0 )

      printf ("error chanel thermal map\n") ;

  // free all data
  ////////////////////////////////////////////////////////////////////////////

  free                   (max_results) ;
  free_thermal_data      (&tdata) ;
  free_analysis          (&analysis) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS ;
}
