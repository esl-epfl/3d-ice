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
    this->ColumnPointers = NULL ;
    this->RowIndices     = NULL ;
    this->Values         = NULL ;
    this->NRows          = 0u ;
    this->NColumns       = 0u ;
    this->NNz            = 0u ;
}

/******************************************************************************/

Error_t alloc_floorplan_matrix
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

    this->Values = (float *) malloc (sizeof(float) * nnz) ;

    if (this->Values == NULL)
    {
        FREE_POINTER (free, this->ColumnPointers) ;
        FREE_POINTER (free, this->RowIndices) ;
        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void free_floorplan_matrix (FloorplanMatrix_t* this)
{
    FREE_POINTER (free, this->ColumnPointers) ;
    FREE_POINTER (free, this->RowIndices) ;
    FREE_POINTER (free, this->Values) ;
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
