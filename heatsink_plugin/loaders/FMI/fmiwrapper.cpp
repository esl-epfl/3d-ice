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
#include <cstring>
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
   : fmi(getName(args),getPath(args),LogLevel::Normal), timeStep(timeStep),
     spreaderMapping(nullptr,0,0), sinkMapping(nullptr,0,0),
     sinkTemperatures(nullptr,0,0), sinkHeatFlows(nullptr,0,0)
{
    // Set FMI parameters coming from 3D-ICE
    fmi.setScalarString(fmi.variableIndex("args"),args);
    fmi.setScalarDouble(fmi.variableIndex("initialTemperature"),initialTemperature);
    
    // Exit initialization mode
    cout<<"\nFMI parameters:\n";
    fmi.startSimulation();
    
    // Get FMI parameters from 3D-ICE
    double sinkConductance=fmi.getScalarDouble(fmi.variableIndex("sinkCellBottomConductance"));
    double sinkLength=fmi.getScalarDouble(fmi.variableIndex("sinkLength"))*1e6; //Convert from m to um
    double sinkWidth =fmi.getScalarDouble(fmi.variableIndex("sinkWidth")) *1e6; //Convert from m to um
    double spreaderX0=fmi.getScalarDouble(fmi.variableIndex("spreaderX0"))*1e6; //Convert from m to um
    double spreaderY0=fmi.getScalarDouble(fmi.variableIndex("spreaderY0"))*1e6; //Convert from m to um
    int sinkRows     =fmi.getScalarInteger(fmi.variableIndex("sinkRows"));
    int sinkColumns  =fmi.getScalarInteger(fmi.variableIndex("sinkColumns"));
    
    // Compute the per unit area total conductance by paralleling the spreader and sink conductances
    double spreaderConductancePerUnitArea=spreaderConductance/(cellLength*cellWidth);
    double sinkConductancePerUnitArea=sinkConductance/((sinkLength/sinkColumns)*(sinkWidth/sinkRows));
    double conductancePerUnitArea=parallel(spreaderConductancePerUnitArea,sinkConductancePerUnitArea);
    
    // Precompute the grid mapping data structures for faster simulation
    Grid sinkGrid(
        0.0,        //X0
        0.0,        //Y0
        sinkLength, //X
        sinkWidth,  //Y
        sinkRows,   //Nr
        sinkColumns //Nc
    );
    Grid spreaderGrid(
        spreaderX0, //X0
        spreaderY0, //Y0
        cellLength*nCols, //X
        cellWidth*nRows,  //Y
        nRows,      //Nr
        nCols       //Nc
    );
    spreaderMapping=std::move(mapGrids(spreaderGrid,sinkGrid,conductancePerUnitArea,true));
    sinkMapping    =std::move(mapGrids(sinkGrid,spreaderGrid,conductancePerUnitArea,true));
    // Do not remove these printouts, they are used by the test suite (make test)
    cout<<"\nspreaderMapping:\n"<<spreaderMapping<<"\nsinkMapping:\n"<<sinkMapping<<"\n";
    
    // Precompute the FMI port variable indices for faster simulation
    sinkTemperatures=std::move(Matrix<double>(sinkRows,sinkColumns));
    sinkHeatFlows   =std::move(Matrix<double>(sinkRows,sinkColumns));
    // Note: the order of insertion in the vectors must be row major, to match the indexing inside the Matrix class
    sinkTemperatureIndices.reserve(sinkRows*sinkColumns);
    sinkHeatFlowIndices.reserve(sinkRows*sinkColumns);
    for(int i=1;i<=sinkRows;i++) // Modelica arrays are 1-based
    {
        for(int j=1;j<=sinkColumns;j++) // Modelica arrays are 1-based
        {
            sinkTemperatureIndices.push_back(fmi.variableIndex(
                string("T[")+to_string(i)+","+to_string(j)+"]"));
            sinkHeatFlowIndices.push_back(fmi.variableIndex(
                string("Q_flow[")+to_string(i)+","+to_string(j)+"]"));
        }
    }
    //fmi.printVariables();
}

void FmiWrapper::simulateStep(const CellMatrix spreaderTemperatures,
                                    CellMatrix heatFlow)
{
    // Get sink temperature matrix from FMI
    fmi.getVectorDouble(sinkTemperatureIndices.data(),&sinkTemperatures.at(0,0),sinkTemperatureIndices.size());
    
    // Compute the heat flow from the spreader side
    int spreaderRows=spreaderTemperatures.rows();
    int spreaderColumns=spreaderTemperatures.cols();
    for(int i=0;i<spreaderRows;i++)
    {
        for(int j=0;j<spreaderColumns;j++)
        {
            double q=0.0;
            double t=spreaderTemperatures.at(i,j);
            auto& fragments=spreaderMapping.at(i,j);
            for(auto& fragment : fragments)
                q+=fragment.feature*(t-sinkTemperatures.at(fragment.r,fragment.c));
            heatFlow.at(i,j)=q;
        }
    }
    
    // Compute the heat flow from the sink side
    int sinkRows=sinkTemperatures.row();
    int sinkColumns=sinkTemperatures.col();
    for(int i=0;i<sinkRows;i++)
    {
        for(int j=0;j<sinkColumns;j++)
        {
            double q=0.0;
            double t=sinkTemperatures.at(i,j);
            auto& fragments=sinkMapping.at(i,j);
            for(auto& fragment : fragments)
                q+=fragment.feature*(spreaderTemperatures.at(fragment.r,fragment.c)-t);
            sinkHeatFlows.at(i,j)=q;
        }
    }
    
    // Pass sink heat flow to FMI
    fmi.setVectorDouble(sinkHeatFlowIndices.data(),&sinkHeatFlows.at(0,0),sinkHeatFlowIndices.size());
    
    // Simulate one step
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
