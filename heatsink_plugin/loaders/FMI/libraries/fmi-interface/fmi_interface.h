/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#pragma once

#include <string>
#include <map>
#include <stdexcept>

#include "fmi/fmi2Functions.h"

/**
 * Log level
 */
enum class LogLevel { Off, Normal, Verbose };

/**
 * Fmi interface functions
 */
struct FmiFunctions
{
    fmi2InstantiateTYPE             *fmi2Instantiate;
    fmi2SetupExperimentTYPE         *fmi2SetupExperiment;
    fmi2EnterInitializationModeTYPE *fmi2EnterInitializationMode;
    fmi2ExitInitializationModeTYPE  *fmi2ExitInitializationMode;
    fmi2DoStepTYPE                  *fmi2DoStep;
    fmi2GetRealTYPE                 *fmi2GetReal;
    fmi2GetIntegerTYPE              *fmi2GetInteger;
    fmi2GetBooleanTYPE              *fmi2GetBoolean;
    fmi2GetStringTYPE               *fmi2GetString;
    fmi2SetRealTYPE                 *fmi2SetReal;
    fmi2SetIntegerTYPE              *fmi2SetInteger;
    fmi2SetBooleanTYPE              *fmi2SetBoolean;
    fmi2SetStringTYPE               *fmi2SetString;
    fmi2TerminateTYPE               *fmi2Terminate;
    fmi2FreeInstanceTYPE            *fmi2FreeInstance;
};

/**
 * Simple C++ wrapper for the FMI interface
 */
class FmiInterface
{
public:
    /**
     * Constructor
     * \param modelName name of FMI model.
     * \param modelPath path where to find the model directory
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
    FmiInterface(const std::string& modelName,
                 const std::string& modelPath,
                 LogLevel logLevel,
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
     * NOTE: the first time this functtion is called, time should be 0, and at
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
     * \param variableIndex variable index obtained through variableIndex()
     * \return variable value
     */
    double getScalarDouble(unsigned int variableIndex) const;
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \param value variable value
     */
    void setScalarDouble(unsigned int variableIndex, double value);
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \return variable value
     */
    int getScalarInteger(unsigned int variableIndex) const;
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \param value variable value
     */
    void setScalarInteger(unsigned int variableIndex, int value);
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \return variable value
     */
    bool getScalarBoolean(unsigned int variableIndex) const;
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \param value variable value
     */
    void setScalarBoolean(unsigned int variableIndex, bool value);
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \return variable value
     */
    std::string getScalarString(unsigned int variableIndex) const;
    
    /**
     * \param variableIndex variable index obtained through variableIndex()
     * \param value variable value
     */
    void setScalarString(unsigned int variableIndex, const std::string& value);
    
    /**
     * \param indices pointer to the first index array element
     * \param variables pointer to an array where variable values will be read from the FMI
     * \param numVariables number of variables to read (must be the size of both arrays)
     * 
     */
    void getVectorDouble(const unsigned int *indices, double *variables, int numVariables) const;
    
    /**
     * \param indices pointer to the first index array element
     * \param variables pointer to an array with variable values that will be written in the FMI
     * \param numVariables number of variables to read (must be the size of both arrays)
     */
    void setVectorDouble(const unsigned int *indices, const double *variables, int numVariables);
    
    /**
     * \return the fmi2Component pointer
     */
    fmi2Component get() { return model; }
    
    /**
     * \return the function pointers to access directly FMI operations
     */
    const FmiFunctions getFunctions() const { return functions; }
    
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
    
    void collectInformationFromXml(const std::string& modelBase);
    
    void loadSharedObject(const std::string& fullSoName);
    
    static void stepfunc(fmi2ComponentEnvironment ptr, fmi2Status s);
    
    static void logfunc(fmi2ComponentEnvironment ptr,
                        fmi2String instance,
                        fmi2Status status,
                        fmi2String category,
                        fmi2String message, ...);
    
    fmi2CallbackFunctions callbacks;
    fmi2Component model;
    void *so=nullptr;
    FmiFunctions functions;
    LogLevel logLevel;
    std::string guid;
    std::map<std::string,unsigned int> variables;
    bool simulationStarted=false;
};
