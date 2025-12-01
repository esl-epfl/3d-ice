/******************************************************************************
 * This file is part of 3D-ICE, version 4.0 .                                 *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Kai Zhu                     Luis Costero                          *
 *          Marina Zapater              David Atienza                         *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#include <stdlib.h> // For the memory functions malloc/free
#include <string.h>
#include "stack_element.h"

/******************************************************************************/

void stack_element_init (StackElement_t *stkel)
{
    stkel->SEType           = (StackElementType_t) TDICE_STACK_ELEMENT_NONE ;
    stkel->Pointer.Layer    = NULL ;
    stkel->Pointer.Die      = NULL ;
    stkel->Pointer.Channel  = NULL ;

    stkel->TopSink    = NULL ;
    stkel->BottomSink = NULL ;

    string_init (&stkel->Id) ;

    stkel->NLayers          = (CellIndex_t) 0u ;
    stkel->Offset           = (CellIndex_t) 0u ;
}

/******************************************************************************/

void stack_element_copy (StackElement_t *dst, StackElement_t *src)
{
    stack_element_destroy (dst) ;

    dst->SEType   = src->SEType ;
    dst->NLayers  = src->NLayers ;
    dst->Offset   = src->Offset ;

    string_copy (&dst->Id, &src->Id) ;

    if (src->SEType == TDICE_STACK_ELEMENT_LAYER)

        dst->Pointer.Layer = layer_clone (src->Pointer.Layer) ;

    else if (src->SEType == TDICE_STACK_ELEMENT_DIE)

        dst->Pointer.Die = die_clone (src->Pointer.Die) ;

    else

        // Channel

        dst->Pointer = src->Pointer ;

   if (src->TopSink    != NULL) dst->TopSink    = heat_sink_clone (src->TopSink) ;
   if (src->BottomSink != NULL) dst->BottomSink = heat_sink_clone (src->BottomSink) ;
}

/******************************************************************************/

void stack_element_destroy (StackElement_t *stkel)
{
    string_destroy (&stkel->Id) ;

    if (stkel->SEType == TDICE_STACK_ELEMENT_DIE)

        die_free (stkel->Pointer.Die) ;

    else if (stkel->SEType == TDICE_STACK_ELEMENT_LAYER)

        layer_free (stkel->Pointer.Layer) ;

    if (stkel->TopSink != NULL)     heat_sink_free (stkel->TopSink) ;
    if (stkel->BottomSink != NULL)  heat_sink_free (stkel->BottomSink) ;

    stack_element_init (stkel) ;
}

/******************************************************************************/

StackElement_t *stack_element_calloc (void)
{
    StackElement_t *stkel = (StackElement_t *) malloc (sizeof(StackElement_t)) ;

    if (stkel != NULL)

        stack_element_init (stkel) ;

    return stkel ;
}

/******************************************************************************/

StackElement_t *stack_element_clone (StackElement_t *stkel)
{
    if (stkel == NULL)

        return NULL ;

    StackElement_t *news = stack_element_calloc ( ) ;

    if (news != NULL)

        stack_element_copy (news, stkel) ;

    return news ;
}

/******************************************************************************/

void stack_element_free (StackElement_t *stkel)
{
    if (stkel == NULL)

        return ;

    stack_element_destroy (stkel) ;

    free (stkel) ;
}

/******************************************************************************/

bool stack_element_same_id (StackElement_t *stkel, StackElement_t *other)
{
    return string_equal (&stkel->Id, &other->Id) ;
}

/******************************************************************************/

void stack_element_print
(
    StackElement_t *stkel,
    FILE           *stream,
    String_t        prefix
)
{
    switch (stkel->SEType)
    {
        case TDICE_STACK_ELEMENT_CHANNEL :

            fprintf (stream,
                "%s   channel  %s ;\n",
                prefix, stkel->Id) ;
            break ;

        case TDICE_STACK_ELEMENT_DIE :

            fprintf (stream,
                "%s   die      %s %s floorplan \"%s\" ;\n",
                prefix,
                stkel->Id,
                stkel->Pointer.Die->Id,
                stkel->Pointer.Die->Floorplan.FileName) ;

            break ;

        case TDICE_STACK_ELEMENT_LAYER :

            fprintf (stream,
                "%s   layer    %s %s ;\n",
                prefix,
                stkel->Id, stkel->Pointer.Layer->Id) ;

            break ;

        case TDICE_STACK_ELEMENT_NONE :

            fprintf (stderr, "Warning: found stack element type none\n") ;
            break ;

        default :

            fprintf (stderr, "Undefined stack element type %d\n", stkel->SEType) ;
    }
}

/******************************************************************************/

CellIndex_t get_source_layer_offset (StackElement_t *stkel)
{
    CellIndex_t layer_offset = stkel->Offset ;

    if (stkel->SEType == TDICE_STACK_ELEMENT_DIE)

        layer_offset += stkel->Pointer.Die->SourceLayerOffset ;

    else if (stkel->SEType == TDICE_STACK_ELEMENT_CHANNEL)

        layer_offset += stkel->Pointer.Channel->SourceLayerOffset ;

    return layer_offset ;
}

/******************************************************************************/

void stack_element_print_thermal_map
(
    StackElement_t  *stkel,
    Dimensions_t    *dimensions,
    Temperature_t   *temperatures,
    FILE            *stream
)
{
    
    if (dimensions->NonUniform == 1)
    {
        // write the xyaxis file in the non-uniform scenario
        char name[50] = "xyaxis_";
        strcat(name,stkel->Id);
        strcat(name,".txt");
        FILE *filexy = fopen (name, "w") ;
        if (filexy == NULL)
        {
            fprintf (stderr, "Cannot create text file for x axis in the non-uniform scenario\n") ;
            return ;
        }
        // output all of the cells' temperature in the layer for non-uniform scenario
        CellIndex_t counter = 0;
        CellIndex_t layer_offset = get_source_layer_offset(stkel) ;
        for (Non_uniform_cellListNode_t* cell_i = dimensions->Cell_list.First; cell_i != NULL; cell_i = cell_i->Next)
        {
            if (cell_i->Data.layer_info == layer_offset)
            {
                fprintf (stream, "%7.3f  ", *(temperatures+counter)) ;
                fprintf (filexy, "%5.2f\t%5.2f\t%5.2f\t%5.2f\n", cell_i->Data.left_x, cell_i->Data.left_y, cell_i->Data.length,cell_i->Data.width) ;
            }
            counter++;
        }
        fprintf (stream, "\n") ;
        fclose (filexy) ;
    }
    else
    {
        temperatures += get_cell_offset_in_stack

            (dimensions, get_source_layer_offset (stkel),
            first_row (dimensions), first_column (dimensions)) ;


        CellIndex_t row ;
        CellIndex_t column ;

        for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
        {
            for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
            {
                fprintf (stream, "%7.3f  ", *temperatures++) ;
            }

            fprintf (stream, "\n") ;
        } 
    }
}

/******************************************************************************/

void print_thermal_map_3D
(
    Dimensions_t    *dimensions,
    Temperature_t   *temperatures,
    Quantity_t      timestep_index,
    Quantity_t      slotlength,
    String_t        filename,
    OutputInstant_t instant 
)
{
       
    if (dimensions->NonUniform == 1)
    {
        // write the tmap vtk file in the non-uniform scenario
        char newname[50];
        strcpy(newname, filename);

        if (instant == TDICE_OUTPUT_INSTANT_SLOT)
        {
            timestep_index /= slotlength;
        }

        if (instant != TDICE_OUTPUT_INSTANT_FINAL)
        {
            // Determine the length of the string
            Quantity_t len = strlen(newname);
            // Remove the origin ".vtk"
            for (Quantity_t i = 0; i < len; i++)
            {
                if (newname[i] == '.')
                {
                    newname[i] = '\0';
                }
            }

            char index [10];
            // Convert the uint32_t to a string
            sprintf(index, "%u", timestep_index);

            strcat(newname, index);
            strcat(newname, ".vtk");
        }
       

        FILE *filevtk = fopen (newname, "w") ;
        if (filevtk == NULL)
        {
            fprintf (stderr, "Cannot create vtk file in the non-uniform scenario\n") ;
        }
        //output head information for vtk
        fprintf (filevtk, "# vtk DataFile Version 3.0\n");
        fprintf (filevtk, "3D Temperature Map\n");
        fprintf (filevtk, "ASCII\n");
        fprintf (filevtk, "DATASET UNSTRUCTURED_GRID\n");

        // Number of points (8 points per hexahedron)
        fprintf (filevtk, "POINTS %d float\n", dimensions->Grid.NCells * 8);

        CellIndex_t counter = 0;
        CellIndex_t num_elements = dimensions->Grid.NCells;

        for (Non_uniform_cellListNode_t* cell_i = dimensions->Cell_list.First; cell_i != NULL; cell_i = cell_i->Next)
        {
            // 8 corners of the hexahedron
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x, cell_i->Data.left_y, cell_i->Data.left_z);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x + cell_i->Data.length, cell_i->Data.left_y, cell_i->Data.left_z);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x + cell_i->Data.length, cell_i->Data.left_y + cell_i->Data.width, cell_i->Data.left_z);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x, cell_i->Data.left_y + cell_i->Data.width, cell_i->Data.left_z);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x, cell_i->Data.left_y, cell_i->Data.left_z + cell_i->Data.height);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x + cell_i->Data.length, cell_i->Data.left_y, cell_i->Data.left_z + cell_i->Data.height);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x + cell_i->Data.length, cell_i->Data.left_y + cell_i->Data.width, cell_i->Data.left_z + cell_i->Data.height);
            fprintf(filevtk, "%f %f %f\n", cell_i->Data.left_x, cell_i->Data.left_y + cell_i->Data.width, cell_i->Data.left_z + cell_i->Data.height);       
        }

        // Write the hexahedron elements (8 points per hexahedron)
        fprintf(filevtk, "CELLS %d %d\n", num_elements, num_elements * 9);
        for (CellIndex_t i = 0; i < num_elements; i++) 
        {
            fprintf(filevtk, "8 %d %d %d %d %d %d %d %d\n", 
                i * 8, i * 8 + 1, i * 8 + 2, i * 8 + 3,
                i * 8 + 4, i * 8 + 5, i * 8 + 6, i * 8 + 7);
        }

        // Cell types (12 corresponds to VTK_HEXAHEDRON)
        fprintf(filevtk, "CELL_TYPES %d\n", num_elements);
        for (CellIndex_t i = 0; i < num_elements; i++) 
        {
            fprintf(filevtk, "12\n");
        }

        // Temperature data (assigning values to each hexahedron)
        fprintf(filevtk, "CELL_DATA %d\n", num_elements);
        fprintf(filevtk, "SCALARS Temperature float 1\n");
        fprintf(filevtk, "LOOKUP_TABLE default\n");
        for (CellIndex_t i = 0; i < num_elements; i++) 
        {
            fprintf(filevtk, "%7.3f\n", *(temperatures+counter));
            counter++;
        }

         // Add labels based on material
        fprintf(filevtk, "SCALARS Labels int 1\n");
        fprintf(filevtk, "LOOKUP_TABLE default\n");
        for (Non_uniform_cellListNode_t* cell_i = dimensions->Cell_list.First; cell_i != NULL; cell_i = cell_i->Next)
        {
            String_t temp = "UNDERFILL";
            String_t temp1 = "AIR";
            if (cell_i->Data.Material.Id != NULL && (strcmp(cell_i->Data.Material.Id, temp) == 0 || strcmp(cell_i->Data.Material.Id, temp1) == 0))
            {
                fprintf(filevtk, "1\n");
            }            
            else
            {
                fprintf(filevtk, "0\n");
            }
        }
        fprintf (filevtk, "\n") ;
        fclose (filevtk) ;
    }

}

/******************************************************************************/

void stack_element_print_power_map
(
    StackElement_t  *stkel,
    Dimensions_t    *dimensions,
    Source_t        *sources,
    FILE            *stream
)
{
    
    if (dimensions->NonUniform == 1)
    {
        // write the xyaxis file in the non-uniform scenario
        char name[50] = "xyaxis_";
        strcat(name,stkel->Id);
        strcat(name,".txt");
        FILE *filexy = fopen (name, "w") ;
        if (filexy == NULL)
        {
            fprintf (stderr, "Cannot create text file for x axis in the non-uniform scenario\n") ;
            return ;
        }
        // output all of the cells' power in the layer for non-uniform scenario
        CellIndex_t counter = 0;
        CellIndex_t layer_offset = get_source_layer_offset(stkel) ;
        for (Non_uniform_cellListNode_t* cell_i = dimensions->Cell_list.First; cell_i != NULL; cell_i = cell_i->Next)
        {
            if (cell_i->Data.layer_info == layer_offset)
            {
                fprintf (stream, "%7.3f  ", *(sources+counter)) ;
                fprintf (filexy, "%5.2f\t%5.2f\t%5.2f\t%5.2f\n", cell_i->Data.left_x, cell_i->Data.left_y, cell_i->Data.length,cell_i->Data.width) ;
            }
            counter++;
        }
        fprintf (stream, "\n") ;
        fclose (filexy) ;
    }
    else
    {
        sources += get_cell_offset_in_stack

            (dimensions, get_source_layer_offset (stkel),
            first_row (dimensions), first_column (dimensions)) ;

        CellIndex_t row ;
        CellIndex_t column ;

        for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
        {
            for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
            {
                fprintf (stream, "%7.3f  ", *sources++) ;
            }

            fprintf (stream, "\n") ;
        }
    }
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_stack_element
(
    StackElement_t *stkel
)
{
    if (stkel->SEType == TDICE_STACK_ELEMENT_DIE)

        return get_number_of_floorplan_elements_floorplan

            (&stkel->Pointer.Die->Floorplan) ;

    else

        return 0u ;
}

/******************************************************************************/
