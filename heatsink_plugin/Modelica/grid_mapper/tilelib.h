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

#ifndef TILELIB_H
#define TILELIB_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <limits>
#include <cmath>

using namespace std;

struct Coord
{
	Coord(double x,double y) :x(x), y(y) {}
	double x,y;
};

struct Component
{
	Component() : name(""), sizex(0), sizey(0), posx(0), posy(0) {}
	Component(const string& name, double x, double y)
		: name(name), sizex(x), sizey(y), posx(0), posy(0) {}
	string name;
	double sizex,sizey,posx,posy;
};

inline ostream& operator << (ostream& os, Component c)
{
	os<<c.name<<"\t"<<c.sizex<<"\t"<<c.sizey<<"\t"<<c.posx<<"\t"<<c.posy<<endl;
	return os;
}

inline istream& operator >> (istream& is, Component& c)
{
	is>>c.name>>c.sizex>>c.sizey>>c.posx>>c.posy;
	return is;
}

typedef vector<Component> Tile;

inline Tile operator + (const Tile& a, const Tile& b)
{
	Tile result;
	for(auto it=begin(a);it!=end(a);++it) result.push_back(*it);
	for(auto it=begin(b);it!=end(b);++it) result.push_back(*it);
	return result;
}

inline Tile& operator += (Tile& a, const Tile& b)
{
	for(auto it=begin(b);it!=end(b);++it) a.push_back(*it);
	return a;
}

inline Tile mktile(const Component& component)
{
	Tile result;
	result.push_back(component);
	return result;
}

inline Tile load(const string& file)
{
	ifstream in(file.c_str());
	if(!in.good()) throw runtime_error("Tile file not found");
	string line;
	Tile result;
	while(getline(in,line))
	{
		istringstream ss(line);
		ss.exceptions(ios::failbit);
		Component c;
		ss>>c;
		result.push_back(c);
	}
	return result;
}

inline void store(const string& file, const Tile& tile)
{
	ofstream out(file.c_str());
	for(auto it=begin(tile);it!=end(tile);++it) out<<*it;
}

inline void move(Tile& tile, double x, double y)
{
	for(auto it=begin(tile);it!=end(tile);++it)
	{
		it->posx+=x;
		it->posy+=y;
	}
}

inline Coord envelope(const Tile& tile)
{
	double minx=numeric_limits<double>::max();
	double maxx=numeric_limits<double>::min();
	double miny=minx,maxy=maxx;
	for(auto it=begin(tile);it!=end(tile);++it)
	{
		minx=min(minx,it->posx);
		miny=min(miny,it->posy);
		maxx=max(maxx,it->posx+it->sizex);
		maxy=max(maxy,it->posy+it->sizey);
	}
	return Coord(maxx-minx,maxy-miny);
}

inline void normalize(Tile& tile)
{
	double minx=numeric_limits<double>::max();
	double miny=minx;
	for(auto it=begin(tile);it!=end(tile);++it)
	{
		minx=min(minx,it->posx);
		miny=min(miny,it->posy);
	}
	move(tile,-minx,-miny); //Lower left corner is 0,0
}

inline void suffix(Tile& tile, const string& suffix)
{
	for(auto it=begin(tile);it!=end(tile);++it) it->name+=suffix;
}

inline void rename(Tile& tile, const string& search, const string& replace)
{
	for(auto it=begin(tile);it!=end(tile);++it)
	{
		size_t idx=it->name.find(search);
		if(idx==string::npos) continue;
		it->name.erase(idx,search.length());
		it->name.insert(idx,replace);
	}
}

#endif //TILELIB_H