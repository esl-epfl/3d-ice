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

  init_stack_description (&stkd) ;

  init_analysis (&analysis) ;

  if (fill_stack_description (&stkd, &analysis, argv[1]) != 0)

    return EXIT_FAILURE ;

  fprintf (stdout, "----------------------------------------------\n");
  print_detailed_stack_description (stdout, "", &stkd) ;
  fprintf (stdout, "----------------------------------------------\n");
  print_formatted_stack_description (stdout, "", &stkd) ;
  fprintf (stdout, "----------------------------------------------\n");

  // Init thermal data and fill it using the StackDescription
  ////////////////////////////////////////////////////////////////////////////

  init_thermal_data (&tdata) ;

  if (fill_thermal_data  (&tdata, &stkd, &analysis) != 0)
  {
    fprintf (stderr, "fill thermal data failed\n") ;
    free_thermal_data (&tdata) ;
    free_stack_description (&stkd) ;
    return EXIT_FAILURE ;
  }

  // Here we can do something ... emulating? printing info ??
  ////////////////////////////////////////////////////////////////////////////

  // print_all_floorplans(stdout, "", &stkd) ;

  // This is to consume a power value (to see the effect of PRINT_SOURCES
  emulate_step(&tdata, &stkd, &analysis) ;

  // free all data
  ////////////////////////////////////////////////////////////////////////////

  free_thermal_data (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS ;
}
