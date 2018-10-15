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

#include "temperature_writer.h"
#include <sstream>
#include <stdexcept>
#include <cmath>

using namespace std;

//
// class FilePowerData
//

FileTemperatureData::FileTemperatureData(const string& filename)
    : file(filename.c_str())
{
    if(!file.good()) throw runtime_error("Can't create temperature file");
    file.exceptions(ios::badbit);
}

void FileTemperatureData::setIndices(const map<string,int>& indices)
{
    temperature.resize(indices.size(),NAN);
    
    vector<string> temp;
    temp.resize(indices.size());
    for(auto it : indices) temp.at(it.second)=it.first;
    for(auto it : temp) file<<it<<"\t";
    file<<endl;
}

void FileTemperatureData::setValues(const vector<double>& values)
{
    if(values.size()!=temperature.size()) throw logic_error("values mismatch");
    for(unsigned int i=0;i<temperature.size();i++)
    {
        if(values.at(i)<0.0) continue;
        if(!std::isnan(temperature.at(i)))
            throw logic_error("A temperature value is computed more than once");
        temperature.at(i)=values.at(i);
    }
}

void FileTemperatureData::advanceOut()
{
    for(auto it : temperature) file<<(it-273.15)<<"\t";
    file<<endl;
    fill(begin(temperature),end(temperature),NAN);
}
