/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
 ******************************************************************************/

#include <stdlib.h>

#include "stack_description.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd  ;
  ThermalData      tdata ;

  // Checks if there is the argument file to process

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Init StackDescription

  init_stack_description (&stkd) ;

  // Fill StackDescription using the argument file

  if (fill_stack_description (&stkd, argv[1]) != 0)

    return EXIT_FAILURE ;

  print_dimensions (stdout, "dim: ", stkd.Dimensions) ;

  // Init thermal data

  init_thermal_data (&tdata, &stkd, 300.00, 0.002, 0.020) ;

  // Fill thermal data using the stack description data

  if (fill_thermal_data  (&tdata, &stkd) != 0)
  {
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Alloc memory to store 1 temperature value for each floorplan element
  // in the floorplan on die "die2"

  Quantity_t counter;
  Quantity_t nfloorplanelements
    = get_number_of_floorplan_elements_of_floorplan (&stkd, "die2") ;

  if (nfloorplanelements < 0)
  {
    printf ("no element die2 found\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
  }

  Temperature_t* max_results
    = malloc (sizeof(Temperature_t) * nfloorplanelements) ;

  if (max_results == NULL)
  {
    printf ("alloc memory failed\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
  }

  // Consume all time slots printing time by time the max temperature
  // for every floorplan element composing die "die2" and the temperature
  // of the liquid leaving the channel layer at the first channel.

  Temperature_t outlet;

  do {

    // get max temperatures

    if ( get_all_max_temperatures_of_floorplan
         (
           &stkd, "die2",
           tdata.Temperatures, max_results
         ) != 0 )
    {
      printf ("error getting max temps\n") ;
      break ;
    }

    // get outlet temperature

    if ( get_temperature_of_channel_outlet
         (
           &stkd, "channel1", 0,
           tdata.Temperatures, &outlet
         ) != 0 )
    {
      printf ("error getting oultet temp\n") ;
      break ;
    }


    // Print results

    printf("%5.3fs  ", get_current_time(&tdata)) ;
    for (counter = 0; counter < nfloorplanelements; counter++)
      printf("%7.3f  ", max_results[counter]) ;
    printf("  %7.3f\n", outlet) ;

  } while (emulate_time_slot (&tdata, &stkd) == 0 ) ;

  // At the end of emulation, creates the thermal map of the channel layer

  if ( print_thermal_map
       (
         &stkd, tdata.Temperatures,
         "channel1", "channel1_map.txt"
       ) != 0 )

      printf ("error chanel thermal map\n") ;

  // free all data

  free (max_results) ;
  free_thermal_data (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
