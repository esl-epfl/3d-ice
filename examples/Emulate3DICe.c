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

#include "stack_description.h"
#include "thermal_data.h"

int
main(int argc, char** argv)
{
  StackDescription stkd  ;
  ThermalData      tdata ;

  int cell1, cell2;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  init_stack_description (&stkd) ;
  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  //print_all_floorplans (stdout, "Before  ", &stkd) ;

  init_thermal_data  (&stkd, &tdata, 300.00, 0.002, 0.020) ;
  fill_thermal_data  (&stkd, &tdata) ;

  //print_dimensions (stdout, "dim: ",     stkd.Dimensions) ;
  //print_channel    (stdout, "channel: ", stkd.Channel) ;

  cell1 = get_cell_offset_in_stack (stkd.Dimensions, 0, 50, 1) ;
  cell2 = get_cell_offset_in_stack (stkd.Dimensions, 4, 50, 1) ;

  do {
    printf ("%8.4f -- %8.4f\n", tdata.Temperatures[cell1],
                                tdata.Temperatures[cell2]) ;
  } while (emulate_time_slot (&stkd, &tdata) == 0 ) ;

//  while (emulate_time_slot (&stkd, &tdata) == 0 ) ;

//  printf ("%d: %8.4f -- %d:%8.4f\n", cell1, tdata.Temperatures[cell1],
//                                     cell2, tdata.Temperatures[cell2]) ;

//  int count ;
//        for (count = 0; count < tdata.Size ; count ++)
//    fprintf(stdout, "%.6e\n", tdata.Temperatures[count]);

//  for (count = 0; count < tdata.SM_A.NNz ; count ++)
//    fprintf(stdout, "%.6e\n", tdata.SM_A.Values[count]);

  //print_all_floorplans (stdout, "After  ", &stkd) ;

  free_thermal_data (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
