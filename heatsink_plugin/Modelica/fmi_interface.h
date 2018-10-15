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

#pragma once

#include <string>
#include <map>
#include <stdexcept>

#define FMI2_FUNCTION_PREFIX
#include "fmi2Functions.h"

/**
 * Log level
 */
enum class LogLevel { Off, Normal, Verbose };

/**
 * Simple C++ wrapper for the FMI interface
 */
class FmiInterface
{
public:
    /**
     * Constructor
     * \param modelName name of FMI model. This is also used as path to find
     * the model directory
     * \param logLevel log level
     * \param type simulation type
     * 
     * This constructors performs the following FMI calls
     * - fmi2Instantiate()
     * - fmi2SetupExperiment()
     * - fmi2EnterInitializationMode()
     * and thus leaves the FMI model in intialization mode
     */
    FmiInterface(const std::string& modelName, LogLevel logLevel,
                 fmi2Type type=fmi2CoSimulation);
    
    /**
     * Exit initialization mode and start simulation
     */
    void startSimulation();
    
    /**
     * Integrate the model for the given time step
     * \param time current simulation time
     * \param step integration step
     * 
     * NOTE: the first time this functtion is called, time shoulld be 0, and at
     * every subsequent call the time shall be increased by the previous step
     */
    void doStep(double time, double step);
    
    /**
     * Print all variables in the model for debugging purposes
     */
    void printVariables();
    
    /**
     * \param name name of a variable in the model
     * \return an index to allow setting/getting the variable
     */
    unsigned int variableIndex(const std::string& name) const;
    
    /**
     * \param variableIndex variable indec obtained through variableIndex()
     * \return variable value
     */
    double getScalarDouble(unsigned int variableIndex) const;
    
    /**
     * \param variableIndex variable indec obtained through variableIndex()
     * \param value variable value
     */
    void setScalarDouble(unsigned int variableIndex, double value);
    
    /**
     * \return the fmi2Component pointer
     */
    fmi2Component get() { return model; }
    
    /**
     * \return the log level
     */
    LogLevel getLogLevel() const { return logLevel; }

    /**
     * Destructor, deallocates the model with fmi2FreeInstance().
     * Also terminates current simulation if started
     */
    ~FmiInterface();
    
private:
    
    void collectInformationFromXml(const std::string& modelName);
    
    static void stepfunc(fmi2ComponentEnvironment ptr, fmi2Status s);
    
    static void logfunc(fmi2ComponentEnvironment ptr,
                        fmi2String instance,
                        fmi2Status status,
                        fmi2String category,
                        fmi2String message, ...);
    
    fmi2CallbackFunctions callbacks;
    fmi2Component model;
    LogLevel logLevel;
    std::string guid;
    std::map<std::string,unsigned int> variables;
    bool simulationStarted=false;
};
