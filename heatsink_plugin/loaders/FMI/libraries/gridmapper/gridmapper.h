/***************************************************************************
 *   Copyright (C) 2014-2019 by Terraneo Federico                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#pragma once

#include "matrix.h"
#include <vector>
#include <ostream>

/**
 * A Grid is a matrix of cells of contiguous and uniform size placed somewhere
 * in a cartesian plane (not necessarily at the origin).
 */
class Grid
{
public:
    Grid(double x0, double y0, double x, double y, int Nr, int Nc)
        : x0(x0), y0(y0), x(x), y(y), Nr(Nr), Nc(Nc) {}
    
    double x0; ///< Bottom left corner X placing the grid in the plane
    double y0; ///< Bottom left corner Y placing the grid in the plane
    double x;  ///< Grid X size
    double y;  ///< Grid Y size
    int Nr;    ///< Number of rows    in the grid. Height of a cell is thus y/Nr
    int Nc;    ///< Number of columns in the grid. Width  of a cell is thus x/Nc
};

/**
 * Fragments arise out of the superposition of two grids.
 * When viewing this superposition, we distinguish the source and target grid.
 * Every element of the source grid is split into
 * - zero (if there are no corresponding target grid elements),
 * - one (if the target grid cell size is larger or equal and suitably aligned),
 * - or more (general case)
 * fragments.
 * 
 * Each fragment contains the following information:
 * - the row and column index of the target grid (the source grid is implicit)
 * - a generic feature which is scaled by the overlapping area
 */
class Fragment
{
public:
    Fragment() : r(-1), c(-1), feature(-1) {}
    Fragment(int r, int c, double feature) : r(r), c(c), feature(feature) {}
    
    int r,c;        /// Row and column into the target grid
    double feature; /// Feature
};

/**
 * Matrix of fragments. The matrix size is equal to the source grid, while the
 * indices in the fragments relate to the target grid.
 */
typedef Matrix<std::vector<Fragment>> FragmentGrid;

/**
 * \param source the source grid
 * \param target the target grid to be mapped into the source one
 * \param featurePerUnitArea the feature per unit area
 * \param swapRows
 * - if false, row 0 is the topmost row, like in a matrix, and increasing the
 *   row number we decrease the y coordinate.
 * - if true, row 0 is the bottommost row, and increasing the row number we
 *   increase the y coordinate.
 */
FragmentGrid mapGrids(Grid source, Grid target, double featurePerUnitArea, bool swapRows=false);

/**
 * Print a fragment grid
 */
std::ostream& operator<<(std::ostream& os, const FragmentGrid& fg);
