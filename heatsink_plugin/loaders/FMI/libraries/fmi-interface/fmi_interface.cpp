/***************************************************************************
 *   Copyright (C) 2018,2019,2020 by Terraneo Federico                     *
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
#include <algorithm>
#include <dlfcn.h>
#include <pugixml.hpp>
#include "fmi_interface.h"

using namespace std;
using namespace pugi;

FmiInterface::FmiInterface(const string& modelName,
                           const string& modelPath,
                           LogLevel logLevel,
                           fmi2Type type)
    : callbacks{
          &FmiInterface::logfunc,  //logger
          calloc,                  //allocateMemory
          free,                    //freeMemory
          &FmiInterface::stepfunc, //stepFinished
          this                     //componentEnvironment
      }, logLevel(logLevel)
{
    string modelBase=(modelPath.empty() ? modelName : modelPath+"/"+modelName);
    collectInformationFromXml(modelBase);
    
    string soName=modelName;
    replace(begin(soName),end(soName),'.','_');
    loadSharedObject(modelBase+"/binaries/linux64/"+soName+".so");
    
    string resourceLocation=string("file:")+modelBase+"/resources";
    model=functions.fmi2Instantiate(
        modelName.c_str(),        //fmi2String instanceName,
        type,                     //fmi2Type fmuType,
        guid.c_str(),             //fmi2String fmuGUID,
        resourceLocation.c_str(), //fmi2String fmuResourceLocation,
        &callbacks,               //const fmi2CallbackFunctions* functions,
        true,                     //fmi2Boolean visible,
        logLevel!=LogLevel::Off   //fmi2Boolean loggingOn
    );
    if(model==nullptr) throw runtime_error("FMI: failed to instantiate model ");
    
    if(functions.fmi2SetupExperiment(
        model, //fmi2Component c,
        false, //fmi2Boolean toleranceDefined,
        1e-6,  //fmi2Real tolerance,
        0.0,   //fmi2Real startTime,
        false, //fmi2Boolean stopTimeDefined,
        1.0    //fmi2Real stopTime
    )!=fmi2OK) throw runtime_error("FMI: failed fmi2SetupExperiment");
    
    if(functions.fmi2EnterInitializationMode(model)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2EnterInitializationMode");
}

void FmiInterface::startSimulation()
{
    if(functions.fmi2ExitInitializationMode(model)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2ExitInitializationMode");
    simulationStarted=true;
}

void FmiInterface::doStep(double time, double step)
{
    if(functions.fmi2DoStep(
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
    if(functions.fmi2GetReal(model,vr,1,&result)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2GetReal");
    return result;
}

void FmiInterface::setScalarDouble(unsigned int variableIndex, double value)
{
    unsigned int vr[1]={variableIndex};
    if(functions.fmi2SetReal(model,vr,1,&value)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2SetReal");
}

int FmiInterface::getScalarInteger(unsigned int variableIndex) const
{
    unsigned int vr[1]={variableIndex};
    int result;
    if(functions.fmi2GetInteger(model,vr,1,&result)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2GetInteger");
    return result;
}

void FmiInterface::setScalarInteger(unsigned int variableIndex, int value)
{
    unsigned int vr[1]={variableIndex};
    if(functions.fmi2SetInteger(model,vr,1,&value)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2SetInteger");
}

bool FmiInterface::getScalarBoolean(unsigned int variableIndex) const
{
    unsigned int vr[1]={variableIndex};
    int result;
    if(functions.fmi2GetBoolean(model,vr,1,&result)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2GetBoolean");
    return static_cast<bool>(result);
}

void FmiInterface::setScalarBoolean(unsigned int variableIndex, bool value)
{
    unsigned int vr[1]={variableIndex};
    int v=static_cast<int>(value);
    if(functions.fmi2SetBoolean(model,vr,1,&v)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2SetBoolean");
}

string FmiInterface::getScalarString(unsigned int variableIndex) const
{
    unsigned int vr[1]={variableIndex};
    const char *result;
    if(functions.fmi2GetString(model,vr,1,&result)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2GetString");
    return result;
}

void FmiInterface::setScalarString(unsigned int variableIndex, const string& value)
{
    unsigned int vr[1]={variableIndex};
    const char *str=value.c_str();
    if(functions.fmi2SetString(model,vr,1,&str)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2SetString");
}

void FmiInterface::getVectorDouble(const unsigned int *indices, double *variables, int numVariables) const
{
    if(functions.fmi2GetReal(model,indices,numVariables,variables)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2GetReal");
}
    
void FmiInterface::setVectorDouble(const unsigned int *indices, const double *variables, int numVariables)
{
    if(functions.fmi2SetReal(model,indices,numVariables,variables)!=fmi2OK)
        throw runtime_error("FMI: failed fmi2SetReal");
}

FmiInterface::~FmiInterface()
{
    if(simulationStarted)
    {
        auto result=functions.fmi2Terminate(model);
        //Can't throw from destructors
        if(result!=fmi2OK && logLevel!=LogLevel::Off)
            printf("FMI: failed fmi2Terminate\n");
    }
    if(model) functions.fmi2FreeInstance(model);
    if(so) dlclose(so);
}

void FmiInterface::collectInformationFromXml(const std::string& modelBase)
{
    xml_document doc;
    if(!doc.load_file((modelBase+"/modelDescription.xml").c_str()))
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

void FmiInterface::loadSharedObject(const std::string& fullSoName)
{
    so=dlopen(fullSoName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if(so==nullptr) throw runtime_error("FMI: failed to open shared object");
    
    auto resolve=[this](const char *function)->void*
    {
        auto result=dlsym(so,function);
        if(result==nullptr)
            throw runtime_error(string("FMI: couldn't resolve ")+function);
        return result;
    };
    
    #define RESOLVE(x) functions.x=reinterpret_cast<x##TYPE*>(resolve(#x))
    RESOLVE(fmi2Instantiate);
    RESOLVE(fmi2SetupExperiment);
    RESOLVE(fmi2EnterInitializationMode);
    RESOLVE(fmi2ExitInitializationMode);
    RESOLVE(fmi2DoStep);
    RESOLVE(fmi2GetReal);
    RESOLVE(fmi2GetInteger);
    RESOLVE(fmi2GetString);
    RESOLVE(fmi2SetReal);
    RESOLVE(fmi2SetInteger);
    RESOLVE(fmi2SetString);
    RESOLVE(fmi2Terminate);
    RESOLVE(fmi2FreeInstance);
    #undef RESOLVE
}

void FmiInterface::stepfunc(fmi2ComponentEnvironment /*ptr*/, fmi2Status /*s*/)
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
