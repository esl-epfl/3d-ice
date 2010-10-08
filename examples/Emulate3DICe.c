/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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

#include <stdlib.h>
#include <time.h>

#include "stack_description.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd  ;
  ThermalData      tdata ;

  // Checks if there are the all the arguments
  ////////////////////////////////////////////////////////////////////////////

  if (argc != 4)
  {
    fprintf(stderr, "Usage: \"%s file.stk slot_time step_time\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Init StackDescription and parse the input file
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing stk data ...\n");

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)
  {
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Init thermal data and fill it using the StackDescription
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing thermal data ...\n");

  init_thermal_data (&tdata, 300.00, atof(argv[2]), atof(argv[3])) ;

  if (fill_thermal_data (&tdata, &stkd) != 0)
  {
    fprintf(stderr, "fill thermal data failed\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Alloc memory to store 1 temperature value for each floorplan element
  // in the floorplan on die "die2"
  ////////////////////////////////////////////////////////////////////////////

  Quantity_t counter;
  Quantity_t nfloorplanelements
    = get_number_of_floorplan_elements (&stkd, "die2") ;

  if (nfloorplanelements < 0)
  {
    printf ("no element die2 found\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  Temperature_t* max_results
    = malloc (sizeof(Temperature_t) * nfloorplanelements) ;

  if (max_results == NULL)
  {
    printf ("alloc memory failed\n") ;
    free_thermal_data      (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Consume all time slots printing time by time the max temperature
  // for every floorplan element composing die "die2" and the temperature
  // of the liquid leaving the channel layer at the first channel.
  // The do { } while () ; loop prints the temperatures at all the time points.
  ////////////////////////////////////////////////////////////////////////////

  Temperature_t outlet, cell1, cell2 ;
  clock_t Time = clock();

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

    // get outlet temperature

    if ( get_temperature_of_channel_outlet (&stkd, &tdata,
                                            "channel1", 0, &outlet)
         != 0 )
    {
      printf ("error getting oultet temp\n") ;
      break ;
    }

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

    // Print results

    printf("%5.3fs  ", get_current_time(&tdata)) ;
    for (counter = 0; counter < nfloorplanelements; counter++)
      printf("%7.3f  ", max_results[counter]) ;
    printf("%7.3f  %7.3f  %7.3f\n", cell1, cell2, outlet) ;
  }
  while (emulate_step (&tdata, &stkd) != 1) ;
  // while (emulate_slot (&tdata, &stkd) != 1) ;

  fprintf (stdout, "emulation took %.3f sec\n",
           ( (double)clock() - Time ) / CLOCKS_PER_SEC );

  // At the end of emulation, creates the thermal map of the channel layer
  ////////////////////////////////////////////////////////////////////////////

  if ( print_thermal_map (&stkd, &tdata, "channel1", "channel1_map.txt") != 0 )

      printf ("error chanel thermal map\n") ;

  // free all data
  ////////////////////////////////////////////////////////////////////////////

  free                   (max_results) ;
  free_thermal_data      (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
