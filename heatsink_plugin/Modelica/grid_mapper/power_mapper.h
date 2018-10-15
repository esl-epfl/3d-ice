/***************************************************************************
 *   Copyright (C) 2014 by Terraneo Federico                               *
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

#ifndef POWER_MAPPER_H
#define POWER_MAPPER_H

#include "power_reader.h"
#include "temperature_writer.h"
#include "tilelib.h"
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <map>
#include <tuple>

/**
 * How to backport temperature data from the grid model to the block model.
 * If defined, consider only the grid cell in the exact center of the functional
 * unit block. Otherwise do a weighted average of the temperatures of the grid
 * cells that intersect the block, proportional to their area.
 */
//#define CENTER_CELL_ONLY

/**
 * Dump block to grid mapping table for power and grid to block mapping for
 * temperatures, for debugging purposes
 */
#define DUMP_MAPPING_TABLES

template<typename T>
class Matrix
{
public:
    Matrix(int r, int c) : r(r), c(c), d(new T[r*c]), owner(true) {}
    
    Matrix(T* d, int r, int c) : r(r), c(c), d(d), owner(false) {}
    
    int row() const { return r; }
    
    int col() const { return c; }
    
    T& at(int r, int c)
    {
        int R=this->r;
        int C=this->c;
        if(r<0 || c<0 || r>=R || c>=C)
            throw std::range_error("Index out of range");
        return d[r*C+c];
    }
    
    const T& at(int r, int c) const
    {
        int R=this->r;
        int C=this->c;
        if(r<0 || c<0 || r>=R || c>=C)
            throw std::range_error("Index out of range");
        return d[r*C+c];
    }
    
    Matrix(const Matrix& rhs) : r(rhs.r), c(rhs.c), d(new T[r*c])
    {
        memcpy(d,rhs.d,r*c*sizeof(T));
    }
    
    Matrix& operator=(const Matrix& rhs)
    {
        if(r!=rhs.r || c!=rhs.c) throw std::range_error("Size mismatch");
        memcpy(d,rhs.d,r*c*sizeof(T));
    }
    
    ~Matrix() { if(owner) delete[] d; }
private:
    const int r,c;
    T *d;
    bool owner;
};

typedef Matrix<double> ModelicaMatrixDouble;

class PowerMapper
{
public:
    PowerMapper(std::shared_ptr<PowerData> pd,
                std::shared_ptr<TemperatureData> td,
                const std::string& flpfile, int gridsize) : pd(pd), td(td),
                flpfile(flpfile), gridsize(gridsize) {}
    
    virtual void getPowerGrid(ModelicaMatrixDouble& mmd)=0;
    virtual void setTemperatureGrid(const ModelicaMatrixDouble& mmd)=0;
    
    double getxChipSize() const { return xChipSize; }
    double getyChipSize() const { return yChipSize; }
    double getxGridPitch() const { return xGridPitch; }
    double getyGridPitch() const { return yGridPitch; }
    int getGridSize() const { return gridsize; }
    
    virtual ~PowerMapper() {}

protected:
    std::shared_ptr<PowerData> pd;
    std::shared_ptr<TemperatureData> td;
    std::string flpfile;
    
    double xChipSize,yChipSize,xGridPitch,yGridPitch;
    int gridsize;
};

class AreaProportionalPowerMapper : public PowerMapper
{
public:
    AreaProportionalPowerMapper(std::shared_ptr<PowerData> pd,
            std::shared_ptr<TemperatureData> td, const std::string& flpfile,
            int gridsize);
    
    void getPowerGrid(ModelicaMatrixDouble& mmd);
    void setTemperatureGrid(const ModelicaMatrixDouble& mmd);
    
private:
    
    void precomputePowerMappingCoefficients(const Tile& chip,
        const std::map<std::string,int>& indices);
    
    void precomputeTemperatureMappingCoefficients(const Tile& chip,
        const std::map<std::string,int>& indices);
    
    //A square of the output grid is made of one or more fragments of the
    //input Components. The int is the component index in the power file,
    //while the double is the area-proportional coefficient, which sums to one
    typedef std::vector<std::pair<int,double>> Fragments;
    
    typedef Matrix<Fragments> FragmentGrid;
    
    FragmentGrid fragmentGrid;
    
    #ifdef CENTER_CELL_ONLY
    //A block maps to a grid cell only
    std::vector<std::pair<int,int>> backwardMapping;
    #else //CENTER_CELL_ONLY
    //A block is made of one or more fragments of grid cells. The two int are
    //row and column, while the double is the area-proportional coefficient,
    //which sums to one
    typedef std::vector<std::tuple<int,int,double>> BackwardFragments;
    std::vector<BackwardFragments> backwardFragmentList;
    #endif //CENTER_CELL_ONLY
    int numBlocks;
};

#endif //POWER_MAPPER_H
