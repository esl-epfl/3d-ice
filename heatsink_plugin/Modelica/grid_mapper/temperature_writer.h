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

#ifndef TEMPERATURE_WRITER_H
#define TEMPERATURE_WRITER_H

#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <map>

/**
 * Base class from which classes receiving temperature data from the simulator
 * derive
 */
class TemperatureData
{
public:
    /**
     * Set the *full* list of names of functional units, together with their
     * index into the temperature trace. Note that in case of a 3D chip or
     * split floorplan this must be the list of all functional units.
     * \param list of functional units
     */
    virtual void setIndices(const std::map<std::string,int>& indices)=0;
    
    /**
     * Set the temperature values for some or all the functional units. In
     * case of a 2D chip this will be the list of all functional units,
     * in case of a 3D chip multiple calls will set part of the functional
     * units. In the latter case, indices related to functional units whose
     * temperature is not computed must contain negative numbers.
     * After all the temperatures have been computed, you must call
     * advance() to consolidate the temperature data into the output, be it
     * a file or socket
     * \param values computed temperature values.
     */
    virtual void setValues(const std::vector<double>& values)=0;

    /**
     * Advance to the next time sample, so that the data provided via
     * setValues() is output.
     */
    virtual void advanceOut()=0;
    
    virtual ~TemperatureData() {}
};

/**
 * Class that dumps temperature data to a trace file
 */
class FileTemperatureData : public TemperatureData
{
public:
    FileTemperatureData(const std::string& filename);
    
    virtual void setIndices(const std::map<std::string,int>& indices);
    
    virtual void setValues(const std::vector<double>& values);
    
    virtual void advanceOut();
    
private:
    std::ofstream file;
    std::vector<double> temperature;
};

#endif //TEMPERATURE_WRITER_H
