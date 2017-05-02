
#include <iostream>
#include "entrypoint.h"
#include "heatsink.h"

using namespace std;
using namespace std::chrono;

static unsigned int nRows, nCols;
static HeatSink heatsink;
static Profiler profiler;

bool heatsink_init(unsigned int nrows, unsigned int ncols,
                   double cellwidth, double celllength,
                   double initialtemperature, double timestep)
{
    ProfileFunction pf(profiler);
    nRows=nrows;
    nCols=ncols;
    try {
        heatsink.heatsinkInit(nrows,ncols,cellwidth,celllength,initialtemperature,timestep);
        return true;
    } catch(exception& e) {
        cerr<<"exception thrown: "<<e.what()<<endl;
        return false;
    }
}

bool heatsink_simulate_step(const double *heatflows, double *temperatures,
                            unsigned int size)
{
    ProfileFunction pf(profiler);
    if(size!=nRows*nCols) return false;
    try {
        const CellMatrix hf(const_cast<double*>(heatflows),nRows,nCols);
        CellMatrix tm(temperatures,nRows,nCols);
        heatsink.heatsinkSimulateStep(hf,tm);
        return true;
    } catch(exception& e) {
        cerr<<"exception thrown: "<<e.what()<<endl;
        return false;
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
