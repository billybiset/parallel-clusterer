/**
   File: coord3d.cpp
  
   System:         Parallel Clusterer
   Component Name: Coord3d struct implementation
   Status:         Version 0.1 
   Language:       C++
  
   License: GNU General Public License ver. 3
  
   Author: Billy Biset
   E-Mail: billybiset AT gmail.com
  
   Description: Implementation file for Parallel Cluster providing struct Coord3d
*/ 

#include <cmath>
#include "coord3d.h"

using namespace parallel_clusterer;

Coord3D& Coord3D::rotate2D(Angle alpha, Axis3D axis1, Axis3D axis2)
{
    const Coord c = std::cos(alpha), s = std::sin(alpha);
    
    const Coord old1 = axis[axis1];
    
    axis[axis1] = c * axis[axis1] - s * axis[axis2];
    axis[axis2] = s * old1        + c * axis[axis2];
    
    return *this;
}

