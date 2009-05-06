/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           common.h
    Contents:       Header file for Parallel Cluster providing the main type
                    definitions.

    System:         Parallel Clusterer
    Language:       C++
  
    Author:         Guillermo Biset
    E-Mail:         billybiset AT gmail.com
  
    Parallel Clusterer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Parallel Clusterer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Parallel Clusterer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string>
#include <vector>

namespace parallel_clusterer
{
    /** ID of a JobUnit, should be unique. */
    typedef size_t       JobUnitID;

    /** 
      * Size of a JobUnit, meaning is user defined. Generally will indicate
      * amount of bytes.
      */
    typedef size_t       JobUnitSize;

    /** Used to identify a method. */
    typedef std::string  MethodDescriptor;

    /** A Distance is a simple scalar type. */
    typedef float    Distance;

    /** 
      * Root Mean Square Deviation. Represents a measure of distance 
      * between the backbones of two superimposed proteins.
      */
    typedef Distance RMSD;

    /** A Mass is a simple scalar type. */
    typedef float    Mass;

    /** A Mass vector. Non-scalar type. */
    typedef std::vector<Mass> Masses;

    /** A Coord is a scalar type. */
    typedef float Coord;

    /** An angle is a scalar type. */
    typedef float Angle;
}

#endif