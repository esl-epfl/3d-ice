
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

PythonWrapper::PythonWrapper()
{
    setenv("PYTHONPATH",".",1);
    //https://mail.python.org/pipermail/new-bugs-announce/2008-November/003322.html
    so=dlopen("libpython3.5m.so", RTLD_LAZY | RTLD_GLOBAL);
    Py_Initialize();
    auto heatsink = check(PyImport_ImportModule("heatsink"));
    init          = check(PyObject_GetAttrString(heatsink,"heatsinkInit"));
    simulateStep  = check(PyObject_GetAttrString(heatsink,"heatsinkSimulateStep"));
}

void PythonWrapper::heatsinkInit(unsigned int nRows, unsigned int nCols,
                                 double cellWidth, double cellLength,
                                 double initialTemperature, double timeStep)
{
    auto args=check(PyTuple_New(6));
    PyTuple_SetItem(args,0,check(PyLong_FromLong(nRows)));
    PyTuple_SetItem(args,1,check(PyLong_FromLong(nCols)));
    PyTuple_SetItem(args,2,check(PyFloat_FromDouble(cellWidth)));
    PyTuple_SetItem(args,3,check(PyFloat_FromDouble(cellLength)));
    PyTuple_SetItem(args,4,check(PyFloat_FromDouble(initialTemperature)));
    PyTuple_SetItem(args,5,check(PyFloat_FromDouble(timeStep)));
    // If function throws a python exception, check fails and a C++ exception is thrown
    Py_DECREF(check(PyObject_CallObject(init,args)));
    Py_DECREF(args);
}

void PythonWrapper::heatsinkSimulateStep(const double *heatFlows, double *temperatures,
                                         unsigned int size)
{
//FIXME: using numpy arrays should be more performant, but all I got was segfaults
// long int sizes[1];
// sizes[0]=size;
// auto args=check(PyArray_SimpleNewFromData(1,sizes,NPY_DOUBLE,(void*)heatFlow));

    auto list=check(PyList_New(size));
    for(unsigned int i=0;i<size;i++)
        PyList_SET_ITEM(list,i,PyFloat_FromDouble(heatFlows[i]));
    auto args=check(Py_BuildValue("(O)",list));
    // If function throws a python exception, check fails and a C++ exception is thrown
    auto retVal=check(PyObject_CallObject(simulateStep,args));
    Py_DECREF(args);
    Py_DECREF(list);
    if(PyList_Check(retVal)==false)
        throw runtime_error("heatsinkSimulateStep should return a list");
    if(PyList_GET_SIZE(retVal)!=size)
        throw runtime_error("heatsinkSimulateStep returned list has wrong size");
    for(unsigned int i=0;i<size;i++)
        temperatures[i]=PyFloat_AsDouble(PyList_GetItem(retVal,i));
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
