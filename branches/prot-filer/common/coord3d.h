/**
 * \file  coord3d.h
 * \brief Definition of Coord3d struct.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009,2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing struc Coord3d.
 *
 * System:         FDC
 * Language:       C++
 *
 * Author:         Guillermo Biset
 * E-Mail:         billybiset AT gmail.com
 *
 * FDC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FDC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FuD.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

namespace clusterer
{
    struct Coord3d
    {
        inline Coord3d() :
            x(.0),
            y(.0),
            z(.0)
        {
        }

        inline Coord3d(float x , float y, float z ) :
            x(x), y(y), z(z)
        {
        }

        inline Coord3d(const Coord3d& other):
            x(other.x),
            y(other.y),
            z(other.z)
        {
        }

        inline Coord3d& operator += (const Coord3d& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return (*this);
        }

        inline Coord3d& operator /= (float f)
        {
            x /= f;
            y /= f;
            z /= f;
            return (*this);
        }

        float x,y,z;
    };
}