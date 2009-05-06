/**
   File: coord3d.h
  
   System:         Parallel Clusterer
   Component Name: Coord3d struct and auxiliries
   Status:         Version 0.1 
   Language:       C++
  
   License: GNU General Public License ver. 3
  
   Author: Billy Biset
   E-Mail: billybiset AT gmail.com
  
   Description: Header file for Parallel Cluster providing struct Coord3d and auxiliaries
*/ 

#ifndef COORD_3D_H
#define COORD_3D_H

#include <cmath>
#include "common.h"

namespace parallel_clusterer
{

    enum Axis3D
    {
        AxisX,
        AxisY,
        AxisZ,

        NAxis
    };

    struct Coord3D
    {
        union
        {
            struct
            {    
                Coord x,y,z;
            };
            Coord    axis[NAxis];
        };
        
        Coord3D(Coord x, Coord y, Coord z)   : x(x), y(y), z(z) {}
        Coord3D()                            : x(0.0), y(0.0), z(0.0) {}
        Coord3D(const Coord vec[NAxis])      : x(vec[AxisX]), y(vec[AxisY]), z(vec[AxisZ]) {}
        Coord3D(const Coord3D& other)        : x(other.x), y(other.y), z(other.z) {}
        
        /* 3D operations */
        Coord3D& operator += (const Coord3D& other)
        {
            x += other.x; y += other.y; z += other.z;
            return *this;
        }

        Coord3D& operator -= (const Coord3D& other)
        {
            x -= other.x; y -= other.y; z -= other.z;
            return *this;
        }
        
        Coord3D operator - (const Coord3D& other) const
        {
            Coord3D ret(*this);
            return ret -= other;
        }

        Coord3D& operator *= (Coord k)
        {
            x *= k; y *= k; z *= k;
            return *this;
        }
        
        Coord3D& operator /= (Coord k)
        {
            return operator *= (1.0/k);
        }
        
        Coord3D operator - () const
        {
            Coord3D ret(*this);
            ret *= -1.0;
            return ret;
        }
        
        Coord3D operator * (Coord k) const
        {
            Coord3D ret(*this);
            return (ret *= k);
        }
        
        Coord operator * (const Coord3D& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        Coord modulus2() const
        {
            return operator * (*this);
        }
        
        Coord modulus() const
        {
            return std::sqrt(modulus2());
        }

        Coord distance2_to(const Coord3D& other) const
        {
            Coord3D dist(*this);
            dist -= other;
            return dist.modulus2();
        }
        
        Coord distance_to(const Coord3D& other) const
        {
            Coord3D dist(*this);
            dist -= other;
            return dist.modulus();
        }
        
        Coord3D& rotate2D(Angle alpha, Axis3D axis1, Axis3D axis2);
    };



    /* Inline definitions. */
    inline Coord3D operator * (Coord k, const Coord3D c)
    {
        return c * k;
    }
}
#endif

