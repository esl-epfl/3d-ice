/***************************************************************************
 *   Copyright (C) 2014-2019 by Terraneo Federico                          *
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

#pragma once

#include <algorithm>
#include <stdexcept>

template<typename T>
class Matrix
{
public:
    Matrix(int r, int c) : r(r), c(c), d(new T[r*c]), owner(true) {}

    Matrix(T* d, int r, int c) : r(r), c(c), d(d), owner(false) {}

    int row() const { return r; }

    int col() const { return c; }

    T& at(int r, int c)
    {
        using namespace std;
        int R=this->r;
        int C=this->c;
        if(r<0 || c<0 || r>=R || c>=C)
            throw std::range_error(string("Index out of range ")+to_string(r)+","+to_string(c)+">"+to_string(R)+","+to_string(C));
        return d[r*C+c];
    }

    const T& at(int r, int c) const
    {
        using namespace std;
        int R=this->r;
        int C=this->c;
        if(r<0 || c<0 || r>=R || c>=C)
            throw std::range_error(string("Index out of range ")+to_string(r)+","+to_string(c)+">"+to_string(R)+","+to_string(C));
        return d[r*C+c];
    }

    Matrix(const Matrix& rhs) : r(rhs.r), c(rhs.c), d(new T[r*c])
    {
        copy(rhs.d,rhs.d+r*c,d);
    }
    
    Matrix(Matrix&& rhs) : r(rhs.r), c(rhs.c), d(rhs.d), owner(rhs.owner)
    {
        rhs.d=nullptr;
        rhs.owner=false;
    }

    Matrix& operator=(const Matrix& rhs)
    {
        if(r!=rhs.r || c!=rhs.c) throw std::range_error("Size mismatch");
        copy(rhs.d,rhs.d+r*c,d);
        return *this;
    }
    
    
    Matrix& operator=(Matrix&& rhs)
    {
        if(owner) delete[] d;
        this->r=rhs.r;
        this->c=rhs.c;
        this->d=rhs.d;
        this->owner=rhs.owner;
        rhs.d=nullptr;
        rhs.owner=false;
        return *this;
    }

    ~Matrix() { if(owner) delete[] d; }
private:
    int r,c;
    T *d;
    bool owner;
};
