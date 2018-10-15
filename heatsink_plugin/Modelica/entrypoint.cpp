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

#include <iostream>
#include <memory>
#include "entrypoint.h"
#include "heatsink.h"

using namespace std;
using namespace std::chrono;

static unsigned int nRows, nCols;
static unique_ptr<HeatSink> heatsink;
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
        heatsink=unique_ptr<HeatSink>(new HeatSink(nrows,ncols,cellwidth,celllength,
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
        const CellMatrix spt(const_cast<double*>(spreadertemperatures),nRows,nCols);
        CellMatrix sit(sinktemperatures,nRows,nCols);
        CellMatrix tc(conductances,nRows,nCols);
        switch(heatsink->simulateStep(spt,sit,tc))
        {
            case Conductances::NOT_CHANGED:
                return 0;
            case Conductances::CHANGED:
                return 1;
        }
        return 0;
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
