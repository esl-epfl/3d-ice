/******************************************************************************
 * This file is part of 3D-ICE, version 3.0.0 .                               *
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

#include "stack_file_parser.h"

#include "stack_description.h"
#include "thermal_data.h"
#include "output.h"
#include "analysis.h"

int main(int argc, char** argv)
{
    StackDescription_t stkd ;
    Analysis_t         analysis ;
    Output_t           output ;
    ThermalData_t      tdata ;

    SimResult_t (*emulate) (ThermalData_t*, Dimensions_t*, Analysis_t*) ;

    Error_t error ;

    // Checks if there are the all the arguments
    ////////////////////////////////////////////////////////////////////////////

#define NARGC        2
#define EXE_NAME     argv[0]
#define STK_FILE     argv[1]

    if (argc != NARGC)
    {
        fprintf(stderr, "Usage: \"%s file.stk\"\n", EXE_NAME) ;
        return EXIT_FAILURE ;
    }

    // Init StackDescription and parse the input file
    ////////////////////////////////////////////////////////////////////////////

    fprintf (stdout, "Preparing stk data ... ") ; fflush (stdout) ;

    stack_description_init (&stkd) ;
    analysis_init          (&analysis) ;
    output_init            (&output) ;

    error = parse_stack_description_file (STK_FILE, &stkd, &analysis, &output) ;

    if (error != TDICE_SUCCESS)    return EXIT_FAILURE ;

    if (analysis.AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)

        emulate = &emulate_step ;

    else if (analysis.AnalysisType == TDICE_ANALYSIS_TYPE_STEADY)

        emulate = &emulate_steady ;

    else
    {
        fprintf (stderr, "unknown analysis type!\n ");

        stack_description_destroy (&stkd) ;
        output_destroy            (&output) ;

        return EXIT_FAILURE ;
    }

    fprintf (stdout, "done !\n") ;

    // Generate output files
    ////////////////////////////////////////////////////////////////////////////

    // We use "% " as prefix for matlab compatibility (header will be a comment)

    error = generate_output_headers (&output, stkd.Dimensions, (String_t)"% ") ;

    if (error != TDICE_SUCCESS)
    {
        fprintf (stderr, "error in initializing output files \n ");

        stack_description_destroy (&stkd) ;
        output_destroy            (&output) ;

        return EXIT_FAILURE ;
    }

    // Init thermal data and fill it using the StackDescription
    ////////////////////////////////////////////////////////////////////////////

    fprintf (stdout, "Preparing thermal data ... ") ; fflush (stdout) ;

    thermal_data_init (&tdata) ;

    error = thermal_data_build

        (&tdata, &stkd.StackElements, stkd.Dimensions, &analysis) ;

    if (error != TDICE_SUCCESS)
    {
        stack_description_destroy (&stkd) ;
        output_destroy            (&output) ;

        return EXIT_FAILURE ;
    }

    fprintf (stdout, "done !\n") ;

    // Run the simulation and print the output
    ////////////////////////////////////////////////////////////////////////////

    clock_t Time = clock() ;

    SimResult_t sim_result ;

    do
    {
        sim_result = emulate (&tdata, stkd.Dimensions, &analysis) ;

        printf("Temperature grid info:\n");
        for(CellIndex_t i = 0; i < stkd.Dimensions->Grid.NCells; i++)
            printf("%d:\t%f\n", i, *(tdata.Temperatures+i));

        // New output part 1/2
        int bar_len = 30;
        char s1[31] = "------------------------------";
        char s2[31] = "                              ";
        int time_cur;
        int slot_cur;
        float scale_factor = (float) bar_len / (float) analysis.SlotLength;;

        if (sim_result == TDICE_STEP_DONE || sim_result == TDICE_SLOT_DONE)
        {
            // New output part 2/2
            time_cur = (analysis.CurrentTime % analysis.SlotLength)*scale_factor;
            if (analysis.CurrentTime % analysis.SlotLength == 0)
                 time_cur = bar_len;
            slot_cur = (analysis.CurrentTime-1) / analysis.SlotLength + 1;
            fprintf(stderr, "Time slot %d: |%.*s>%.*s| %.3fs\r", slot_cur, time_cur, s1, bar_len-time_cur, s2, analysis.CurrentTime*analysis.StepTime);
            fflush(stderr);  //< Flush the output (just in case)

            // Original ouput
            // fprintf (stdout, "%.3f ", get_simulated_time (&analysis)) ;

            // fflush (stdout) ;

            generate_output (&output, stkd.Dimensions,
                             tdata.Temperatures, tdata.PowerGrid.Sources,
                             get_simulated_time (&analysis),
                             TDICE_OUTPUT_INSTANT_STEP) ;
        }

        if (sim_result == TDICE_SLOT_DONE)
        {
            fprintf (stdout, "\n") ;

            generate_output (&output, stkd.Dimensions,
                             tdata.Temperatures, tdata.PowerGrid.Sources,
                             get_simulated_time (&analysis),
                             TDICE_OUTPUT_INSTANT_SLOT) ;
        }

    } while (sim_result != TDICE_END_OF_SIMULATION && sim_result != TDICE_SOLVER_ERROR) ;

    generate_output (&output, stkd.Dimensions,
                     tdata.Temperatures, tdata.PowerGrid.Sources,
                     get_simulated_time (&analysis),
                     TDICE_OUTPUT_INSTANT_FINAL) ;

    fprintf (stdout, "emulation took %.3f sec\n",
        ( (double)clock() - Time ) / CLOCKS_PER_SEC ) ;

    // free all data
    ////////////////////////////////////////////////////////////////////////////

    thermal_data_destroy      (&tdata) ;
    stack_description_destroy (&stkd) ;
    output_destroy            (&output) ;

    return EXIT_SUCCESS ;
}
