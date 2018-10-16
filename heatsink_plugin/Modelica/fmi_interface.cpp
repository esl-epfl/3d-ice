/***************************************************************************
 *   Copyright (C) 2018 by Terraneo Federico                               *
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
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <pugixml.hpp>
#include "fmi_interface.h"

using namespace std;
using namespace pugi;

FmiInterface::FmiInterface(const string& modelName, LogLevel logLevel,
                           fmi2Type type)
    : logLevel(logLevel),
      callbacks{
          &FmiInterface::logfunc,  //logger
          calloc,                  //allocateMemory
          free,                    //freenMemory
          &FmiInterface::stepfunc, //stepFinished
          this                     //componentEnvironment
      }
{
    collectInformationFromXml(modelName);
    
    string resourceLocation=string("file:./")+modelName+"/resources";
    
    model=fmi2Instantiate(
        modelName.c_str(),        //fmi2String instanceName,
        type,                     //fmi2Type fmuType,
        guid.c_str(),             //fmi2String fmuGUID,
        resourceLocation.c_str(), //fmi2String fmuResourceLocation,
        &callbacks,               //const fmi2CallbackFunctions* functions,
        true,                     //fmi2Boolean visible,
        logLevel!=LogLevel::Off   //fmi2Boolean loggingOn
    );
    if(model==nullptr) throw runtime_error("FMI: failed to instantiate model ");
    
    if(fmi2SetupExperiment(
        model, //fmi2Component c,
        false, //fmi2Boolean toleranceDefined,
        1e-6,  //fmi2Real tolerance,
        0.0,   //fmi2Real startTime,
        false, //fmi2Boolean stopTimeDefined,
        1.0    //fmi2Real stopTime
    )!=fmi2OK) throw runtime_error("FMI: failed fmi2SetupExperiment");
    
    if(fmi2EnterInitializationMode(model)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2EnterInitializationMode");
}

void FmiInterface::startSimulation()
{
    if(fmi2ExitInitializationMode(model)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2ExitInitializationMode");
    simulationStarted=true;
}

void FmiInterface::doStep(double time, double step)
{
    if(fmi2DoStep(
        model, //fmi2Component c,
        time,  //fmi2Real currentCommunicationPoint,
        step,  //fmi2Real communicationStepSize,
        true   //fmi2Boolean noSetFMUStatePriorToCurrentPoint
    )!=fmi2OK) throw runtime_error("FMI: failed fmi2DoStep");
}

void FmiInterface::printVariables()
{
    printf("Variables:\n");
    for(auto & var : variables) printf("%d\t%s\n",var.second,var.first.c_str());        
    printf("\n");
}

unsigned int FmiInterface::variableIndex(const std::string& name) const
{
    auto it=variables.find(name);
    if(it==variables.end())
        throw std::runtime_error(std::string("Unknown variable ")+name);
    return it->second;
}

double FmiInterface::getScalarDouble(unsigned int variableIndex) const
{
    unsigned int vr[1]={variableIndex};
    double result;
    if(fmi2GetReal(model,vr,1,&result)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2GetReal");
    return result;
}

void FmiInterface::setScalarDouble(unsigned int variableIndex, double value)
{
    unsigned int vr[1]={variableIndex};
    if(fmi2SetReal(model,vr,1,&value)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2SetReal");
}

FmiInterface::~FmiInterface()
{
    if(simulationStarted)
    {
        auto result=fmi2Terminate(model);
        //Can't throw from destructors
        if(result!=fmi2OK && logLevel!=LogLevel::Off)
            printf("FMI: failed fmi2Terminate\n");
    }
    fmi2FreeInstance(model);
}

void FmiInterface::collectInformationFromXml(const std::string& modelName)
{
    xml_document doc;
    if(!doc.load_file((modelName+"/modelDescription.xml").c_str()))
        throw runtime_error("missing or wrong modelDescription.xml");

    auto root=doc.document_element();
    if(!root.attribute("guid")) throw runtime_error("XML: missing guid");
    guid=root.attribute("guid").value();
    
    auto vars=doc.select_nodes("/fmiModelDescription/ModelVariables/ScalarVariable");
    for(auto it=vars.begin();it!=vars.end();++it)
    {
        auto name=it->node().attribute("name").value();
        auto index=stoi(it->node().attribute("valueReference").value());
        variables[name]=index;
    }
}

void FmiInterface::stepfunc(fmi2ComponentEnvironment ptr, fmi2Status s)
{
//     auto obj=reinterpret_cast<FmiInterface*>(ptr);
    throw runtime_error("FMI model called stepFinished callback. This was not expected");
}

void FmiInterface::logfunc(fmi2ComponentEnvironment ptr,
                           fmi2String instance,
                           fmi2Status status,
                           fmi2String category,
                           fmi2String message, ...)
{
    auto obj=reinterpret_cast<FmiInterface*>(ptr);
    if(status==fmi2OK)
    {
        if(obj->getLogLevel()!=LogLevel::Verbose) return;
        printf("Note: ");
    } else {
        printf("Warning: ");
    }
    printf("model %s reported message of category %s: ",instance,category);
    va_list arg;
    va_start(arg,message);
    vprintf(message,arg);
    va_end(arg);
    printf("\n");
}
