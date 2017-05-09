
#include "pythonwrapper.h"
#include "entrypoint.h"
#include <iostream>
#include <memory>

using namespace std;
using namespace std::chrono;

static unsigned int nRows, nCols;
static unique_ptr<PythonWrapper> wrapper;
static Profiler profiler;

int heatsink_init(unsigned int nrows, unsigned int ncols,
                  double cellwidth,   double celllength,
                  double initialtemperature,
                  double spreaderconductance,
                  double timestep)
{
    ProfileFunction pf(profiler);
    nRows=nrows;
    nCols=ncols;
    try {
        wrapper=unique_ptr<PythonWrapper>(new PythonWrapper(nrows,ncols,cellwidth,celllength,
                                          initialtemperature,spreaderconductance,timestep));
        return 0;
    } catch(exception& e) {
        cerr<<"exception thrown: "<<e.what()<<endl;
        return -1;
    }
}

int heatsink_simulate_step(const double *spreadertemperatures,
                                 double *sinktemperatures,
                                 double *conductances)
{
    ProfileFunction pf(profiler);
    try {
        return wrapper->simulateStep(spreadertemperatures,sinktemperatures,conductances);
    } catch(exception& e) {
        cerr<<"exception thrown: "<<e.what()<<endl;
        return -1;
    }
}

//
// Class Profiler
//

Profiler::Profiler()
{
    last=high_resolution_clock::now();
    mainTime=nanoseconds::zero();
    pluginTime=nanoseconds::zero();
}

void Profiler::enterPlugin()
{
    auto now=high_resolution_clock::now();
    mainTime+=duration_cast<nanoseconds>(now-last);
    last=now;
}

void Profiler::exitPlugin()
{
    auto now=high_resolution_clock::now();
    pluginTime+=duration_cast<nanoseconds>(now-last);
    last=now;
}

Profiler::~Profiler()
{
    auto now=high_resolution_clock::now();
    mainTime+=duration_cast<nanoseconds>(now-last);
    #ifdef PRINT_PROFILER_STATS
    printStats();
    #endif //PRINT_PROFILER_STATS
}

void Profiler::printStats()
{
    double mt=static_cast<double>(mainTime.count())/1e9;
    double pt=static_cast<double>(pluginTime.count())/1e9;
    double mp=100*mt/(mt+pt);
    double pp=100*pt/(mt+pt);
    cout<<"\nExecution time stats\n====================\n";
    auto print=[](string m, double t, double p)
    {
        auto flags=cout.setf(ios::fixed);
        auto prec=cout.precision(3);
        cout<<m<<t<<"s (";
        cout.precision(1);
        cout<<p<<"%)\n";
        cout.flags(flags);
        cout.precision(prec);
    };
    print("3D-ICE: ",mt,mp);
    print("plugin: ",pt,pp);
}
