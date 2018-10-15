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

#ifndef POWER_READER_H
#define POWER_READER_H

#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>

/**
 * Base class from which classes providing power data to the simulator derive
 */
class PowerData
{
public:
    /**
     * \param names a list of names coming from the floorplan file that are
     * required. The list can coincide with the full list of names in the power
     * trace in the case of a 2D chip, or be a subset of it in a 3D chip where
     * there are multiple floorplans.
     * \return a map that maps the requested names into indices into the vector
     * returned by getValues()
     */
    virtual std::map<std::string,int> getIndices(const std::list<std::string>& names)=0;

    /**
     * \return all indices found in the temperature source
     */
    virtual std::map<std::string,int> getAllIndices()=0;

    /**
     * \return the full number of functional units
     */
    virtual int getNumberOfFunctionalUnits()=0;
    
    /**
     * \return a vector with the power values for each functional unit.
     * This function keeps returning the very same value until advance() is
     * called, at which point it returns the next time sample.
     * Throws on end of stream.
     */
    virtual std::vector<double>& getValues()=0;
  
    /**
     * Advance to the next time sample, so that getValues() returns the new
     * power data. Throws on end of stream.
     */
    virtual void advanceIn()=0;

    virtual ~PowerData() {}
};

/**
 * Class that provides power data from a trace file
 */
class FilePowerData : public PowerData
{
public:
    FilePowerData(const std::string& filename);
    
    virtual std::map<std::string,int> getIndices(const std::list<std::string>& names);

    virtual std::map<std::string,int> getAllIndices() { return indices; }
    
    virtual int getNumberOfFunctionalUnits() { return indices.size(); }
    
    virtual std::vector<double>& getValues();
    
    virtual void advanceIn();
private:
    std::ifstream file;
    std::map<std::string,int> indices;
    std::vector<double> values;
};

#endif //POWER_READER_H
