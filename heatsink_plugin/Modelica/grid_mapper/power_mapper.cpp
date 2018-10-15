/***************************************************************************
 *   Copyright (C) 2014 by Terraneo Federico                               *
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
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include "power_mapper.h"
#include <iostream>
#include <cstdio>

using namespace std;

//
// class AreaProportionalPowerMapper
//

AreaProportionalPowerMapper::AreaProportionalPowerMapper(
    shared_ptr<PowerData> pd, shared_ptr<TemperatureData> td,
    const string& flpfile, int gridsize)
    : PowerMapper(pd,td,flpfile,gridsize), fragmentGrid(gridsize,gridsize)
{
    Tile chip=load(flpfile);
    normalize(chip);
    
    Coord e=envelope(chip);
    xChipSize=e.x;
    yChipSize=e.y;
    xGridPitch=e.x/gridsize;
    yGridPitch=e.y/gridsize;
    
    list<string> names;
    for(auto it : chip) names.push_back(it.name);
    map<string,int> indices=pd->getIndices(names);
    //All blocks, even those not computed here in the 3D chip case
    numBlocks=pd->getNumberOfFunctionalUnits();
            
    precomputePowerMappingCoefficients(chip,indices);
    precomputeTemperatureMappingCoefficients(chip,indices);
}

void AreaProportionalPowerMapper::precomputePowerMappingCoefficients(
    const Tile& chip, const map<string,int>& indices)
{
    for(int r=0;r<gridsize;r++)
    {
        for(int c=0;c<gridsize;c++)
        {
            //Clip rectangle coordinates
            Coord a(xGridPitch*c,yGridPitch*(gridsize-1-r));
            Coord b(xGridPitch*(c+1),yGridPitch*(gridsize-r));
            Fragments& frags=fragmentGrid.at(r,c);
            for(auto it : chip)
            {
                //Candidate fragment coordinates
                Coord p(it.posx,it.posy);
                //Find rectangle which is the non-empty intersection of the
                //fragment rectangle with the clip rectangle
                double xa=max(p.x,a.x);
                double xb=min<double>(p.x+it.sizex,b.x);
                if(xa>=xb) continue; //Empty intersection

                double ya=max(p.y,a.y);
                double yb=min<double>(p.y+it.sizey,b.y);
                if(ya>=yb) continue; //Empty intersection
                
                double fragmentArea=(xb-xa)*(yb-ya);
                double componentArea=it.sizex*it.sizey;
                frags.push_back(make_pair(indices.at(it.name),fragmentArea/componentArea));
            }
            if(frags.empty()) throw logic_error("Unmapped grid cell");
        }
    }
    #ifdef DUMP_MAPPING_TABLES
    cerr<<"Block to grid power mapping table"<<endl;
    for(int r=0;r<gridsize;r++)
    {
        for(int c=0;c<gridsize;c++)
        {
            Fragments& frags=fragmentGrid.at(r,c);
            cerr<<"P("<<r+1<<","<<c+1<<")=";
            bool plus=false;
            for(auto it : frags)
            {
                string fuName;
                //Inefficent lookup, but only used for debug dumping
                for(auto index : indices)
                {
                    if(index.second==it.first)
                    {
                        fuName=index.first;
                        break;
                    }
                }
                cerr<<(plus ? "+" : "")<<it.second<<"*P("<<fuName<<")";
                plus=true;
            }
            cerr<<endl;
        }
    }
    cerr<<endl;
    #endif //DUMP_MAPPING_TABLES
}

void AreaProportionalPowerMapper::precomputeTemperatureMappingCoefficients(
    const Tile& chip, const map<string,int>& indices)
{
    #ifdef CENTER_CELL_ONLY
    //return true if the point a is within the area identified by
    //b (upper left) and c (lower right)
    auto within=[](Coord a, Coord b, Coord c)->bool
    {
        return a.x>=b.x && a.y>=b.y && a.x<c.x && a.y<c.y;
    };
    backwardMapping.resize(numBlocks,make_pair(-1,-1));
    for(int i=0;i<chip.size();i++)
    {
        //Point in the middle of a functional unit
        auto block=chip.at(i);
        Coord p(block.posx+block.sizex/2.0,block.posy+block.sizey/2.0);
        for(int r=0;r<gridsize;r++)
        {
            for(int c=0;c<gridsize;c++)
            {
                Coord a(xGridPitch*c,yGridPitch*(gridsize-1-r));
                Coord b(xGridPitch*(c+1),yGridPitch*(gridsize-r));
                if(!within(p,a,b)) continue;
                backwardMapping.at(indices.at(block.name))=make_pair(r,c);
                goto quit;
            }
        }
        throw runtime_error("Functional unit not mapped to grid cell");
        quit:;
    }
    
    #ifdef DUMP_MAPPING_TABLES
    cerr<<"Grid to block temperature mapping table"<<endl;
    for(int i=0;i<chip.size();i++)
    {
        auto block=chip.at(i);
        auto bm=backwardMapping.at(indices.at(block.name));
        cerr<<"T("<<block.name<<")=T("<<bm.first+1<<","<<bm.second+1<<")"<<endl;
    }
    cerr<<endl;
    #endif //DUMP_MAPPING_TABLES
    #else //CENTER_CELL_ONLY
    //All blocks, even those not computed here in the 3D chip case
    backwardFragmentList.resize(numBlocks);
    for(auto it : chip)
    {
        //Clip rectangle coordinates
        Coord a(it.posx,it.posy);
        Coord b(it.posx+it.sizex,it.posy+it.sizey);
        
        double componentArea=it.sizex*it.sizey;
        BackwardFragments& frags=backwardFragmentList.at(indices.at(it.name));
        for(int r=0;r<gridsize;r++)
        {
            for(int c=0;c<gridsize;c++)
            {
                //Candidate functional unit
                Coord p(xGridPitch*c,yGridPitch*(gridsize-1-r));
                Coord p2(xGridPitch*(c+1),yGridPitch*(gridsize-r));
                
                //Find rectangle which is the non-empty intersection of the
                //fragment rectangle with the clip rectangle
                double xa=max(p.x,a.x);
                double xb=min<double>(p2.x,b.x);
                if(xa>=xb) continue; //Empty intersection

                double ya=max(p.y,a.y);
                double yb=min<double>(p2.y,b.y);
                if(ya>=yb) continue; //Empty intersection
                
                double fragmentArea=(xb-xa)*(yb-ya);
                frags.push_back(make_tuple(r,c,fragmentArea/componentArea));
            }
        }
    }
    //TODO: check that for non empty backwardFragmentList entries the sum of
    //coefficient is (close enought to) 1
    #ifdef DUMP_MAPPING_TABLES
    cerr<<"Grid to block temperature mapping table"<<endl;
    for(auto it : chip)
    {
        BackwardFragments& frags=backwardFragmentList.at(indices.at(it.name));
        cerr<<"T("<<it.name<<")=";
        bool plus=false;
        for(auto frag : frags)
        {
            cerr<<(plus ? "+" : "")<<get<2>(frag)<<"*T("
                <<get<0>(frag)+1<<","<<get<1>(frag)+1<<")";
            plus=true;
        }
        cerr<<endl;
    }
    cerr<<endl;
    #endif //DUMP_MAPPING_TABLES
    #endif //CENTER_CELL_ONLY
}

void AreaProportionalPowerMapper::getPowerGrid(ModelicaMatrixDouble& mmd)
{
    vector<double> componentPower=pd->getValues();
    pd->advanceIn();
    for(int r=0;r<gridsize;r++)
    {
        for(int c=0;c<gridsize;c++)
        {
            double power=0;
            Fragments& frags=fragmentGrid.at(r,c);
            for(auto it : frags) power+=componentPower.at(it.first)*it.second;
            mmd.at(r,c)=power;
        }
    }
}

void AreaProportionalPowerMapper::setTemperatureGrid(const ModelicaMatrixDouble& mmd)
{
    vector<double> temperatures;
    temperatures.resize(numBlocks,-1.0);
    #ifdef CENTER_CELL_ONLY
    for(int i=0;i<numBlocks;i++)
    {
        int r=backwardMapping.at(i).first;
        int c=backwardMapping.at(i).second;
        if(r<0 || c<0) continue; //Not computed here (for 3D chip)
        temperatures.at(i)=mmd.at(r,c);
    }
    #else //CENTER_CELL_ONLY
    int i=0;
    for(auto it : backwardFragmentList)
    {
        double val=0.0;
        //it can be an empty vector for a 3D chip
        for(auto frag : it)
        {
            int r=get<0>(frag);
            int c=get<1>(frag);
            val+=mmd.at(r,c)*get<2>(frag);
        }
        //If it is emty this temperature is not computed here (for 3D chip)
        if(it.empty()==false) temperatures.at(i)=val;
        i++;
    }
    #endif //CENTER_CELL_ONLY
    if(!td) throw runtime_error("no sink for temperature data");
    td->setValues(temperatures);
    td->advanceOut();
}
