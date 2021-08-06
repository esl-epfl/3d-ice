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
 *          Federico Terraneo                                                 *
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

#include <stdio.h> // For the file type FILE

#include "thermal_data.h"
#include "macros.h"
#include "connection_list.h"

/******************************************************************************/

static void init_data (double* data, uint32_t size, double init_value)
{
    while (size--) *data++ = init_value ;
}

/******************************************************************************/

void thermal_data_init (ThermalData_t *tdata)
{
    tdata->Size = (CellIndex_t) 0u ;

    tdata->Temperatures = NULL ;

    thermal_grid_init  (&tdata->ThermalGrid) ;
    power_grid_init    (&tdata->PowerGrid) ;
    system_matrix_init (&tdata->SM_A) ;

    tdata->SLUMatrix_B.Store = NULL ;
}

/******************************************************************************/
// update the numver of cells in non-uniform grid scenario
void update_number_of_cells (Dimensions_t *dimensions, StackElementList_t *stack_elements_list)
{
    CellIndex_t cell_num_non_uniform = 0;
    CellIndex_t cell_num_die;
    CellIndex_t discr_x_element = 0;
    CellIndex_t discr_y_element = 0;
    // enumerate all the stack elements (dies)
    StackElementListNode_t *stkeln ;
    for (stkeln  = stack_elements_list->First ;
        stkeln != NULL ;
        stkeln  = stkeln->Next)
    {
        StackElement_t *stkel = &stkeln->Data ;
        switch (stkel->SEType)
        {
            case TDICE_STACK_ELEMENT_DIE:
            {
                CellIndex_t discr_x_die = stkel->Pointer.Die->Discr_X;
                CellIndex_t discr_y_die = stkel->Pointer.Die->Discr_Y;

                // enumerate all the floorplan elements
                FloorplanElementListNode_t *ele_flp ;
                cell_num_die = 0;
                for (ele_flp  = stkel->Pointer.Die->Floorplan.ElementsList.First ;
                    ele_flp != NULL ;
                    ele_flp  = ele_flp->Next)
                {
                    FloorplanElement_t *ele_flpi = &ele_flp->Data ;
                    discr_x_element = ele_flpi->ICElements.First->Data.Discr_X;
                    discr_y_element = ele_flpi->ICElements.First->Data.Discr_Y;
                    if (discr_x_element != 0 && discr_y_element != 0)
                    {
                        cell_num_die += discr_x_element*discr_y_element;
                    }
                    else
                    {
                        cell_num_die += discr_x_die*discr_y_die;
                        ele_flpi->ICElements.First->Data.Discr_X = discr_x_die;
                        ele_flpi->ICElements.First->Data.Discr_Y = discr_y_die;
                    }
                }
                cell_num_die = cell_num_die*stkel->NLayers;
                cell_num_non_uniform += cell_num_die;
                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER:
            {
                CellIndex_t discr_x = stkel->Pointer.Layer->Discr_X;
                CellIndex_t discr_y = stkel->Pointer.Layer->Discr_Y;
                cell_num_non_uniform += discr_x*discr_y;
                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL:
            {
                cell_num_non_uniform += stkel->NLayers * dimensions->Grid.NRows * dimensions->Grid.NColumns;
                break ;
            }
            case TDICE_STACK_ELEMENT_NONE :
            {
                fprintf (stderr, "Unsupported stack element type\n") ;
                break ;
            }
            default :
            {
                fprintf (stderr, "Unsupported stack element type\n") ;
                break ;
            }
        }
        // if(stkel->SEType!=TDICE_STACK_ELEMENT_DIE)
        // {
        //     continue;
        // }

        // Total cell number in all the layers the die has
    }
    dimensions->Grid.NCells = cell_num_non_uniform;
}

/******************************************************************************/
// get cell position for each cell and sace info to arrays position_info and layer_cell_record
void get_cell_position(ChipDimension_t (*position_info)[4], CellIndex_t *layer_cell_record, CellIndex_t *layer_type_record, StackElementList_t *stack_elements_list, Dimensions_t* dimensions)
{
    CellIndex_t current_layer = 0;
    CellIndex_t cell_num_non_uniform = 0;
    CellIndex_t cell_num_layer;
    // enumerate all the stack elements (dies)
    StackElementListNode_t *stkeln ;
    for (stkeln  = stack_elements_list->Last ;
        stkeln != NULL ;
        stkeln  = stkeln->Prev)
    {
        StackElement_t *stkel = &stkeln->Data ;
        // Darong_TODO: support more layer types
        // default discretization level for the die
        switch (stkel->SEType)
        {
            case TDICE_STACK_ELEMENT_DIE:
            {
                CellIndex_t discr_x_element = 0;
                CellIndex_t discr_y_element = 0;
                CellIndex_t discr_x_die = stkel->Pointer.Die->Discr_X;
                CellIndex_t discr_y_die = stkel->Pointer.Die->Discr_Y;
                // enumerate all the layers in the die
                CellIndex_t total_layer_number_die = stkel->Pointer.Die->NLayers;
                for (CellIndex_t layer_index = 0; layer_index< total_layer_number_die; layer_index++)
                {
                    // enumerate all the floorplan elements
                    FloorplanElementListNode_t *ele_flp ;
                    for (ele_flp  = stkel->Pointer.Die->Floorplan.ElementsList.First ;
                        ele_flp != NULL ;
                        ele_flp  = ele_flp->Next)
                    {
                        FloorplanElement_t *ele_flpi = &ele_flp->Data ;
                        discr_x_element = ele_flpi->ICElements.First->Data.Discr_X;
                        discr_y_element = ele_flpi->ICElements.First->Data.Discr_Y;
                        // take default discretization level if the floorplan unit does not define the disretization level
                        if (discr_x_element == 0 || discr_y_element == 0)
                        {
                            discr_x_element = discr_x_die;
                            discr_y_element = discr_y_die;
                        }
                        cell_num_layer = discr_x_element*discr_y_element;

                        // fill position info
                        // generate the thermal grid first from left to right (x->), bottom to up (y->) for each floorplan element
                        ChipDimension_t ori_element_x = ele_flpi->ICElements.First->Data.SW_X;
                        ChipDimension_t ori_element_y = ele_flpi->ICElements.First->Data.SW_Y;
                        ChipDimension_t ori_element_length = ele_flpi->ICElements.First->Data.Length;
                        ChipDimension_t ori_element_width = ele_flpi->ICElements.First->Data.Width;
                        CellIndex_t position_info_index;
                        CellIndex_t discr_x_position;
                        CellIndex_t discr_y_position;
                        for (CellIndex_t sub_element = 0; sub_element < cell_num_layer; sub_element++)
                        {
                            position_info_index = sub_element+cell_num_non_uniform;
                            discr_x_position = sub_element % discr_x_element;
                            discr_y_position = sub_element / discr_x_element;
                            // left corner coordinate (left_x, left_y)
                            position_info[position_info_index][0] = ori_element_x + (ori_element_length/discr_x_element)*discr_x_position;
                            position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y_element)*discr_y_position;
                            // right corner coordinate (right_x, right_y)
                            position_info[position_info_index][2] = ori_element_x + (ori_element_length/discr_x_element)*(discr_x_position + 1);
                            position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y_element)*(discr_y_position + 1);
                            Non_uniform_cell_t new_cell;
                            non_uniform_cell_init(&new_cell);
                            new_cell.layer_info =  current_layer;
                            new_cell.left_x = position_info[position_info_index][0] ;
                            new_cell.left_y = position_info[position_info_index][1] ;
                            new_cell.length = position_info[position_info_index][2] - position_info[position_info_index][0];
                            new_cell.width = position_info[position_info_index][3] - position_info[position_info_index][1];
                            non_uniform_cell_list_insert_end(&dimensions->Cell_list, &new_cell);    
                        }

                        cell_num_non_uniform += cell_num_layer;

                        // In the non-uniform scenario, NE_Row and NE_Column are used to record start and end index of the element
                        if (layer_index == stkel->Pointer.Die->SourceLayerOffset)
                        {
                            ele_flpi->ICElements.First->Data.NE_Row = cell_num_non_uniform-cell_num_layer;
                            ele_flpi->ICElements.First->Data.NE_Column = cell_num_non_uniform-1;
                        }
                    }
                    // record the end index of cell number in the layer
                    layer_cell_record[current_layer] = cell_num_non_uniform;
                    layer_type_record[current_layer] = 0;
                    current_layer++;
                }

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER:
            {
                CellIndex_t discr_x = stkel->Pointer.Layer->Discr_X;
                CellIndex_t discr_y = stkel->Pointer.Layer->Discr_Y;
                cell_num_layer = discr_x*discr_y;

                CellIndex_t position_info_index;
                CellIndex_t discr_x_position;
                CellIndex_t discr_y_position;
                ChipDimension_t ori_element_x = 0.0;
                ChipDimension_t ori_element_y = 0.0;
                ChipDimension_t ori_element_length = dimensions->Chip.Length;
                ChipDimension_t ori_element_width = dimensions->Chip.Width;
                for (CellIndex_t sub_element = 0; sub_element < cell_num_layer; sub_element++)
                {
                    position_info_index = sub_element+cell_num_non_uniform;
                    discr_x_position = sub_element % discr_x;
                    discr_y_position = sub_element / discr_x;
                    // left corner coordinate (left_x, left_y)
                    position_info[position_info_index][0] = ori_element_x + (ori_element_length/discr_x)*discr_x_position;
                    position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                    // right corner coordinate (right_x, right_y)
                    position_info[position_info_index][2] = ori_element_x + (ori_element_length/discr_x)*(discr_x_position + 1);
                    position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                    Non_uniform_cell_t new_cell;
                    non_uniform_cell_init(&new_cell);
                    new_cell.layer_info =  current_layer;
                    new_cell.left_x = position_info[position_info_index][0] ;
                    new_cell.left_y = position_info[position_info_index][1] ;
                    new_cell.length = position_info[position_info_index][2] - position_info[position_info_index][0];
                    new_cell.width = position_info[position_info_index][3] - position_info[position_info_index][1];
                    non_uniform_cell_list_insert_end(&dimensions->Cell_list, &new_cell);    
                }

                cell_num_non_uniform += cell_num_layer;
                layer_cell_record[current_layer] = cell_num_non_uniform;
                layer_type_record[current_layer] = 0;
                current_layer++;
                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL:
            {
                if (stkel->Pointer.Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
                {
                    CellIndex_t discr_x = dimensions->Grid.NColumns;
                    CellIndex_t discr_y = dimensions->Grid.NRows;
                    cell_num_layer = discr_x*discr_y;
                    
                    CellIndex_t isChannel = 0;
                    CellIndex_t position_info_index;
                    CellIndex_t discr_x_position;
                    CellIndex_t discr_y_position;
                    // ChipDimension_t ori_element_x = 0.0;
                    ChipDimension_t ori_element_y = 0.0;
                    // ChipDimension_t ori_element_length = dimensions->Chip.Length;
                    ChipDimension_t ori_element_width = dimensions->Chip.Width;
                    for (CellIndex_t layer_i = 0; layer_i < stkel->Pointer.Channel->NLayers; layer_i++)
                    {
                        if (layer_i == 2)
                            isChannel = 1;
                        else
                            isChannel = 0;

                        for (CellIndex_t sub_element = 0; sub_element < cell_num_layer; sub_element++)
                        {

                            position_info_index = sub_element+cell_num_non_uniform;
                            discr_x_position = sub_element % discr_x;
                            discr_y_position = sub_element / discr_x;
                            if (discr_x_position == 0) //first column (wall)
                            {
                                // left corner coordinate (left_x, left_y)
                                position_info[position_info_index][0] = 0.0;
                                position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                // right corner coordinate (right_x, right_y)
                                position_info[position_info_index][2] = dimensions->Cell.FirstWallLength;
                                position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                            }
                            else if (discr_x_position == discr_x-1) //last column (wall)
                            {
                                // left corner coordinate (left_x, left_y)
                                position_info[position_info_index][0] = dimensions->Chip.Length - dimensions->Cell.LastWallLength;
                                position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                // right corner coordinate (right_x, right_y)
                                position_info[position_info_index][2] = dimensions->Chip.Length;
                                position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                            }
                            else
                            {
                                if (discr_x_position % 2 != 0)  //channel position
                                {
                                    // left corner coordinate (left_x, left_y)
                                    CellIndex_t index_channel = (discr_x_position-1)/2;
                                    position_info[position_info_index][0] = dimensions->Cell.FirstWallLength + index_channel*(dimensions->Cell.WallLength+dimensions->Cell.ChannelLength);
                                    position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                    // right corner coordinate (right_x, right_y)
                                    position_info[position_info_index][2] = position_info[position_info_index][0] + dimensions->Cell.ChannelLength;;
                                    position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);

                                }
                                else                        //wall
                                {
                                    // left corner coordinate (left_x, left_y)
                                    CellIndex_t index_wall = (discr_x_position-2)/2;
                                    position_info[position_info_index][0] = (dimensions->Cell.FirstWallLength + dimensions->Cell.ChannelLength) + index_wall*(dimensions->Cell.WallLength+dimensions->Cell.ChannelLength);
                                    position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                    // right corner coordinate (right_x, right_y)
                                    position_info[position_info_index][2] = position_info[position_info_index][0] + dimensions->Cell.WallLength;
                                    position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                                }

                            }

                            Non_uniform_cell_t new_cell;
                            non_uniform_cell_init(&new_cell);
                            new_cell.layer_info =  current_layer;
                            new_cell.left_x = position_info[position_info_index][0] ;
                            new_cell.left_y = position_info[position_info_index][1] ;
                            new_cell.length = position_info[position_info_index][2] - position_info[position_info_index][0];
                            new_cell.width = position_info[position_info_index][3] - position_info[position_info_index][1];
                            if (isChannel)
                                new_cell.isChannel = 1;
                            non_uniform_cell_list_insert_end(&dimensions->Cell_list, &new_cell);    
                        }

                        cell_num_non_uniform += cell_num_layer;
                        layer_cell_record[current_layer] = cell_num_non_uniform;
                        if (layer_i == 0 || layer_i == 3)
                            layer_type_record[current_layer] = 1;
                        else
                            layer_type_record[current_layer] = 2;
                        current_layer++;
                    }

                    break ;
                }
                else if (stkel->Pointer.Channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE || stkel->Pointer.Channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
                {
                    CellIndex_t discr_x = dimensions->Grid.NColumns;
                    CellIndex_t discr_y = dimensions->Grid.NRows;
                    cell_num_layer = discr_x*discr_y;
                    
                    CellIndex_t isChannel = 0;
                    CellIndex_t position_info_index;
                    CellIndex_t discr_x_position;
                    CellIndex_t discr_y_position;
                    // ChipDimension_t ori_element_x = 0.0;
                    ChipDimension_t ori_element_y = 0.0;
                    // ChipDimension_t ori_element_length = dimensions->Chip.Length;
                    ChipDimension_t ori_element_width = dimensions->Chip.Width;
                    for (CellIndex_t layer_i = 0; layer_i < stkel->Pointer.Channel->NLayers; layer_i++)
                    {
                        if (layer_i == 2)
                            isChannel = 1;
                        else
                            isChannel = 0;

                        for (CellIndex_t sub_element = 0; sub_element < cell_num_layer; sub_element++)
                        {

                            position_info_index = sub_element+cell_num_non_uniform;
                            discr_x_position = sub_element % discr_x;
                            discr_y_position = sub_element / discr_x;
                            if (discr_x_position == 0) //first column (wall)
                            {
                                // left corner coordinate (left_x, left_y)
                                position_info[position_info_index][0] = 0.0;
                                position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                // right corner coordinate (right_x, right_y)
                                position_info[position_info_index][2] = dimensions->Cell.FirstWallLength;
                                position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                            }
                            else if (discr_x_position == discr_x-1) //last column (wall)
                            {
                                // left corner coordinate (left_x, left_y)
                                position_info[position_info_index][0] = dimensions->Chip.Length - dimensions->Cell.LastWallLength;
                                position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                // right corner coordinate (right_x, right_y)
                                position_info[position_info_index][2] = dimensions->Chip.Length;
                                position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                            }
                            else
                            {
                                if (discr_x_position % 2 != 0)  //channel position
                                {
                                    // left corner coordinate (left_x, left_y)
                                    CellIndex_t index_channel = (discr_x_position-1)/2;
                                    position_info[position_info_index][0] = dimensions->Cell.FirstWallLength + index_channel*(dimensions->Cell.WallLength+dimensions->Cell.ChannelLength);
                                    position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                    // right corner coordinate (right_x, right_y)
                                    position_info[position_info_index][2] = position_info[position_info_index][0] + dimensions->Cell.ChannelLength;;
                                    position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);

                                }
                                else                        //wall
                                {
                                    // left corner coordinate (left_x, left_y)
                                    CellIndex_t index_wall = (discr_x_position-2)/2;
                                    position_info[position_info_index][0] = (dimensions->Cell.FirstWallLength + dimensions->Cell.ChannelLength) + index_wall*(dimensions->Cell.WallLength+dimensions->Cell.ChannelLength);
                                    position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                    // right corner coordinate (right_x, right_y)
                                    position_info[position_info_index][2] = position_info[position_info_index][0] + dimensions->Cell.WallLength;
                                    position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                                }

                            }

                            Non_uniform_cell_t new_cell;
                            non_uniform_cell_init(&new_cell);
                            new_cell.layer_info =  current_layer;
                            new_cell.left_x = position_info[position_info_index][0] ;
                            new_cell.left_y = position_info[position_info_index][1] ;
                            new_cell.length = position_info[position_info_index][2] - position_info[position_info_index][0];
                            new_cell.width = position_info[position_info_index][3] - position_info[position_info_index][1];
                            if (isChannel)
                                new_cell.isChannel = 1;
                            non_uniform_cell_list_insert_end(&dimensions->Cell_list, &new_cell);    
                        }

                        cell_num_non_uniform += cell_num_layer;
                        layer_cell_record[current_layer] = cell_num_non_uniform;
                        if (layer_i == 0 || layer_i == 3)
                        {
                            layer_type_record[current_layer] = 1;
                        }
                        else if (layer_i == 1)
                        {
                            layer_type_record[current_layer] = 3;
                        }
                        else
                        {
                            layer_type_record[current_layer] = 2;
                        }
                        current_layer++;
                    }

                    break ;
                }
                else
                {
                    CellIndex_t discr_x = dimensions->Grid.NColumns;
                    CellIndex_t discr_y = dimensions->Grid.NRows;
                    cell_num_layer = discr_x*discr_y;
                    
                    CellIndex_t isChannel = 0;
                    CellIndex_t position_info_index;
                    CellIndex_t discr_x_position;
                    CellIndex_t discr_y_position;
                    // ChipDimension_t ori_element_x = 0.0;
                    ChipDimension_t ori_element_y = 0.0;
                    // ChipDimension_t ori_element_length = dimensions->Chip.Length;
                    ChipDimension_t ori_element_width = dimensions->Chip.Width;
                    for (CellIndex_t sub_element = 0; sub_element < cell_num_layer; sub_element++)
                    {
                        position_info_index = sub_element+cell_num_non_uniform;
                        discr_x_position = sub_element % discr_x;
                        discr_y_position = sub_element / discr_x;
                        if (discr_x_position == 0) //first column (wall)
                        {
                            isChannel = 0; //first wall
                            // left corner coordinate (left_x, left_y)
                            position_info[position_info_index][0] = 0.0;
                            position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                            // right corner coordinate (right_x, right_y)
                            position_info[position_info_index][2] = dimensions->Cell.FirstWallLength;
                            position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                        }
                        else if (discr_x_position == discr_x-1) //last column (wall)
                        {
                            isChannel = 0; //last wall
                            // left corner coordinate (left_x, left_y)
                            position_info[position_info_index][0] = dimensions->Chip.Length - dimensions->Cell.LastWallLength;
                            position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                            // right corner coordinate (right_x, right_y)
                            position_info[position_info_index][2] = dimensions->Chip.Length;
                            position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                        }
                        else
                        {
                            if (discr_x_position % 2 != 0)  //channel
                            {
                                isChannel = 1;
                                // left corner coordinate (left_x, left_y)
                                CellIndex_t index_channel = (discr_x_position-1)/2;
                                position_info[position_info_index][0] = dimensions->Cell.FirstWallLength + index_channel*(dimensions->Cell.WallLength+dimensions->Cell.ChannelLength);
                                position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                // right corner coordinate (right_x, right_y)
                                position_info[position_info_index][2] = position_info[position_info_index][0] + dimensions->Cell.ChannelLength;;
                                position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);

                            }
                            else                        //wall
                            {
                                isChannel = 0;
                                // left corner coordinate (left_x, left_y)
                                CellIndex_t index_wall = (discr_x_position-2)/2;
                                position_info[position_info_index][0] = (dimensions->Cell.FirstWallLength + dimensions->Cell.ChannelLength) + index_wall*(dimensions->Cell.WallLength+dimensions->Cell.ChannelLength);
                                position_info[position_info_index][1] = ori_element_y + (ori_element_width/discr_y)*discr_y_position;
                                // right corner coordinate (right_x, right_y)
                                position_info[position_info_index][2] = position_info[position_info_index][0] + dimensions->Cell.WallLength;
                                position_info[position_info_index][3] = ori_element_y + (ori_element_width/discr_y)*(discr_y_position + 1);
                            }

                        }

                        Non_uniform_cell_t new_cell;
                        non_uniform_cell_init(&new_cell);
                        new_cell.layer_info =  current_layer;
                        new_cell.left_x = position_info[position_info_index][0] ;
                        new_cell.left_y = position_info[position_info_index][1] ;
                        new_cell.length = position_info[position_info_index][2] - position_info[position_info_index][0];
                        new_cell.width = position_info[position_info_index][3] - position_info[position_info_index][1];
                        if (isChannel)
                            new_cell.isChannel = 1;
                        non_uniform_cell_list_insert_end(&dimensions->Cell_list, &new_cell);    
                    }

                    cell_num_non_uniform += cell_num_layer;
                    layer_cell_record[current_layer] = cell_num_non_uniform;
                    layer_type_record[current_layer] = 0;
                    current_layer++;
                    break ;
                }

            }
            case TDICE_STACK_ELEMENT_NONE :
            {
                fprintf (stderr, "Unsupported stack element type\n") ;
                break ;
            }
            default :
            {
                fprintf (stderr, "Unsupported stack element type\n") ;
                break ;
            }
        }
        // if(stkel->SEType!=TDICE_STACK_ELEMENT_DIE)
        // {
        //     continue;
        // }
    }
}


/******************************************************************************/
// Get the Minkowski difference between two cells
void get_minkowski_difference(ChipDimension_t *minkowski_diff, ChipDimension_t (*position_info)[4], int i_x, int i_y)
{
    minkowski_diff[0] = position_info[i_x][0] - position_info[i_y][2];
    minkowski_diff[1] = position_info[i_x][1] - position_info[i_y][3];
    minkowski_diff[2] = position_info[i_x][2] - position_info[i_y][0];
    minkowski_diff[3] = position_info[i_x][3] - position_info[i_y][1];
}

/******************************************************************************/
// get connections of each grid in the same layer
void get_connections_in_layer
(
    CellIndex_t* layer_cell_record,
    CellIndex_t* layer_type_record,
    ChipDimension_t (*position_info_ptr)[4], 
    Dimensions_t* dimensions
)
{
    ConnectionList_t* connections_list = &dimensions->connections_list;
    CellIndex_t layer_start_index = 0;
    ChipDimension_t minkowski_diff[4];
    CellIndex_t layer_end_index;
    CellIndex_t layer_type;
    for (CellIndex_t layer_index = 0; layer_index < dimensions->Grid.NLayers; layer_index++)
    {
        layer_end_index = layer_cell_record[layer_index];
        layer_type =  layer_type_record[layer_index];
        for (CellIndex_t i_x = layer_start_index; i_x < layer_end_index; i_x++)
        {
            for (CellIndex_t i_y = i_x+1; i_y < layer_end_index; i_y++)
            {
                // a simplified GJK algorithm to detect interconnect cells in a single layer
                // first compute the Minkowski difference
                get_minkowski_difference(minkowski_diff, position_info_ptr, i_x, i_y);
                // if two rectangles interconnect with each other, Minkowski difference should contain the origin point (0, 0)
                if (minkowski_diff[0] == 0 || minkowski_diff[1] == 0 || minkowski_diff[2] == 0 || minkowski_diff[3] == 0)
                {
                    // furthermore, it should cross the origin point
                    if (minkowski_diff[0] * minkowski_diff[2] + minkowski_diff[1] * minkowski_diff[3] < 0)
                    {
                        Connection_t new_connection;
                        connection_init(&new_connection);
                        new_connection.node1 = i_x;
                        new_connection.node2 = i_y;
                        new_connection.node1_layer = layer_index;
                        new_connection.node2_layer = layer_index;
                        // Find the interconnect length
                        if (minkowski_diff[0] * minkowski_diff[2] == 0)
                        {
                            if (layer_type == 1 || layer_type == 2 || layer_type == 3)
                                continue;
                            new_connection.value = (fabs(minkowski_diff[1])<=fabs(minkowski_diff[3])) ? fabs(minkowski_diff[1]) : fabs(minkowski_diff[3]);
                            new_connection.direction = 1; //two nodes interconect in direction x
                        }
                        else if (minkowski_diff[1] * minkowski_diff[3] == 0)
                        {
                            if (layer_type == 1 || layer_type == 3)
                                continue;
                            new_connection.value = (fabs(minkowski_diff[0])<=fabs(minkowski_diff[2])) ? fabs(minkowski_diff[0]) : fabs(minkowski_diff[2]);
                            new_connection.direction = 2; //two nodes interconect in direction y
                        }
                        else
                            fprintf (stderr, "Cannot determine interconnect length\n") ;
                        
                        // printf("Node%d in layer %d <-> Node%d in layer %d\n", new_connection.node1, new_connection.node1_layer, new_connection.node2, new_connection.node2_layer) ;
                        // printf("Direction %d, Value %f\n", new_connection.direction, new_connection.value) ;
                        // add the connection information to the connections variable
                        connection_list_insert_end(connections_list, &new_connection);
                        
                    }
                }
            }
        }
        layer_start_index = layer_end_index;
    }
}

/******************************************************************************/
// get connections between layers (bottom->top)
void get_connections_between_layer
(
    CellIndex_t* layer_cell_record, 
    CellIndex_t* layer_type_record,
    ChipDimension_t (*position_info_ptr)[4], 
    Dimensions_t* dimensions
)
{
    ConnectionList_t* connections_list = &dimensions->connections_list;
    // First define the information between the bottom layer and its upper layer
    CellIndex_t botom_layer_start_index = 0;
    ChipDimension_t minkowski_diff[4];
    CellIndex_t botom_layer_end_index;
    CellIndex_t top_layer_start_index;
    CellIndex_t top_layer_end_index;
    CellIndex_t bottom_layer_type;
    CellIndex_t top_layer_type;
    // enumerate all the layers
    for (CellIndex_t layer_index = 1; layer_index < dimensions->Grid.NLayers; layer_index++)
    {
        bottom_layer_type = layer_type_record[layer_index-1];
        top_layer_type = layer_type_record[layer_index];
        if ( (bottom_layer_type == 2 && top_layer_type == 2) || (bottom_layer_type == 3 && top_layer_type == 2))
        {
            botom_layer_start_index = layer_cell_record[layer_index-3];
            botom_layer_end_index = layer_cell_record[layer_index-2];
            top_layer_start_index = layer_cell_record[layer_index-1];
            top_layer_end_index = layer_cell_record[layer_index];
            
            // enumerate all the elements in a bottom layer
            for (CellIndex_t i_x = botom_layer_start_index; i_x < botom_layer_end_index; i_x++)
            {
                // enumerate all the elements in an upper layer
                for (CellIndex_t i_y = top_layer_start_index; i_y < top_layer_end_index; i_y++)
                {
                    // first compute Minkowski difference
                    get_minkowski_difference(minkowski_diff, position_info_ptr, i_x, i_y);
                    // Minkowski difference should contain the origin point (0, 0) if two cells have overlap area
                    if (minkowski_diff[0] * minkowski_diff[2] < 0 && minkowski_diff[1] * minkowski_diff[3] < 0)
                    {
                        // add the connection information to the connections variable
                        Connection_t new_connection;
                        connection_init(&new_connection);
                        new_connection.node1 = i_x;
                        new_connection.node2 = i_y;
                        new_connection.node1_layer = layer_index-1;
                        new_connection.node2_layer = layer_index;

                        // overlap area is the minum area
                        ChipDimension_t area1 = minkowski_diff[0] * minkowski_diff[1];
                        ChipDimension_t area2 = minkowski_diff[2] * minkowski_diff[3];
                        ChipDimension_t area3 = fabs(minkowski_diff[0] * minkowski_diff[3]);
                        ChipDimension_t area4 = fabs(minkowski_diff[1] * minkowski_diff[2]);
                        new_connection.value = (area1 <= area2) ? area1 : area2;
                        new_connection.value = (area3 < new_connection.value) ? area3 : new_connection.value;
                        new_connection.value = (area4 < new_connection.value) ? area4 : new_connection.value;

                        new_connection.direction = 0; //connect direction is Z(=0) for two nodes in different layers;
                        // printf("Node%d in layer %d <-> Node%d in layer %d\n", new_connection.node1, new_connection.node1_layer, new_connection.node2, new_connection.node2_layer) ;
                        // printf("Direction %d, Value %f\n", new_connection.direction, new_connection.value) ;
                        // add the connection information to the connections variable
                        connection_list_insert_end(connections_list, &new_connection);
                    }
                }
            }

            botom_layer_start_index = layer_cell_record[layer_index-2];
            botom_layer_end_index = layer_cell_record[layer_index-1];
            top_layer_start_index = layer_cell_record[layer_index];
            top_layer_end_index = layer_cell_record[layer_index+1];
            
            // enumerate all the elements in a bottom layer
            for (CellIndex_t i_x = botom_layer_start_index; i_x < botom_layer_end_index; i_x++)
            {
                // enumerate all the elements in an upper layer
                for (CellIndex_t i_y = top_layer_start_index; i_y < top_layer_end_index; i_y++)
                {
                    // first compute Minkowski difference
                    get_minkowski_difference(minkowski_diff, position_info_ptr, i_x, i_y);
                    // Minkowski difference should contain the origin point (0, 0) if two cells have overlap area
                    if (minkowski_diff[0] * minkowski_diff[2] < 0 && minkowski_diff[1] * minkowski_diff[3] < 0)
                    {
                        // add the connection information to the connections variable
                        Connection_t new_connection;
                        connection_init(&new_connection);
                        new_connection.node1 = i_x;
                        new_connection.node2 = i_y;
                        new_connection.node1_layer = layer_index-1;
                        new_connection.node2_layer = layer_index;

                        // overlap area is the minum area
                        ChipDimension_t area1 = minkowski_diff[0] * minkowski_diff[1];
                        ChipDimension_t area2 = minkowski_diff[2] * minkowski_diff[3];
                        ChipDimension_t area3 = fabs(minkowski_diff[0] * minkowski_diff[3]);
                        ChipDimension_t area4 = fabs(minkowski_diff[1] * minkowski_diff[2]);
                        new_connection.value = (area1 <= area2) ? area1 : area2;
                        new_connection.value = (area3 < new_connection.value) ? area3 : new_connection.value;
                        new_connection.value = (area4 < new_connection.value) ? area4 : new_connection.value;

                        new_connection.direction = 0; //connect direction is Z(=0) for two nodes in different layers;
                        // printf("Node%d in layer %d <-> Node%d in layer %d\n", new_connection.node1, new_connection.node1_layer, new_connection.node2, new_connection.node2_layer) ;
                        // printf("Direction %d, Value %f\n", new_connection.direction, new_connection.value) ;
                        // add the connection information to the connections variable
                        connection_list_insert_end(connections_list, &new_connection);
                    }
                }
            }
            botom_layer_start_index = botom_layer_end_index;
        }
        else
        {
            botom_layer_end_index = layer_cell_record[layer_index-1];
            top_layer_start_index = layer_cell_record[layer_index-1];
            top_layer_end_index = layer_cell_record[layer_index];
            
            // enumerate all the elements in a bottom layer
            for (CellIndex_t i_x = botom_layer_start_index; i_x < botom_layer_end_index; i_x++)
            {
                // enumerate all the elements in an upper layer
                for (CellIndex_t i_y = top_layer_start_index; i_y < top_layer_end_index; i_y++)
                {
                    // first compute Minkowski difference
                    get_minkowski_difference(minkowski_diff, position_info_ptr, i_x, i_y);
                    // Minkowski difference should contain the origin point (0, 0) if two cells have overlap area
                    if (minkowski_diff[0] * minkowski_diff[2] < 0 && minkowski_diff[1] * minkowski_diff[3] < 0)
                    {
                        // add the connection information to the connections variable
                        Connection_t new_connection;
                        connection_init(&new_connection);
                        new_connection.node1 = i_x;
                        new_connection.node2 = i_y;
                        new_connection.node1_layer = layer_index-1;
                        new_connection.node2_layer = layer_index;

                        // overlap area is the minum area
                        ChipDimension_t area1 = minkowski_diff[0] * minkowski_diff[1];
                        ChipDimension_t area2 = minkowski_diff[2] * minkowski_diff[3];
                        ChipDimension_t area3 = fabs(minkowski_diff[0] * minkowski_diff[3]);
                        ChipDimension_t area4 = fabs(minkowski_diff[1] * minkowski_diff[2]);
                        new_connection.value = (area1 <= area2) ? area1 : area2;
                        new_connection.value = (area3 < new_connection.value) ? area3 : new_connection.value;
                        new_connection.value = (area4 < new_connection.value) ? area4 : new_connection.value;

                        new_connection.direction = 0; //connect direction is Z(=0) for two nodes in different layers;
                        // printf("Node%d in layer %d <-> Node%d in layer %d\n", new_connection.node1, new_connection.node1_layer, new_connection.node2, new_connection.node2_layer) ;
                        // printf("Direction %d, Value %f\n", new_connection.direction, new_connection.value) ;
                        // add the connection information to the connections variable
                        connection_list_insert_end(connections_list, &new_connection);
                    }
                }
            }
            botom_layer_start_index = top_layer_start_index;
        }    

    }
}

/******************************************************************************/

Error_t thermal_data_build
(
    ThermalData_t      *tdata,
    StackElementList_t *stack_elements_list,
    Dimensions_t       *dimensions,
    Analysis_t         *analysis
)
{
    Error_t result ;
    
    // re-evaluate the number of thermal grids
    // update number of connections in non-uniform grid scenario
    if (dimensions->NonUniform == 1)
    {
        update_number_of_cells (dimensions, stack_elements_list);
        
        ChipDimension_t position_info[dimensions->Grid.NCells][4]; // position info contains "left_x, left_y, right_x, right_y" for each thermal cell
        ChipDimension_t (*position_info_ptr)[4] = position_info;
        CellIndex_t layer_cell_record[dimensions->Grid.NLayers]; // record the end index of each layer in the position_info

        // 0: caculate surroding
        // 1: TDICE_LAYER_BOTTOM_WALL, TDICE_LAYER_TOP_WALL, : they don't have connections in x and y directions
        // 2: TDICE_LAYER_VWALL_CHANNEL (2RM), TDICE_LAYER_CHANNEL_2RM: they don't have connections in the x direction
        // 3: TDICE_LAYER_VWALL_CHANNEL (pin) : it doesn't have connections in x and y directions
        CellIndex_t layer_type_record[dimensions->Grid.NLayers];

        // get cell position for each cell and sace info to arrays position_info and layer_cell_record
        get_cell_position(position_info_ptr, layer_cell_record, layer_type_record, stack_elements_list, dimensions);

        // // initalize connections variable to store connections info
        // ConnectionList_t connections_list;
        // connection_list_init(&connections_list);
        
        // get connections of each grid in the same layer
        // printf("\n Node1 Node2 Number \n");
        get_connections_in_layer(layer_cell_record, layer_type_record, position_info_ptr, dimensions);
        // get connections between layers (bottom->top)
        get_connections_between_layer(layer_cell_record, layer_type_record, position_info_ptr, dimensions);
        // update number of connections
        dimensions->Grid.NConnections =  2*(dimensions->connections_list.Size)+dimensions->Grid.NCells;
    }

    tdata->Size = get_number_of_cells (dimensions) ;

    /* Alloc and set temperatures */

    tdata->Temperatures =

        (Temperature_t*) malloc (sizeof(Temperature_t) * tdata->Size) ;

    if (tdata->Temperatures == NULL)
    {
        fprintf (stderr, "Cannot malloc temperature array\n") ;

        return TDICE_FAILURE ;
    }

    /* Set Temperatures to the initial thermal state and builds SLU vector B */

    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;

    dCreate_Dense_Matrix  /* Vector B */

        (&tdata->SLUMatrix_B, tdata->Size, 1,
         tdata->Temperatures, tdata->Size,
         SLU_DN, SLU_D, SLU_GE) ;

    /* Alloc and fill the thermal grid */

    result = thermal_grid_build (&tdata->ThermalGrid, dimensions) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr, "Cannot malloc thermal grid\n") ;

        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        return TDICE_FAILURE ;
    }

    result = thermal_grid_fill (&tdata->ThermalGrid, stack_elements_list) ;
    
    if (result == TDICE_FAILURE)
    {
        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        return TDICE_FAILURE ;
    }

    /* Alloc and fill the power grid */

    result = power_grid_build (&tdata->PowerGrid, dimensions) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr, "Cannot malloc power grid\n") ;

        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        thermal_grid_destroy (&tdata->ThermalGrid) ;

        return TDICE_FAILURE ;
    }

    power_grid_fill

        (&tdata->PowerGrid, &tdata->ThermalGrid, stack_elements_list, dimensions) ;


    /* Alloc and fill the system matrix and builds the SLU wrapper */
    result = system_matrix_build

        (&tdata->SM_A, tdata->Size, get_number_of_connections (dimensions)) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr, "Cannot malloc syatem matrix\n") ;

        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        thermal_grid_destroy (&tdata->ThermalGrid) ;
        power_grid_destroy   (&tdata->PowerGrid) ;

        return TDICE_FAILURE ;
    }

    fill_system_matrix

        (&tdata->SM_A, &tdata->ThermalGrid, analysis, dimensions) ;

    result = do_factorization (&tdata->SM_A) ;

    if (result == TDICE_FAILURE)
    {
        thermal_data_destroy (tdata) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void thermal_data_destroy (ThermalData_t *tdata)
{
    free (tdata->Temperatures) ;

    thermal_grid_destroy (&tdata->ThermalGrid) ;
    power_grid_destroy   (&tdata->PowerGrid) ;

    system_matrix_destroy (&tdata->SM_A) ;

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

    thermal_data_init (tdata) ;
}

/******************************************************************************/

void reset_thermal_state (ThermalData_t *tdata, Analysis_t *analysis)
{
    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;
}

/******************************************************************************/

static void fill_system_vector
(
    Dimensions_t  *dimensions,
    HeatSink_t    *topSink,
    double        *vector,
    Source_t      *sources,
    Capacity_t    *capacities,
    Temperature_t *temperatures,
    Time_t         step_time
)
{
#ifdef PRINT_SYSTEM_VECTOR
    Temperature_t old ;
#endif
    if (dimensions->NonUniform == 1)
    {
        for (CellIndex_t i = 0; i<dimensions->Cell_list.Size; i++)
            *vector++ =   *sources++
                          + (*capacities++ / step_time)
                          * *temperatures++ ;
    }
    else
    {
        CellIndex_t layer ;
        CellIndex_t row ;
        CellIndex_t column ;

        for (layer = first_layer (dimensions) ; layer <= last_layer (dimensions) ; layer++)
        {
            for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
            {
                for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
                {

    #ifdef PRINT_SYSTEM_VECTOR
                    old = *temperatures ;
    #endif

                    *vector++ = *sources++
                                + (*capacities++ / step_time)
                                * *temperatures++ ;

    #ifdef PRINT_SYSTEM_VECTOR
                    fprintf (stderr,
                        " l %2d r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",
                        layer, row, column,
                        get_cell_offset_in_stack (dimensions, layer, row, column),
                        *(vector-1), *(sources-1), *(capacities-1), old) ;
    #endif

                } // FOR_EVERY_COLUMN
            } // FOR_EVERY_ROW
        } // FOR_EVERY_LAYER
    
        // Copy the rest of the vector
        if(topSink && topSink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)
        {
            for(row = 0; row < topSink->NRows; row++)
            {
                for(column = 0; column < topSink->NColumns; column++)
                {
    #ifdef PRINT_SYSTEM_VECTOR
                    old = *temperatures ;
    #endif

                    *vector++ = *sources++
                                + (*capacities++ / step_time)
                                * *temperatures++ ;

    #ifdef PRINT_SYSTEM_VECTOR
                    fprintf (stderr,
                        "      r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",
                        row, column,
                        get_spreader_cell_offset (dimensions, topSink, row, column),
                        *(vector-1), *(sources-1), *(capacities-1), old) ;
    #endif
                }
            }
        }
    }
}

/******************************************************************************/

static void fill_system_vector_steady
(
    Dimensions_t *dimensions,
    double       *vector,
    Source_t     *sources
)
{
    CellIndex_t layer ;
    CellIndex_t row ;
    CellIndex_t column ;

    if (dimensions->NonUniform == 1)
    {
        for (CellIndex_t i = 0; i<dimensions->Cell_list.Size; i++)
            *vector++ =   *sources++ ;
    }
    else
    {
        for (layer = first_layer (dimensions) ; layer <= last_layer (dimensions) ; layer++)
        {
            for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
            {
                for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
                {
                    *vector++ =   *sources++ ;

                    #ifdef PRINT_SYSTEM_VECTOR
                                    fprintf (stderr,
                                        " l %2d r %4d c %4d [%7d] | %e [b] = %e [s]\n",
                                        layer, row, column,
                                        get_cell_offset_in_stack (dimensions, layer, row, column),
                                        *(vector-1), *(sources-1)) ;
                    #endif

                } // FOR_EVERY_COLUMN
            } // FOR_EVERY_ROW
        } // FOR_EVERY_LAYER
    }

}

Error_t pluggable_heatsink(ThermalData_t *tdata, Dimensions_t *dimensions)
{
    // We have something to do only if we're using the pluggable heatsink model
    HeatSink_t *sink = tdata->ThermalGrid.TopHeatSink;
    if(sink == NULL || sink->SinkModel != TDICE_HEATSINK_TOP_PLUGGABLE)
            return TDICE_SUCCESS;
    
    //Get a pointer to the spreader temperatures
    double *SpreaderTemperatures = tdata->Temperatures;
    SpreaderTemperatures += get_spreader_cell_offset(dimensions,sink,0,0);
    
    Source_t *sources = tdata->PowerGrid.Sources;
    sources += get_spreader_cell_offset(dimensions,sink,0,0);
    
    // Call the pluggable heat sink function to compute the heat flows to
    // the heatsink
    if(sink->PluggableHeatsink(SpreaderTemperatures,sources))
    {
        fprintf(stderr, "Error: pluggable heatsink callback failed\n");
        return TDICE_FAILURE;
    }
    
    // Both 3D-ICE and plugin use passive sign convention
    unsigned int size = sink->NColumns * sink->NRows;
    unsigned int i;
    for(i = 0; i < size; i++)
        sources[i] = - sources[i];

    return TDICE_SUCCESS;
}

/******************************************************************************/

SimResult_t emulate_step
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)

        return TDICE_WRONG_CONFIG ;

    if (slot_completed (analysis) == true)
    {
        Error_t result = update_source_vector (&tdata->PowerGrid, dimensions) ;

        if (result == TDICE_FAILURE)

            return TDICE_END_OF_SIMULATION ;
    }
    
    if(pluggable_heatsink(tdata, dimensions) == TDICE_FAILURE)
        return TDICE_SOLVER_ERROR ;

    fill_system_vector

        (dimensions, tdata->ThermalGrid.TopHeatSink, tdata->Temperatures, tdata->PowerGrid.Sources,
         tdata->PowerGrid.CellsCapacities, tdata->Temperatures, analysis->StepTime) ;

    Error_t res = solve_sparse_linear_system (&tdata->SM_A, &tdata->SLUMatrix_B) ;

    if (res != TDICE_SUCCESS)

        return TDICE_SOLVER_ERROR ;

    increase_by_step_time (analysis) ;

    if (slot_completed (analysis) == false)

        return TDICE_STEP_DONE ;

    else

        return TDICE_SLOT_DONE ;
}

/******************************************************************************/

SimResult_t emulate_slot
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis
)
{
    SimResult_t result ;

    do
    {

        result = emulate_step (tdata, dimensions, analysis) ;

    }   while (result == TDICE_STEP_DONE) ;

    return result ;
}

/******************************************************************************/

SimResult_t emulate_steady
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_STEADY)

        return TDICE_WRONG_CONFIG ;

    if(tdata->ThermalGrid.TopHeatSink &&
       tdata->ThermalGrid.TopHeatSink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)

        return TDICE_SOLVER_ERROR ; //TODO: support steady state pluggable sink

    Error_t result = update_source_vector (&tdata->PowerGrid, dimensions) ;
    printf("sources info:\n");
    for(CellIndex_t i = 0; i < dimensions->Grid.NCells; i++)
        printf("%d:\t%f\n", i, *(tdata->PowerGrid.Sources+i));
    if (result == TDICE_FAILURE)
    {
        fprintf (stderr,

            "Warning: no power trace given for steady state simulation\n") ;

        return TDICE_END_OF_SIMULATION ;
    }

    fill_system_vector_steady (dimensions, tdata->Temperatures, tdata->PowerGrid.Sources) ;
    printf("system matrix info:\n");
    for(CellIndex_t i = 0; i < dimensions->Grid.NConnections; i++)
        printf("%d:\t%f\n", i, *(tdata->SM_A.Values+i));
    Error_t res = solve_sparse_linear_system (&tdata->SM_A, &tdata->SLUMatrix_B) ;

    if (res != TDICE_SUCCESS)

        return TDICE_SOLVER_ERROR ;

    return TDICE_END_OF_SIMULATION ;
}

/******************************************************************************/

Error_t update_coolant_flow_rate
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis,
    CoolantFR_t     new_flow_rate
)
{
    tdata->ThermalGrid.Channel->Coolant.FlowRate =

        FLOW_RATE_FROM_MLMIN_TO_UM3SEC(new_flow_rate) ;

    // TODO replace with "update"

    fill_system_matrix (&tdata->SM_A, &tdata->ThermalGrid, analysis, dimensions) ;

    if (do_factorization (&tdata->SM_A) == TDICE_FAILURE)

        return TDICE_FAILURE ;

    update_channel_sources (&tdata->PowerGrid, dimensions) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Temperature_t get_cell_temperature
(
    ThermalData_t *tdata,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    CellIndex_t id = get_cell_offset_in_stack

                     (dimensions, layer_index, row_index, column_index) ;

    if (id >= get_number_of_cells (dimensions))

        return 0.0 ;

    else

        return *(tdata->Temperatures + id) ;
}

/******************************************************************************/

Error_t print_thermal_map
(
    ThermalData_t      *tdata,
    StackElementList_t *list,
    Dimensions_t       *dimensions,
    String_t            stack_element_id,
    String_t            file_name
)
{
    StackElement_t stkel ;

    stack_element_init (&stkel) ;

    string_copy (&stkel.Id, &stack_element_id) ;

    StackElement_t *tmp = stack_element_list_find (list, &stkel) ;

    if (tmp == NULL)
    {
        stack_element_destroy (&stkel) ;

        return TDICE_FAILURE ;
    }

    stack_element_destroy (&stkel) ;

    FILE *output_file = fopen (file_name, "w") ;

    if (output_file == NULL)
    {
        fprintf (stderr, "Unable to open output file %s\n", file_name) ;

        return TDICE_FAILURE ;
    }

    stack_element_print_thermal_map

        (tmp, dimensions, tdata->Temperatures, output_file) ;

    fclose (output_file) ;

    return TDICE_SUCCESS ;
}
