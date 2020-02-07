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
                             double timeStep,
                             const char *args)
{
    size=nRows*nCols;
    
    string arg=args;
    string filename=arg.substr(0,arg.find_first_of(" "));
    string filenameNoExt=filename.substr(0,filename.find_last_of("."));

    setenv("PYTHONPATH",".",1);
    //https://mail.python.org/pipermail/new-bugs-announce/2008-November/003322.html
    so=dlopen("libpython3.6m.so", RTLD_LAZY | RTLD_GLOBAL);
    Py_Initialize();
    auto heatsink = check(PyImport_ImportModule(filenameNoExt.c_str()));
    auto init     = check(PyObject_GetAttrString(heatsink,"heatsinkInit"));
    hSimulateStep = check(PyObject_GetAttrString(heatsink,"heatsinkSimulateStep"));

    auto pyargs   = check(PyTuple_New(8));
    PyTuple_SetItem(pyargs,0,check(PyLong_FromLong(nRows)));
    PyTuple_SetItem(pyargs,1,check(PyLong_FromLong(nCols)));
    PyTuple_SetItem(pyargs,2,check(PyFloat_FromDouble(cellWidth)));
    PyTuple_SetItem(pyargs,3,check(PyFloat_FromDouble(cellLength)));
    PyTuple_SetItem(pyargs,4,check(PyFloat_FromDouble(initialTemperature)));
    PyTuple_SetItem(pyargs,5,check(PyFloat_FromDouble(spreaderConductance)));
    PyTuple_SetItem(pyargs,6,check(PyFloat_FromDouble(timeStep)));
    PyTuple_SetItem(pyargs,7,check(PyUnicode_FromString(args)));
    // If function throws a python exception, check fails and a C++ exception is thrown
    Py_DECREF(check(PyObject_CallObject(init,pyargs)));
    Py_DECREF(pyargs);
}

void PythonWrapper::simulateStep(const double *spreaderTemperatures,
                                       double *heatFlow)
{
//FIXME: using numpy arrays should be more performant, but all I got was segfaults
// long int sizes[1];
// sizes[0]=size;
// auto pyargs=check(PyArray_SimpleNewFromData(1,sizes,NPY_DOUBLE,(void*)heatFlow));

    //The list of spreader temperatures is made every time
    auto list=check(PyList_New(size));
    for(unsigned int i=0;i<size;i++)
        PyList_SET_ITEM(list,i,PyFloat_FromDouble(spreaderTemperatures[i]));

    auto pyargs = check(PyTuple_New(1));
    PyTuple_SetItem(pyargs,0,list);

    // If function throws a python exception, check fails and a C++ exception is thrown
    auto retVal=check(PyObject_CallObject(hSimulateStep,pyargs));
    Py_DECREF(pyargs);

    if(PyList_Check(retVal)==false)
        throw runtime_error("heatsinkSimulateStep did not return heat flow list");
    if(PyList_GET_SIZE(retVal)!=size)
        throw runtime_error("heatsinkSimulateStep returned heat flow wrong size");
    for(unsigned int i=0;i<size;i++)
        heatFlow[i]=PyFloat_AsDouble(PyList_GetItem(retVal,i));
    
    Py_DECREF(retVal);
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
