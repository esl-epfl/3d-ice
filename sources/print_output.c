/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
#include <string.h>

#include "print_output.h"
#include "macros.h"

/******************************************************************************/

void init_tcell (Tcell *tcell)
{
    tcell->Id          = NULL ;
    tcell->Xval        = 0.0 ;
    tcell->ActualXval  = 0.0 ;
    tcell->Yval        = 0.0 ;
    tcell->ActualYval  = 0.0 ;
    tcell->LayerIndex  = 0u ;
    tcell->RowIndex    = 0u ;
    tcell->ColumnIndex = 0u ;
}

/******************************************************************************/

Tcell *alloc_and_init_tcell (void)
{
    Tcell *tcell = (Tcell *) malloc (sizeof(Tcell)) ;

    if (tcell != NULL)

        init_tcell (tcell) ;

    return tcell ;
}

/******************************************************************************/

void free_tcell (Tcell *tcell)
{
    FREE_POINTER(free, tcell->Id) ;
    FREE_POINTER(free, tcell) ;
}

/******************************************************************************/

void print_detailed_tcell (FILE  *stream, char *prefix, Tcell *tcell)
{
    fprintf (stream,
        "%sTcell                   = %p\n",
        prefix, tcell) ;

    fprintf (stream,
        "%s  Id                    = %s\n",
        prefix, tcell->Id) ;

    fprintf (stream,
        "%s  Xval                  = %.2f\n",
        prefix, tcell->Xval) ;

    fprintf (stream,
        "%s  actualXval            = %.2f\n",
        prefix, tcell->ActualXval) ;

    fprintf (stream,
        "%s  Yval                  = %.2f\n",
        prefix, tcell->Yval) ;

    fprintf (stream,
        "%s  actualYval            = %.2f\n",
        prefix, tcell->ActualYval) ;

    fprintf (stream,
        "%s  LayerIndex            = %d\n",
        prefix, tcell->LayerIndex) ;

    fprintf (stream,
        "%s  RowIndex              = %d\n",
        prefix, tcell->RowIndex) ;

    fprintf (stream,
        "%s  ColumnIndex           = %d\n",
        prefix, tcell->ColumnIndex) ;
}

/******************************************************************************/

void align_tcell
(
    Tcell      *tcell,
    double      xval,
    double      yval,
    Dimensions *dimensions
)
{
    // FIXME

    FOR_EVERY_ROW (row_index, dimensions)
    {
        tcell->RowIndex   = row_index ;
        tcell->ActualYval = get_cell_location_y (dimensions, row_index) ;

        if (tcell->ActualYval > yval)

            break ;
    }

    tcell->Yval = yval ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        tcell->ColumnIndex = column_index ;
        tcell->ActualXval  = get_cell_location_x (dimensions, column_index) ;

        if (tcell->ActualXval > xval)

            break ;
    }

    tcell->Xval = xval ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tflp (Tflp *tflp)
{
    tflp->Id       = NULL ;
    tflp->Quantity = OUTPUTQUANTITY_I ;
}

/******************************************************************************/

Tflp *alloc_and_init_tflp (void)
{
    Tflp *tflp = (Tflp *) malloc (sizeof(Tflp)) ;

    if (tflp != NULL)

        init_tflp(tflp) ;

    return tflp ;
}

/******************************************************************************/

void free_tflp (Tflp *tflp)
{
    FREE_POINTER(free, tflp->Id) ;
    FREE_POINTER(free, tflp) ;
}

/******************************************************************************/

void print_detailed_tflp (FILE  *stream, char *prefix, Tflp *tflp)
{
    fprintf (stream,
        "%sTflp                    = %p\n",
        prefix, tflp) ;

    fprintf (stream,
        "%s  Id                    = %s\n",
        prefix, tflp->Id) ;

    fprintf (stream,
        "%s  Quantity              = %d\n",
        prefix, tflp->Quantity) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tflpel (Tflpel *tflpel)
{
    tflpel->Id       = NULL ;
    tflpel->FlpId    = NULL ;
    tflpel->Quantity = OUTPUTQUANTITY_I ;
}

/******************************************************************************/

Tflpel *alloc_and_init_tflpel (void)
{
    Tflpel *tflpel = (Tflpel *) malloc (sizeof(Tflpel)) ;

    if (tflpel != NULL)

        init_tflpel(tflpel) ;

    return tflpel ;
}

/******************************************************************************/

void free_tflpel (Tflpel *tflpel)
{
    FREE_POINTER(free, tflpel->Id) ;
    FREE_POINTER(free, tflpel->FlpId) ;
    FREE_POINTER(free, tflpel) ;
}

/******************************************************************************/

void print_detailed_tflpel (FILE  *stream, char *prefix, Tflpel *tflpel)
{
    fprintf (stream,
        "%sTflpel                  = %p\n",
        prefix, tflpel) ;

    fprintf (stream,
        "%s  Id                    = %s\n",
        prefix, tflpel->Id) ;

    fprintf (stream,
        "%s  FlpId                 = %s\n",
        prefix, tflpel->FlpId) ;

    fprintf (stream,
        "%s  Quantity              = %d\n",
        prefix, tflpel->Quantity) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_tmap(Tmap *tmap)
{
    tmap->Id = NULL ;
}

/******************************************************************************/

Tmap *alloc_and_init_tmap (void)
{
    Tmap *tmap = (Tmap *) malloc (sizeof(Tmap)) ;

    if (tmap != NULL)

        init_tmap (tmap) ;

    return tmap ;
}

/******************************************************************************/

void free_tmap (Tmap *tmap)
{
    FREE_POINTER(free, tmap->Id) ;
    FREE_POINTER(free, tmap) ;
}

/******************************************************************************/

void print_detailed_tmap (FILE  *stream, char *prefix, Tmap *tmap)
{
    fprintf (stream,
        "%sTmap                    = %p\n",
        prefix, tmap) ;

    fprintf (stream,
        "%s  Id                    = %s\n",
        prefix, tmap->Id) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_print_output (PrintOutput *print_output)
{
    print_output->Type           = OUTPUTTYPE_I ;
    print_output->InstanceType   = OUTPUTINSTANCETYPE_I ;
    print_output->FileName       = NULL ;
    print_output->Pointer.Tcell  = NULL ;
    print_output->Pointer.Tflp   = NULL ;
    print_output->Pointer.Tflpel = NULL ;
    print_output->Pointer.Tmap   = NULL ;
    print_output->Next           = NULL ;
}

/******************************************************************************/

PrintOutput *alloc_and_init_print_output (void)
{
    PrintOutput *print_output = (PrintOutput *) malloc (sizeof(PrintOutput)) ;

    if (print_output != NULL)

        init_print_output(print_output) ;

    return print_output ;
}

/******************************************************************************/

void free_print_output (PrintOutput *print_output)
{
    if (   print_output->Type == TDICE_OUTPUT_TCELL
        && print_output->Pointer.Tcell != NULL)

        FREE_POINTER (free_tcell, print_output->Pointer.Tcell) ;

    else if (   print_output->Type == TDICE_OUTPUT_TFLP
             && print_output->Pointer.Tflp != NULL)

        FREE_POINTER (free_tflp, print_output->Pointer.Tflp) ;

    else if (   print_output->Type == TDICE_OUTPUT_TFLPEL
             && print_output->Pointer.Tflpel != NULL)

        FREE_POINTER (free_tflpel, print_output->Pointer.Tflpel) ;

    else if (   print_output->Type == TDICE_OUTPUT_TMAP
             && print_output->Pointer.Tmap != NULL)

        FREE_POINTER (free_tmap, print_output->Pointer.Tmap) ;

    FREE_POINTER (free, print_output->FileName) ;
    FREE_POINTER (free, print_output) ;
}

/******************************************************************************/

void free_print_output_list (PrintOutput *list)
{
    FREE_LIST (PrintOutput, list, free_print_output) ;
}

/******************************************************************************/

void print_formatted_print_output_list
(
    FILE        *stream,
    char        *prefix,
    PrintOutput *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (PrintOutput, print_output, list)
    {
        switch (print_output->Type)
        {
            case TDICE_OUTPUT_TCELL :

                fprintf (stream, "%sT      (%s, %.1f, %.1f, \"%s\", ",
                    prefix, print_output->Pointer.Tcell->Id,
                    print_output->Pointer.Tcell->Xval,
                    print_output->Pointer.Tcell->Yval,
                    print_output->FileName) ;

                break ;

            case TDICE_OUTPUT_TFLP :

                fprintf (stream, "%sTflp   (%s, \"%s\", ",
                    prefix, print_output->Pointer.Tflp->Id,
                    print_output->FileName) ;

                if (print_output->Pointer.Tflp->Quantity==TDICE_OUTPUT_MAXIMUM)

                    fprintf(stream, "maximum, ");

                else if (print_output->Pointer.Tflp->Quantity==TDICE_OUTPUT_MINIMUM)

                    fprintf(stream, "minimum, ");

                else

                    fprintf(stream, "average, ");

                break ;

            case TDICE_OUTPUT_TFLPEL :

                fprintf (stream, "%sTflpel (%s.%s, \"%s\", ",
                    prefix, print_output->Pointer.Tflpel->Id,
                    print_output->Pointer.Tflpel->FlpId,
                    print_output->FileName) ;

                if (print_output->Pointer.Tflpel->Quantity==TDICE_OUTPUT_MAXIMUM)

                    fprintf(stream, "maximum, ");

                else if (print_output->Pointer.Tflpel->Quantity==TDICE_OUTPUT_MINIMUM)

                    fprintf(stream, "minimum, ");

                else

                    fprintf(stream, "average, ");

                break ;

            case TDICE_OUTPUT_TMAP :

                fprintf (stream, "%sTmap   (%s, \"%s\", ",
                    prefix, print_output->Pointer.Tmap->Id,
                    print_output->FileName) ;

                break ;

            default :

                fprintf (stderr, "Undefined print output command type %d\n", print_output->Type) ;
                break ;
        }

        if (print_output->InstanceType == TDICE_OUTPUT_SLOT)

            fprintf(stream, "slot );\n");

        else if (print_output->InstanceType == TDICE_OUTPUT_STEP)

            fprintf(stream, "step );\n");

        else

            fprintf(stream, "final );\n");

    } // FOR_EVERY_ELEMENT_IN_LIST
}

/******************************************************************************/

void print_detailed_print_output_list
(
    FILE        *stream,
    char        *prefix,
    PrintOutput *list
)
{
    char *new_prefix = malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (PrintOutput, prt_out, list)
    {
        fprintf (stream,
            "%sprint_output                = %p\n",
            prefix,   prt_out);

        fprintf (stream,
            "%s  FileName                  = %s\n",
            prefix,   prt_out->FileName);

        fprintf (stream,
            "%s  InstanceType              = %d\n",
            prefix,   prt_out->InstanceType);

        fprintf (stream,
            "%s  Type                      = %d\n",
            prefix,   prt_out->Type);

        switch(prt_out->Type)
        {
            case TDICE_OUTPUT_TCELL :

                fprintf (stream,
                    "%s  Pointer.Tcell             = %p\n",
                    prefix,   prt_out->Pointer.Tcell);

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tcell

                        (stream, new_prefix, prt_out->Pointer.Tcell) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            case TDICE_OUTPUT_TFLP :

                fprintf (stream,
                    "%s  Pointer.Tflp              = %p\n",
                    prefix,   prt_out->Pointer.Tflp); 

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tflp

                        (stream, new_prefix, prt_out->Pointer.Tflp) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            case TDICE_OUTPUT_TFLPEL :

                fprintf (stream,
                    "%s  Pointer.Tflpel            = %p\n",
                    prefix,   prt_out->Pointer.Tflpel); 

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tflpel

                        (stream, new_prefix, prt_out->Pointer.Tflpel) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            case TDICE_OUTPUT_TMAP :

                fprintf (stream,
                    "%s  Pointer.Tmap              = %p\n",
                    prefix,   prt_out->Pointer.Tmap); 

                    fprintf (stream, "%s\n", prefix) ;

                    print_detailed_tmap

                        (stream, new_prefix, prt_out->Pointer.Tmap) ;

                    fprintf (stream, "%s\n", prefix) ;

                break ;

            default :
                fprintf (stream, "Undefined print output command Type %d\n", prt_out->Type) ;
                break ;
        }

        fprintf (stream,
            "%s  Next                      = %p\n",
            prefix,   prt_out->Next);

        fprintf (stream, "%s\n", prefix) ;

    } // FOR_EVERY_ELEMENT_IN_LIST

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

Error_t initialize_print_output
(
    PrintOutput  *print_output ,
    StackElement *list
)
{
    FILE *output_stream ;

    if ((output_stream = fopen(print_output->FileName, "w")) == NULL)
    {
        fprintf(stderr, "Print Output: Cannot open %s\n", print_output->FileName);

        return TDICE_FAILURE ;
    }

    switch (print_output->Type)
    {
        // Tcell
        case TDICE_OUTPUT_TCELL :
        {
            fprintf(output_stream, "Cell temperature for the location [%s, x=%5.3f,y=%5.3f]\n", print_output->Pointer.Tcell->Id, print_output->Pointer.Tcell->Xval, print_output->Pointer.Tcell->Yval) ;
            fprintf(output_stream, "Nearest [column, row] indices found= [%d,%d] (locations [x=%5.3f,y=%5.3f])\n", print_output->Pointer.Tcell->ColumnIndex, print_output->Pointer.Tcell->RowIndex, print_output->Pointer.Tcell->ActualXval, print_output->Pointer.Tcell->ActualYval);
            fprintf(output_stream, "Time(s) \t Temperature(K)\n");
            break ;
        }

        // Tflp
        case TDICE_OUTPUT_TFLP :
        {
            if (print_output->Pointer.Tflp->Quantity==TDICE_OUTPUT_MAXIMUM)
                fprintf(output_stream, "Maximum ");
            else if (print_output->Pointer.Tflp->Quantity==TDICE_OUTPUT_MINIMUM)
                fprintf(output_stream, "Minimum ");
            else if (print_output->Pointer.Tflp->Quantity==TDICE_OUTPUT_AVERAGE)
                fprintf(output_stream, "Average ");
            else
            { 
                fprintf(stderr, "Print Output: Error reading output quantity for Tflp\n") ;
                fclose(output_stream) ;
                return TDICE_FAILURE ;
            }
            fprintf(output_stream, "temperatures for the floorplan of the die %s\n", print_output->Pointer.Tflp->Id) ;

            StackElement *stk_el = find_stack_element_in_list (list, print_output->Pointer.Tflp->Id) ;
            if (stk_el->Floorplan == NULL)
            { 
                fprintf(stderr, "Print Output: Error reading floorplan for the die %s\n", print_output->Pointer.Tflp->Id) ;
                fclose(output_stream) ;
                return TDICE_FAILURE ;
            }

            fprintf(output_stream, "Time(s) \t ");
            FOR_EVERY_ELEMENT_IN_LIST_FORWARD (FloorplanElement, flp_el, stk_el->Floorplan->ElementsList)
            {
                fprintf(output_stream, "%s(K) \t ", flp_el->Id);
            }
            fprintf(output_stream, "\n") ;
            break ;
        }

        //Tflpel 
        case TDICE_OUTPUT_TFLPEL :
        {
            if (print_output->Pointer.Tflpel->Quantity==TDICE_OUTPUT_MAXIMUM)
                fprintf(output_stream, "Maximum ");
            else if (print_output->Pointer.Tflpel->Quantity==TDICE_OUTPUT_MINIMUM)
                fprintf(output_stream, "Minimum ");
            else if (print_output->Pointer.Tflpel->Quantity==TDICE_OUTPUT_AVERAGE)
                fprintf(output_stream, "Average ");
            else
            { 
                fprintf(stderr, "Print Output: Error reading output quantity for Tflpel\n") ;
                fclose(output_stream) ;
                return TDICE_FAILURE ;
            }
            fprintf(output_stream, "temperatures for the floorplan element %s of the die %s\n", print_output->Pointer.Tflpel->FlpId, print_output->Pointer.Tflpel->Id) ;
            fprintf(output_stream, "Time(s) \t %s.%s(K)\n", print_output->Pointer.Tflpel->Id, print_output->Pointer.Tflpel->FlpId) ;
            break ;
        }

        //Tmap
        case TDICE_OUTPUT_TMAP :
        {
            fprintf(output_stream, "Thermal map for layer %s (please find axis information in \"xaxis.txt\" and \"yaxis.txt\")\n", print_output->Pointer.Tmap->Id);
            break ;
        }

        default :
        {
            fprintf (stderr, "Error reading output instruction\n") ;
            fclose(output_stream) ;
            return TDICE_FAILURE ;
            break ;
        }
    }

    fclose (output_stream) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

bool there_is_tmap_in_list (PrintOutput *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (PrintOutput, prt_out, list)

        if (prt_out->Type == TDICE_OUTPUT_TMAP)

            return true ;

    return false ;
}

/******************************************************************************/
