/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.7 .                               *
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
 *                             Copyright (C) 2020                             *
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

#include <stdexcept>
#include <iostream>
#include "fmiwrapper.h"

using namespace std;

//
// class FmiWrapper
//

FmiWrapper::FmiWrapper(unsigned int nRows, unsigned int nCols,
                       double cellWidth,   double cellLength,
                       double initialTemperature,
                       double spreaderConductance,
                       double timeStep,
                       const string& args)
   : fmi(getName(args),getPath(args),LogLevel::Normal), timeStep(timeStep)
{
    fmi.setScalarDouble(fmi.variableIndex("initialTemperature"),initialTemperature);
    fmi.setScalarDouble(fmi.variableIndex("spreaderConductance"),spreaderConductance);
    fmi.setScalarString(fmi.variableIndex("args"),args);
    
    temperatureIndices=fmi.variableIndex("port.T");
    heatFlowIndices=fmi.variableIndex("port.Q_flow");
    
    fmi.startSimulation();
}

void FmiWrapper::simulateStep(const CellMatrix spreaderTemperatures,
                                    CellMatrix heatFlow)
{
    if(spreaderTemperatures.rows()!=1 || spreaderTemperatures.cols()!=1) throw runtime_error("TODO: extend");
    
    
    
    fmi.doStep(time,timeStep);
    time+=timeStep;
}

string FmiWrapper::getName(const string& args)
{
    string pathModel=args.substr(0,args.find_first_of(" "));
    auto modelBegin=args.find_last_of("/");
    if(modelBegin==string::npos) return pathModel;
    else return pathModel.substr(modelBegin+1);
}

string FmiWrapper::getPath(const string& args)
{
    string pathModel=args.substr(0,args.find_first_of(" "));
    auto modelBegin=args.find_last_of("/");
    if(modelBegin==string::npos) return "";
    else return pathModel.substr(0,modelBegin);
}
