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

#include <stdlib.h>
#include <stdio.h>

#include "stack_description.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd ;
  ThermalData      tdata ;
  Quantity_t       counter = QUANTITY_I ;

  // Checks if there are the all the arguments
  ////////////////////////////////////////////////////////////////////////////

  if (argc != 4)
  {
    fprintf(stderr, "Usage: \"%s file.stk slot_time step_time\"\n", argv[0]) ;
    return EXIT_FAILURE ;
  }

  FILE* t_output = fopen ("3DICE-Temperatures.txt", "w") ;
  if (t_output == NULL)
  {
    fprintf(stderr, "Error opening temperatures file\n") ;
    return EXIT_FAILURE ;
  }

  FILE* p_output = fopen ("3DICE-PowerSources.txt", "w") ;
  if (p_output == NULL)
  {
    fprintf(stderr, "Error opening power sources file\n") ;
    fclose (t_output) ;
    return EXIT_FAILURE ;
  }

  // Init StackDescription and parse the input file
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing stk data ...\n") ;

  init_stack_description (&stkd) ;

  if (fill_stack_description (&stkd, argv[1]) != 0)

    return EXIT_FAILURE ;

  print_detailed_dimensions (stdout, "", stkd.Dimensions);

  // Init thermal data and fill it using the StackDescription
  ////////////////////////////////////////////////////////////////////////////

  fprintf (stdout, "Preparing thermal data ...\n") ;

  init_thermal_data (&tdata, 300.00, atof(argv[2]), atof(argv[3])) ;

  if (fill_thermal_data (&tdata, &stkd) != 0)
  {
    fprintf(stderr, "fill thermal data failed\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  do
  {
    for (counter = QUANTITY_I ; counter < tdata.Size ; counter++)
    {
      fprintf(t_output, "%.5f  ", tdata.Temperatures[counter]) ;
      fprintf(p_output, "%.5e  ", tdata.Sources[counter]) ;
    }

    fprintf(t_output, "\n") ;
    fprintf(p_output, "\n") ;
  }
  while (emulate_step (&tdata, &stkd) != 1) ;

  fclose(t_output) ;
  fclose(p_output) ;

  // free all data
  ////////////////////////////////////////////////////////////////////////////

  free_thermal_data      (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS ;
}
