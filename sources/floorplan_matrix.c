/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#include <stdlib.h> // For the memory functions malloc/free
#include <string.h> // For the memory function memcpy

#include "floorplan_matrix.h"

/******************************************************************************/

void floorplan_matrix_init (FloorplanMatrix_t* flpmatrix)
{
    flpmatrix->ColumnPointers  = NULL ;
    flpmatrix->RowIndices      = NULL ;
    flpmatrix->Values          = NULL ;
    flpmatrix->NRows           = (CellIndex_t) 0u ;
    flpmatrix->NColumns        = (CellIndex_t) 0u ;
    flpmatrix->NNz             = (CellIndex_t) 0u ;
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

            CellDimension_t width = 0.0 ;
            CellDimension_t y     = icel->SW_Y ;

            CellIndex_t row ;
            CellIndex_t column ;

            for (row = icel->SW_Row ; row <= icel->NE_Row ; row++)
            {
                if (row < icel->NE_Row)

                    width = get_cell_location_y (dimensions, row + 1) - y ;

                else

                    width = (icel->SW_Y + icel->Width) - y ;


                CellDimension_t length = 0.0 ;
                CellDimension_t x      = icel->SW_X ;

                for (column = icel->SW_Column ; column <= icel->NE_Column ; column++)
                {
                    *r_indices++ = get_cell_offset_in_layer

                                   (dimensions, row, column) ;

                    if (column < icel->NE_Column)

                        length = get_cell_location_x (dimensions, column + 1) - x ;

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
    CellIndex_t j, i;

    if (   flpmatrix->NRows    == 0 || flpmatrix->RowIndices     == NULL
        || flpmatrix->NColumns == 0 || flpmatrix->ColumnPointers == NULL
        || flpmatrix->NNz      == 0 || flpmatrix->Values         == NULL)
    {
        fprintf (stderr, "matrix multiply error: matrix unset\n") ;

        return ;
    }

    for (j = 0; j < flpmatrix->NColumns; ++j)

    if (b[j] != 0.)

    for (i = flpmatrix->ColumnPointers[j] ;
         i < flpmatrix->ColumnPointers[j+1] ; ++i)

    x [ flpmatrix->RowIndices [i] ] += b[j] * flpmatrix -> Values [i] ;

    return ;
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
