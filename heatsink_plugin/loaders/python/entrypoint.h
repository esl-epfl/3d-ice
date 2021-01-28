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
 *                             Copyright (C) 2017                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Federico Terraneo                                                 *
 *          Arvind Sridhar                                                    *
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

#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include <chrono>
#include <stdexcept>

// This is the public interface that a heatsink plugin has to
// implement. Functions need to have C linkage to be callable
// from 3D-ICE
extern "C" {
int heatsink_init(unsigned int nrows, unsigned int ncols,
                  double cellwidth,   double celllength,
                  double initialtemperature,
                  double spreaderconductance,
                  double timestep,
                  const char *args);
int heatsink_simulate_step(const double *spreadertemperatures,
                                 double *heatflow);
}

// Comment out to disable bound checking in CellMatrix
// #define CELLMATRIX_BOUND_CHECK

/**
 * A simple non-owning wrapper over a double* to turn it into a more
 * understandable representation. 3D-ICE stores information about cells
 * in an matrix layout, where the cell (0,0) is at the top left.
 */
class CellMatrix
{
public:
    CellMatrix(double *dataPtr, unsigned int nRows, unsigned int nCols)
        : dataPtr(dataPtr), nRows(nRows), nCols(nCols) {}
    
    double& at(unsigned int row, unsigned int col)
    {
        #ifdef CELLMATRIX_BOUND_CHECK
        if(row>nRows || col>nCols)
            throw std::range_error("CellMatrix out of bounds");
        #endif //CELLMATRIX_BOUND_CHECK
        return dataPtr[row*nCols+col];
    }
    
    double at(unsigned int row, unsigned int col) const
    {
        #ifdef CELLMATRIX_BOUND_CHECK
        if(row>nRows || col>nCols)
            throw std::range_error("CellMatrix out of bounds");
        #endif //CELLMATRIX_BOUND_CHECK
        return dataPtr[row*nCols+col];
    }
    
    double *data() { return dataPtr; }
    
    const double *data() const { return dataPtr; }
    
    unsigned int rows() const { return nRows; }
    
    unsigned int cols() const { return nCols; }
    
private:
    double *dataPtr; // Non-owning pointer
    unsigned int nRows, nCols;
};

// Comment out to silence profiler
#define PRINT_PROFILER_STATS

class Profiler
{
public:
    Profiler();
    
    void enterPlugin();
    
    void exitPlugin();
    
    void printStats();
    
    ~Profiler();
    
private:
    std::chrono::high_resolution_clock::time_point last;
    std::chrono::nanoseconds mainTime;
    std::chrono::nanoseconds pluginTime;
};

class ProfileFunction
{
public:
    ProfileFunction(Profiler& p) : p(p) { p.enterPlugin(); }
    ~ProfileFunction()                  { p.exitPlugin();  }
private:
    Profiler& p;
};

#endif //ENTRYPOINT_H
