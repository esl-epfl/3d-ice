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
 *                             Copyright (C) 2017                             *
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

#include "pythonwrapper.h"
// #define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
// #include <arrayobject.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <dlfcn.h>

using namespace std;

//
// class PythonWrapper
//

PythonWrapper::PythonWrapper(unsigned int nRows, unsigned int nCols,
                             double cellWidth,   double cellLength,
                             double initialTemperature,
                             double spreaderConductance,
                             double timeStep)
{
    size=nRows*nCols;

    setenv("PYTHONPATH",".",1);
    //https://mail.python.org/pipermail/new-bugs-announce/2008-November/003322.html
    so=dlopen("libpython3.5m.so", RTLD_LAZY | RTLD_GLOBAL);
    Py_Initialize();
    auto heatsink = check(PyImport_ImportModule("heatsink"));
    auto init     = check(PyObject_GetAttrString(heatsink,"heatsinkInit"));
    hSimulateStep = check(PyObject_GetAttrString(heatsink,"heatsinkSimulateStep"));

    auto args     = check(PyTuple_New(7));
    PyTuple_SetItem(args,0,check(PyLong_FromLong(nRows)));
    PyTuple_SetItem(args,1,check(PyLong_FromLong(nCols)));
    PyTuple_SetItem(args,2,check(PyFloat_FromDouble(cellWidth)));
    PyTuple_SetItem(args,3,check(PyFloat_FromDouble(cellLength)));
    PyTuple_SetItem(args,4,check(PyFloat_FromDouble(initialTemperature)));
    PyTuple_SetItem(args,5,check(PyFloat_FromDouble(spreaderConductance)));
    PyTuple_SetItem(args,6,check(PyFloat_FromDouble(timeStep)));
    // If function throws a python exception, check fails and a C++ exception is thrown
    Py_DECREF(check(PyObject_CallObject(init,args)));
    Py_DECREF(args);
}

int PythonWrapper::simulateStep(const double *spreaderTemperatures,
                                      double *sinkTemperatures,
                                      double *thermalConductances)
{
//FIXME: using numpy arrays should be more performant, but all I got was segfaults
// long int sizes[1];
// sizes[0]=size;
// auto args=check(PyArray_SimpleNewFromData(1,sizes,NPY_DOUBLE,(void*)heatFlow));

    //The list of spreader temperatures is made every time
    auto list=check(PyList_New(size));
    for(unsigned int i=0;i<size;i++)
        PyList_SET_ITEM(list,i,PyFloat_FromDouble(spreaderTemperatures[i]));

    //Thermal conductances are cached, and the list is only made once
    if(cachedConductances==nullptr)
    {
        cachedConductances=check(PyList_New(size));
        for(unsigned int i=0;i<size;i++)
            PyList_SET_ITEM(cachedConductances,i,PyFloat_FromDouble(thermalConductances[i]));
    }

    auto args = check(PyTuple_New(2));
    PyTuple_SetItem(args,0,list);
    Py_INCREF(cachedConductances); //when the tuple is destroyed, refcount is decremented
    PyTuple_SetItem(args,1,cachedConductances);

    // If function throws a python exception, check fails and a C++ exception is thrown
    auto retVal=check(PyObject_CallObject(hSimulateStep,args));
    Py_DECREF(args);

    if(PyTuple_Check(retVal)==false || PyTuple_GET_SIZE(retVal)!=2)
        throw runtime_error("heatsinkSimulateStep should return a tuple of 2 items");

    auto sinkTemp            = PyTuple_GetItem(retVal,0);
    auto conductancesChanged = PyTuple_GetItem(retVal,1);

    if(PyList_Check(sinkTemp)==false)
        throw runtime_error("heatsinkSimulateStep did not return temperatures list");
    if(PyList_GET_SIZE(sinkTemp)!=size)
        throw runtime_error("heatsinkSimulateStep returned temperatures wrong size");
    for(unsigned int i=0;i<size;i++)
        sinkTemperatures[i]=PyFloat_AsDouble(PyList_GetItem(sinkTemp,i));

    int result=0;
    if(PyBool_Check(conductancesChanged)==false)
        throw runtime_error("heatsinkSimulateStep did not return bool");
    if(conductancesChanged==Py_True)
    {
        for(unsigned int i=0;i<size;i++)
            thermalConductances[i]=PyFloat_AsDouble(PyList_GetItem(cachedConductances,i));
        result=1; //Signal 3D-ICE that conductances were updated
    }
    
    Py_DECREF(retVal);
    return result;
}

PythonWrapper::~PythonWrapper()
{
    Py_Finalize();
    dlclose(so);
}

PyObject *PythonWrapper::check(PyObject *object)
{
    if(object) return object;
    PyErr_Print();
    throw runtime_error("python API returned error");
}
