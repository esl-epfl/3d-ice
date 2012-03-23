/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv)
{
    FILE *totest, *reference ;

    fpos_t start_totest, start_reference ;

    char *line ;

    size_t  len ;
    ssize_t read ;

    int counter, counter1, counter2, max_ref ;

    int row1, column1, row2, column2, max_row, max_column ;

    double value1, value2, tmp, max_difference ;

    // Checks if there are the all the arguments
    ////////////////////////////////////////////////////////////////////////////

    if (argc != 3)
    {
        fprintf(stdout, "Usage: \"%s totest.txt reference.txt\"\n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    // Open the two files
    ////////////////////////////////////////////////////////////////////////////

    totest = fopen (argv[1], "r") ;

    if (totest == NULL)
    {
        fprintf (stdout, "Unable to open file %s\n", argv[1]) ;

        return EXIT_FAILURE ;
    }

    fgetpos (totest, &start_totest) ;

    reference = fopen (argv[2], "r") ;

    if (reference == NULL)
    {
        fprintf (stdout, "Unable to open file %s\n", argv[2]) ;

        fclose (totest) ;

        return EXIT_FAILURE ;
    }

    fgetpos (reference, &start_reference) ;

    // Count the number of lines in file totest
    ////////////////////////////////////////////////////////////////////////////

    for ( line = NULL, len  = 0, counter1 = 0 ;

          (read = getline(&line, &len, totest)) != -1 ;

          counter1 ++ ) ;

    free (line) ;

    fsetpos (totest, &start_totest) ;

    // fprintf (stdout, "File %s contains %d lines\n", argv[1], counter1) ;

    // Count the number of lines in file reference
    ////////////////////////////////////////////////////////////////////////////

    for ( line = NULL, len  = 0, counter2 = 0 ;

          (read = getline(&line, &len, reference)) != -1 ;

          counter2 ++ ) ;

    free (line) ;

    fsetpos (reference, &start_reference) ;

    // fprintf (stdout, "File %s contains %d lines\n", argv[2], counter2) ;

    // Check if the lengths are the same
    ////////////////////////////////////////////////////////////////////////////

    if (counter1 != counter2)
    {
        fprintf (stdout, "Files %s:%d and %s:%d have different # lines\n",
            argv[1], counter1, argv[2], counter2) ;

        goto error ;
    }

    // Compare the content
    ////////////////////////////////////////////////////////////////////////////

    max_difference = 0.0 ;

    for (counter = 0 ; counter != counter1 ; counter++)
    {
        if (fscanf (totest, "%d %d %lf\n", &row1, &column1, &value1) != 3)
        {
            fprintf (stdout, "Error reading line %d from %s\n", counter + 1, argv[1]) ;

            break ;
        }

        if (fscanf (reference, "%d %d %lf\n", &row2, &column2, &value2) != 3)
        {
            fprintf (stdout, "Error reading line %d from %s\n", counter + 1, argv[2]) ;

            break ;
        }

        if (row1 != row2 || column1 != column2)
        {
            fprintf (stdout, "Coordinate mismatch at line %d\n", counter + 1) ;

            break ;
        }

        if (   (value1 < 0.0 && value2 > 0.0)
            || (value1 > 0.0 && value2 < 0.0))
        {
            fprintf (stdout, "Sign mismatch at line %d\n", counter + 1) ;

            break ;
        }

        tmp = fabs (value1 - value2) ;

        if (tmp > max_difference)
        {
            max_difference = tmp ;
            max_ref        = counter ;
            max_row        = row1 ;
            max_column     = column1 ;
        }
    }

    if (counter == counter1)

        fprintf (stdout, "%.3e\t@%5d (%d,%d)\n",
            max_difference, max_ref + 1, max_row, max_column) ;

    // Closes ...
    ////////////////////////////////////////////////////////////////////////////

    fclose (totest) ;
    fclose (reference) ;

    return EXIT_SUCCESS ;

error :

    fclose (totest) ;
    fclose (reference) ;

    return EXIT_FAILURE ;

}
