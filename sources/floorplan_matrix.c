/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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

void floorplan_matrix_init (FloorplanMatrix_t* flpmatrix)
{
    flpmatrix->ColumnPointers  = NULL ;
    flpmatrix->RowIndices      = NULL ;
    flpmatrix->Values          = NULL ;
    flpmatrix->NRows           = (CellIndex_t) 0u ;
    flpmatrix->NColumns        = (CellIndex_t) 0u ;
    flpmatrix->NNz             = (CellIndex_t) 0u ;
    flpmatrix->SLUMatrix.Store = NULL ;
}

/******************************************************************************/

void floorplan_matrix_copy (FloorplanMatrix_t *dst, FloorplanMatrix_t *src)
{
    floorplan_matrix_destroy (dst) ;

    if (src->NRows == 0u || src->NColumns == 0u || src->NNz == 0u)

        return ;

    floorplan_matrix_build (dst, src->NRows, src->NColumns, src->NNz) ;

    if (src->ColumnPointers != NULL)

        memcpy (dst->ColumnPointers, src->ColumnPointers,
                sizeof(CellIndex_t) * (src->NColumns + 1)) ;

    if (src->RowIndices != NULL)

        memcpy (dst->RowIndices, src->RowIndices,
                sizeof(CellIndex_t) * src->NNz) ;

    if (src->Values != NULL)

        memcpy (dst->Values, src->Values,
                sizeof(Source_t) * src->NNz) ;
}

/******************************************************************************/

Error_t floorplan_matrix_build
(
    FloorplanMatrix_t *flpmatrix,
    CellIndex_t        nrows,
    CellIndex_t        ncolumns,
    CellIndex_t        nnz
)
{
    flpmatrix->NRows    = nrows ;
    flpmatrix->NColumns = ncolumns ;
    flpmatrix->NNz      = nnz ;

    flpmatrix->RowIndices = (CellIndex_t *) malloc (sizeof(CellIndex_t) * nnz) ;

    if (flpmatrix->RowIndices == NULL)

        return TDICE_FAILURE ;

    flpmatrix->ColumnPointers = (CellIndex_t *) malloc (sizeof(CellIndex_t) * (ncolumns + 1)) ;

    if (flpmatrix->ColumnPointers == NULL)
    {
        free (flpmatrix->RowIndices) ;
        return TDICE_FAILURE ;
    }

    flpmatrix->Values = (Source_t *) malloc (sizeof(Source_t) * nnz) ;

    if (flpmatrix->Values == NULL)
    {
        free (flpmatrix->ColumnPointers) ;
        free (flpmatrix->RowIndices) ;
        return TDICE_FAILURE ;
    }

    dCreate_CompCol_Matrix

        (&flpmatrix->SLUMatrix, flpmatrix->NRows, flpmatrix->NColumns, flpmatrix->NNz,
         flpmatrix->Values, (int*) flpmatrix->RowIndices, (int*) flpmatrix->ColumnPointers,
         SLU_NC, SLU_D, SLU_GE) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void floorplan_matrix_destroy (FloorplanMatrix_t* flpmatrix)
{
    if (flpmatrix->ColumnPointers != NULL)

        free (flpmatrix->ColumnPointers) ;

    if (flpmatrix->RowIndices != NULL)

        free (flpmatrix->RowIndices) ;

    if (flpmatrix->Values != NULL)

        free (flpmatrix->Values) ;

    Destroy_SuperMatrix_Store (&flpmatrix->SLUMatrix) ;

    floorplan_matrix_init (flpmatrix) ;
}

/******************************************************************************/

void floorplan_matrix_fill
(
    FloorplanMatrix_t      *flpmatrix,
    FloorplanElementList_t *list,
    Dimensions_t           *dimensions
)
{
    CellIndex_t *c_pointers = flpmatrix->ColumnPointers ;
    CellIndex_t *r_indices  = flpmatrix->RowIndices ;
    Source_t    *values     = flpmatrix->Values ;

    *c_pointers++ = 0u ;

    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (list) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        *c_pointers = *(c_pointers - 1) ;

        ICElementListNode_t *iceln ;

        for (iceln  = ic_element_list_begin (&flpel->ICElements) ;
             iceln != NULL ;
             iceln  = ic_element_list_next (iceln))
        {
            ICElement_t *icel = ic_element_list_data (iceln) ;

            CellDimension_t width = 0u ;
            CellDimension_t y     = icel->SW_Y ;

            FOR_EVERY_IC_ELEMENT_ROW (row_index, icel)
            {
                if (row_index < icel->NE_Row)

                    width = get_cell_location_y (dimensions, row_index + 1) - y ;

                else

                    width = (icel->SW_Y + icel->Width) - y ;


                CellDimension_t length = 0u ;
                CellDimension_t x      = icel->SW_X ;

                FOR_EVERY_IC_ELEMENT_COLUMN (column_index, icel)
                {
                    *r_indices++ = get_cell_offset_in_layer

                                   (dimensions, row_index, column_index) ;

                    if (column_index < icel->NE_Column)

                        length = get_cell_location_x (dimensions, column_index + 1) - x ;

                    else

                        length = (icel->SW_X + icel->Length) - x ;

                    *values++ = (length * width) /  flpel->Area ;

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

void floorplan_matrix_multiply
(
    FloorplanMatrix_t *flpmatrix,
    Source_t          *x,
    Source_t          *b
)
{
    sp_dgemv((String_t)"N", 1.0, &flpmatrix->SLUMatrix, b, 1, 1.0, x, 1) ;
}

/******************************************************************************/

void floorplan_matrix_print (FloorplanMatrix_t flpmatrix, String_t file_name)
{
    FILE* file = fopen (file_name, "w") ;

    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", file_name) ;
        return ;
    }

    CellIndex_t row, column ;

    for (column = 0 ; column < flpmatrix.NColumns ; column++)

        for (row = flpmatrix.ColumnPointers[column] ;
             row < flpmatrix.ColumnPointers[column + 1] ;
             row++)

            fprintf (file, "%9d\t%9d\t%32.24f\n",
                flpmatrix.RowIndices[row] + 1, column + 1,
                flpmatrix.Values[row]) ;

    fclose (file) ;
}

/******************************************************************************/
