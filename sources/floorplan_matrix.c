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

#include <stdlib.h>

#include "floorplan_matrix.h"
#include "macros.h"

/******************************************************************************/

void init_floorplan_matrix (FloorplanMatrix_t* this)
{
    this->ColumnPointers  = NULL ;
    this->RowIndices      = NULL ;
    this->Values          = NULL ;
    this->NRows           = 0u ;
    this->NColumns        = 0u ;
    this->NNz             = 0u ;
    this->SLUMatrix.Store = NULL ;
}

/******************************************************************************/

Error_t build_floorplan_matrix
(
    FloorplanMatrix_t *this,
    CellIndex_t        nrows,
    CellIndex_t        ncolumns,
    CellIndex_t        nnz
)
{
    this->NRows    = nrows ;
    this->NColumns = ncolumns ;
    this->NNz      = nnz ;

    this->RowIndices = (CellIndex_t *) malloc (sizeof(CellIndex_t) * nnz) ;

    if (this->RowIndices == NULL)

        return TDICE_FAILURE ;

    this->ColumnPointers = (CellIndex_t *) malloc (sizeof(CellIndex_t) * (ncolumns + 1)) ;

    if (this->ColumnPointers == NULL)
    {
        FREE_POINTER (free, this->RowIndices) ;
        return TDICE_FAILURE ;
    }

    this->Values = (Source_t *) malloc (sizeof(Source_t) * nnz) ;

    if (this->Values == NULL)
    {
        FREE_POINTER (free, this->ColumnPointers) ;
        FREE_POINTER (free, this->RowIndices) ;
        return TDICE_FAILURE ;
    }

    dCreate_CompCol_Matrix

        (&this->SLUMatrix, this->NRows, this->NColumns, this->NNz,
         this->Values, (int*) this->RowIndices, (int*) this->ColumnPointers,
         SLU_NC, SLU_D, SLU_GE) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void destroy_floorplan_matrix (FloorplanMatrix_t* this)
{
    FREE_POINTER (free, this->ColumnPointers) ;
    FREE_POINTER (free, this->RowIndices) ;
    FREE_POINTER (free, this->Values) ;

    Destroy_SuperMatrix_Store (&this->SLUMatrix) ;
}

/******************************************************************************/

void fill_floorplan_matrix
(
    FloorplanMatrix_t  *this,
    FloorplanElement_t *list,
    Dimensions_t       *dimensions
)
{
    CellIndex_t *c_pointers = this->ColumnPointers ;
    CellIndex_t *r_indices  = this->RowIndices ;
    Source_t    *values     = this->Values ;

    *c_pointers++ = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement_t, flp_el, list)
    {
        *c_pointers = *(c_pointers - 1) ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT

        (ICElement_t, ic_el, flp_el->ICElementsList)
        {
            CellDimension_t width = 0u ;
            CellDimension_t y     = ic_el->SW_Y ;

            FOR_EVERY_IC_ELEMENT_ROW (row_index, ic_el)
            {
                if (row_index < ic_el->NE_Row)

                    width = get_cell_location_y (dimensions, row_index + 1) - y ;

                else

                    width = (ic_el->SW_Y + ic_el->Width) - y ;


                CellDimension_t length = 0u ;
                CellDimension_t x      = ic_el->SW_X ;

                FOR_EVERY_IC_ELEMENT_COLUMN (column_index, ic_el)
                {
                    *r_indices++ = get_cell_offset_in_layer

                                   (dimensions, row_index, column_index) ;

                    if (column_index < ic_el->NE_Column)

                        length = get_cell_location_x (dimensions, column_index + 1) - x ;

                    else

                        length = (ic_el->SW_X + ic_el->Length) - x ;

                    *values++ = (length * width) /  flp_el->Area ;

                    (*c_pointers)++ ;

                    x += length ;
                }

                y += width ;
            }
        }

        c_pointers++ ;
    }
}

/******************************************************************************/

void multiply_floorplan_matrix
(
    FloorplanMatrix_t *this,
    Source_t          *x,
    Source_t          *b
)
{
    sp_dgemv("N", 1.0, &this->SLUMatrix, b, 1, 1.0, x, 1) ;
}

/******************************************************************************/

void print_floorplan_matrix (FloorplanMatrix_t this, String_t file_name)
{
    FILE* file = fopen (file_name, "w") ;

    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", file_name) ;
        return ;
    }

    CellIndex_t row, column ;

    for (column = 0 ; column < this.NColumns ; column++)

        for (row = this.ColumnPointers[column] ;
             row < this.ColumnPointers[column + 1] ;
             row++)

            fprintf (file, "%9d\t%9d\t%32.24f\n",
                this.RowIndices[row] + 1, column + 1,
                this.Values[row]) ;

    fclose (file) ;
}

/******************************************************************************/
