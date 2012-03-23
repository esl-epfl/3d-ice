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

#include <time.h>

#include "stack_description.h"
#include "thermal_data.h"
#include "analysis.h"

int main(int argc, char** argv)
{
    StackDescription stkd ;
    Analysis         analysis ;
    ThermalData      tdata ;

    SimResult_t (*emulate) (ThermalData*, StackDescription*, Analysis*) ;

    Error_t error ;

    // Checks if there are the all the arguments
    ////////////////////////////////////////////////////////////////////////////

    if (argc != 2)
    {
        fprintf(stderr, "Usage: \"%s file.stk\"\n", argv[0]) ;
        return EXIT_FAILURE ;
    }

    // Init StackDescription and parse the input file
    ////////////////////////////////////////////////////////////////////////////

    fprintf (stdout, "Preparing stk data ... ") ; fflush (stdout) ;

    init_stack_description (&stkd) ;
    init_analysis          (&analysis) ;

    error = fill_stack_description (&stkd, &analysis, argv[1]) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    if (analysis.AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)

        emulate = &emulate_step ;

    else if (analysis.AnalysisType == TDICE_ANALYSIS_TYPE_STEADY)

        emulate = &emulate_steady ;

    else
    {
        fprintf (stderr, "unknown analysis type!\n ");

        free_stack_description (&stkd) ;

        return EXIT_FAILURE ;
    }

    fprintf (stdout, "done !\n") ;

    // Generate output files
    ////////////////////////////////////////////////////////////////////////////

    // We use "% " as prefix for matlab compatibility (header will be a comment)

    error = generate_analysis_headers (&analysis, stkd.Dimensions, "% ") ;

    if (error != TDICE_SUCCESS)
    {
        fprintf (stderr, "error in initializing output files \n ");

        free_stack_description (&stkd) ;

        return EXIT_FAILURE ;
    }

    // Init thermal data and fill it using the StackDescription
    ////////////////////////////////////////////////////////////////////////////

    fprintf (stdout, "Preparing thermal data ... ") ; fflush (stdout) ;

    init_thermal_data (&tdata) ;

    error = fill_thermal_data (&tdata, &stkd, &analysis) ;

    if (error != TDICE_SUCCESS)
    {
        free_analysis          (&analysis) ;
        free_stack_description (&stkd) ;

        return EXIT_FAILURE ;
    }

    fprintf (stdout, "done !\n") ;

    // Run the simulation and print the output
    ////////////////////////////////////////////////////////////////////////////

    clock_t Time = clock() ;

    SimResult_t sim_result ;

    do
    {
        sim_result = emulate (&tdata, &stkd, &analysis) ;

        if (sim_result == TDICE_STEP_DONE || sim_result == TDICE_SLOT_DONE)
        {
            fprintf (stdout, "%.3f ", get_simulated_time (&analysis)) ;

            fflush (stdout) ;

            generate_analysis_output

                (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_INSTANT_STEP) ;
        }

        if (sim_result == TDICE_SLOT_DONE)
        {
            fprintf (stdout, "\n") ;

            generate_analysis_output

                (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_INSTANT_SLOT) ;
        }

    } while (sim_result != TDICE_END_OF_SIMULATION) ;

    generate_analysis_output

        (&analysis, stkd.Dimensions, tdata.Temperatures, TDICE_OUTPUT_INSTANT_FINAL) ;

    fprintf (stdout, "emulation took %.3f sec\n",
        ( (double)clock() - Time ) / CLOCKS_PER_SEC ) ;

    // free all data
    ////////////////////////////////////////////////////////////////////////////

    free_thermal_data      (&tdata) ;
    free_analysis          (&analysis) ;
    free_stack_description (&stkd) ;

    return EXIT_SUCCESS ;
}
