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
    FILE *node1, *node2, *reference ;

    char *line ;

    size_t  len ;
    ssize_t read ;

    fpos_t start_node1, start_node2 ;

    int counter, counter1, counter2, counter3 ;

    double time1, time2, time3, temp1, temp2, temp3, temp4 ;

    double tmp, max_node1, max_node2, time_node1, time_node2 ;

    // Checks if there are the all the arguments
    ////////////////////////////////////////////////////////////////////////////

    if (argc != 4)
    {
        fprintf(stdout, "Usage: \"%s node1.txt node2.txt reference.txt\"\n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    // Open the three files
    ////////////////////////////////////////////////////////////////////////////

    node1 = fopen (argv[1], "r") ;

    if (node1 == NULL)
    {
        fprintf (stdout, "Unable to open file %s\n", argv[1]) ;

        return EXIT_FAILURE ;
    }

    // read three lines

    for (line = NULL, len = 0, counter = 0 ; counter != 3 ; counter++)

        read = getline(&line, &len, node1) ;

    free (line) ;

    fgetpos (node1, &start_node1) ;

    //

    node2 = fopen (argv[2], "r") ;

    if (node2 == NULL)
    {
        fprintf (stdout, "Unable to open file %s\n", argv[2]) ;

        fclose (node1) ;

        return EXIT_FAILURE ;
    }

    // read three lines

    for (line = NULL, len = 0, counter = 0 ; counter != 3 ; counter++)

        read = getline(&line, &len, node2) ;

    free (line) ;

    fgetpos (node2, &start_node2) ;

    //

    reference = fopen (argv[3], "r") ;

    if (reference == NULL)
    {
        fprintf (stdout, "Unable to open file %s\n", argv[3]) ;

        fclose (node1) ;
        fclose (node2) ;

        return EXIT_FAILURE ;
    }

    // Count the number of lines in file node1
    ////////////////////////////////////////////////////////////////////////////

    fsetpos (node1, &start_node1) ;

    for ( line = NULL, len  = 0, counter1 = 0 ;

          (read = getline(&line, &len, node1)) != -1 ;

          counter1 ++ ) ;

    free (line) ;

    fsetpos (node1, &start_node1) ;

    // fprintf (stdout, "File %s contains %d lines\n", argv[1], counter1) ;

    // Count the number of lines in file node2
    ////////////////////////////////////////////////////////////////////////////

    fsetpos (node2, &start_node2) ;

    for ( line = NULL, len  = 0, counter2 = 0 ;

          (read = getline(&line, &len, node2)) != -1 ;

          counter2 ++ ) ;

    free (line) ;

    fsetpos (node2, &start_node2) ;

    // fprintf (stdout, "File %s contains %d lines\n", argv[1], counter1) ;

    // Count the number of lines in file reference
    ////////////////////////////////////////////////////////////////////////////

    for ( line = NULL, len  = 0, counter3 = 0 ;

          (read = getline(&line, &len, reference)) != -1 ;

          counter3 ++ ) ;

    free (line) ;

    rewind (reference) ;

    // fprintf (stdout, "File %s contains %d lines\n", argv[2], counter2) ;

    // Check if the lengths are the same
    ////////////////////////////////////////////////////////////////////////////

    if (counter1 != counter2 || counter2 != counter3)
    {
        fprintf (stdout, "Files %s:%d, %s:%d and %s:%d have different # lines\n",
            argv[1], counter1, argv[2], counter2, argv[3], counter3) ;

        goto error ;
    }

    // Compare the content
    ////////////////////////////////////////////////////////////////////////////

    max_node1 = 0.0 ;
    max_node2 = 0.0 ;

    for (counter = 0 ; counter != counter1 ; counter++)
    {
        if (fscanf (node1, "%lf %lf\n", &time1, &temp1) != 2)
        {
            fprintf (stdout, "Error reading line %d from %s\n", counter + 1, argv[1]) ;

            break ;
        }

        if (fscanf (node2, "%lf %lf\n", &time2, &temp2) != 2)
        {
            fprintf (stdout, "Error reading line %d from %s\n", counter + 1, argv[2]) ;

            break ;
        }

        if (fscanf (reference, "%lf %lf %lf\n", &time3, &temp3, &temp4) != 3)
        {
            fprintf (stdout, "Error reading line %d from %s\n", counter + 1, argv[3]) ;

            break ;
        }

        if (time1 != time2 || time2 != time3)
        {
            fprintf (stdout, "Time mismatch at line %d (%.3f %.3f %.3f)\n",
                counter + 1, time1, time2, time3) ;

            break ;
        }

        tmp = fabs (temp1 - temp3) ;

        if (tmp > max_node1) { max_node1 = tmp ; time_node1 = time1 ; }

        tmp = fabs (temp2 - temp4) ;

        if (tmp > max_node2) { max_node2 = tmp ; time_node2 = time1 ; }
    }

    if (counter == counter1)

        fprintf (stdout, "%.3f (@%.3f) \t%.3f (@%.3f)\n",
            max_node1, time_node1, max_node2, time_node2) ;

    // Closes ...
    ////////////////////////////////////////////////////////////////////////////

    fclose (node1) ;
    fclose (node2) ;
    fclose (reference) ;

    return EXIT_SUCCESS ;

error :

    fclose (node1) ;
    fclose (node2) ;
    fclose (reference) ;

    return EXIT_FAILURE ;

}
