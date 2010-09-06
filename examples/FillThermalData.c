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
  StackDescription stkd ;
  ThermalData      tdata ;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  init_stack_description (&stkd) ;
  if (fill_stack_description (&stkd, argv[1]) != 0)
    return EXIT_FAILURE ;

  init_thermal_data  (&stkd, &tdata, 300.00, 0.002, 0.020) ;
  fill_thermal_data  (&stkd, &tdata) ;

//   int row, column, counter;
//   FILE* file;
//
//   file = fopen("RowOffsets.txt", "w") ;
//   for (counter = 0 ; counter < tdata.SM_A.Size + 1; counter++)
//     fprintf(file, "%d\n", tdata.SM_A.RowOffsets[counter]) ;
//   fclose(file);
//
//   file = fopen("ColumnIndices.txt", "w") ;
//   for (counter = 0 ; counter < tdata.SM_A.NNz; counter++)
//     fprintf(file, "%d\n", tdata.SM_A.ColumnIndices[counter]) ;
//   fclose(file);
//
//   file = fopen("Values.txt", "w") ;
//   for (counter = 0 ; counter < tdata.SM_A.NNz; counter++)
//     fprintf(file, "%5e\n", tdata.SM_A.Values[counter]) ;
//   fclose(file);
//
//   file = fopen("Matrix.txt", "w") ;
//
//   for (row = 0 ; row < tdata.SM_A.Size; row++)
//   {
//     counter = tdata.SM_A.RowOffsets[row] ;
//
//     for (column = 0; column < tdata.SM_A.Size ; column++)
//
//       if (column == tdata.SM_A.ColumnIndices[counter])
//       {
//         fprintf (file, "%+.3e ", tdata.SM_A.Values[column]);
//         counter++;
//       }
//       else
//         fprintf (file, "%+.3e ", 0.0);
//
//     fprintf (file,"\n");
//   }
//   fclose(file);

  free_thermal_data      (&tdata) ;
  free_stack_description (&stkd) ;

  return EXIT_SUCCESS;
}
