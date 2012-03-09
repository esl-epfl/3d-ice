/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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

#include "types.h"
#include "macros.h"
#include "stack_description.h"
#include "analysis.h"
#include "stack_element.h"
#include "floorplan.h"
#include "ic_element.h"
#include "floorplan_element.h"

#define CHIP_BORDER       2
#define FLPEL_REAL_BORDER 1
#define FLPEL_EFF_BORDER  1
#define CELL_CENTER       1
#define SCALE             5

int main (int argc, char** argv)
{
    StackDescription  stack ;
    Analysis          analysis ;
    StackElement     *stack_element ;
    FILE             *svg_file ;
    Floorplan        *floorplan ;

    /* Check command line *****************************************************/

    if (argc != 4)
    {
        fprintf (stderr, "Usage: \"%s file_stk stack_element_id file_svg\n", argv[0]) ;

        return EXIT_FAILURE ;
    }

    /* Init and parse stack file **********************************************/

    init_stack_description (&stack) ;
    init_analysis          (&analysis) ;

    if (fill_stack_description (&stack, &analysis, argv[1]) != TDICE_SUCCESS)

        return EXIT_FAILURE ;

    /* Open svg file **********************************************************/

    svg_file = fopen (argv[3], "w") ;

    if (svg_file == NULL)
    {
        perror ("Error while opening svg file") ;

        goto free_and_fail ;
    }

    /* Opens svg tag **********************************************************/

    uint32_t ChipWidth  = get_chip_width  (stack.Dimensions) / SCALE ;
    uint32_t ChipLength = get_chip_length (stack.Dimensions) / SCALE ;

    fprintf (svg_file,

        "<?xml version=\"1.0\" standalone=\"no\"?>\n"              \
        "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\""         \
        " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" \
        "<svg length=\"%d\" width=\"%d\" version=\"1.1\""          \
        " xmlns=\"http://www.w3.org/2000/svg\">\n",

        ChipLength, ChipWidth) ;


    /* Draws the IC border ****************************************************/

    fprintf (svg_file,
        "\t<rect x=\"0\" y=\"0\""            \
               " width=\"%d\" height=\"%d\"" \
               " fill=\"white\" stroke=\"black\" stroke-width=\"%d\"/>\n",
        ChipLength, ChipWidth, CHIP_BORDER) ;

    /* Looks for the floorplan to draw ****************************************/

    stack_element = find_stack_element_in_list (stack.BottomStackElement, argv[2]) ;

    if (stack_element == NULL)
    {
        fprintf (stderr, "Stack Element %s not found\n", argv[2]) ;

        goto close_free_and_fail ;
    }

    if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
    {
        fprintf (stderr, "The Stack Element %s is not a die\n", argv[2]) ;

        goto close_free_and_fail ;
    }

    floorplan = stack_element->Floorplan ;

    /* Draws every flooprlan element in the floorplan            **************/
    /* using the dimensions after the placement on the cell grid **************/
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, floorplan_element, floorplan->ElementsList)
    {
        FOR_EVERY_ELEMENT_IN_LIST_NEXT

        (ICElement, ice_element, floorplan_element->ICElementsList)
        {
            fprintf (svg_file,

                "\t<rect x=\"%d\" y=\"%d\""          \
                    " width=\"%d\" height=\"%d\""    \
                    " fill=\"none\" stroke=\"red\" stroke-width=\"%d\"/>\n",

                (uint32_t) (get_cell_location_x(stack.Dimensions, ice_element->SW_Column) / SCALE)
                           + FLPEL_EFF_BORDER,
                (uint32_t) (ChipWidth
                            - (get_cell_location_y(stack.Dimensions, ice_element->SW_Row) / SCALE)
                            - (ice_element->EffectiveWidth / SCALE))
                           + FLPEL_EFF_BORDER,
                (uint32_t) (ice_element->EffectiveLength / SCALE) - FLPEL_EFF_BORDER,
                (uint32_t) (ice_element->EffectiveWidth  / SCALE) - FLPEL_EFF_BORDER,
                FLPEL_EFF_BORDER) ;
        }
    }
}
    /* Draws every flooprlan element in the floorplan      ********************/
    /* using the dimensions declared in the flooprlan file ********************/
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, floorplan_element, floorplan->ElementsList)
    {
        FOR_EVERY_ELEMENT_IN_LIST_NEXT

        (ICElement, ice_element, floorplan_element->ICElementsList)
        {
            fprintf (svg_file,

                "\t<rect x=\"%d\" y=\"%d\""          \
                    " width=\"%d\" height=\"%d\""    \
                    " fill=\"none\" stroke=\"black\" stroke-width=\"%d\"/>\n",

                (uint32_t) (ice_element->SW_X / SCALE) + FLPEL_EFF_BORDER,
                (uint32_t) (ChipWidth
                            - (ice_element->SW_Y / SCALE)
                            - (ice_element->Width / SCALE)) + FLPEL_EFF_BORDER,
                (uint32_t) (ice_element->Length / SCALE) - FLPEL_EFF_BORDER,
                (uint32_t) (ice_element->Width  / SCALE) - FLPEL_EFF_BORDER,
                FLPEL_REAL_BORDER) ;
        }
    }
}
    /* Draws the center of every thermal cell *********************************/

    FOR_EVERY_ROW (row_index, stack.Dimensions)
    {
        FOR_EVERY_COLUMN (column_index, stack.Dimensions)
        {
            fprintf (svg_file,

                "\t<circle cx=\"%d\" cy=\"%d\" r=\"%d\"/>\n",

                (uint32_t) (get_cell_center_x (stack.Dimensions, column_index) / SCALE),
                (uint32_t) (ChipWidth
                             - (get_cell_center_y (stack.Dimensions, row_index) / SCALE)),
                CELL_CENTER) ;
        }
    }

    /* Closes svg tag *********************************************************/

    fprintf (svg_file, "</svg>") ;

    /* Close svg file *********************************************************/

    if (fclose (svg_file) != 0)
    {
        perror ("Error while closing svg file") ;

        goto free_and_fail ;
    }

    /* Free 3D-ICE data *******************************************************/

    free_stack_description (&stack) ;
    free_analysis (&analysis) ;

    return EXIT_SUCCESS ;

close_free_and_fail :

    if (fclose (svg_file) != 0)

        perror ("Error while closing svg file") ;

free_and_fail :

    free_stack_description (&stack) ;
    free_analysis (&analysis) ;

    return EXIT_FAILURE ;
}

