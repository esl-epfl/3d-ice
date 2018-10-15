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

#include "power_reader.h"
#include <sstream>
#include <stdexcept>

using namespace std;

//
// class FilePowerData
//

FilePowerData::FilePowerData(const string& filename) : file(filename.c_str())
{
    if(!file.good()) throw runtime_error("Power file not found");
    file.exceptions(ios::badbit);
    string line;
    getline(file,line);
    stringstream ss(line);
    int i=0;
    while(ss>>line) indices[line]=i++;
}

map<string,int> FilePowerData::getIndices(const list<string>& names)
{
    map<string,int> result;
    for(auto it : names) result[it]=indices.at(it);
    return result;
}

vector<double>& FilePowerData::getValues()
{
    if(values.empty()) advanceIn(); //If getValues() is called before advanceIn
    return values;
}

void FilePowerData::advanceIn()
{
    if(file.eof()) throw runtime_error("End of file");
    string line;
    getline(file,line);
    if(line.empty()) throw runtime_error("End of file");
    stringstream ss(line);
    //First time only, note that values.empty() is used as a flag in getValues()
    if(values.empty()) values.resize(indices.size(),0.0);
    unsigned int i=0;
    double d;
    while(ss>>d) values.at(i++)=d;
    if(i!=indices.size()) throw logic_error("Power file line size mismatch");
}
