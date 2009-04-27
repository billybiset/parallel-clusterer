/**
   File: structure.cpp
  
   System:         Parallel Clusterer
   Component Name: Structure class implementation
   Status:         Version 0.1 
   Language:       C++
  
   License: GNU General Public License ver. 3
  
   Author: Billy Biset
   E-Mail: billybiset AT gmail.com
  
   Description: Implementation file for Parallel Cluster providing class Structure
*/ 


#include <assert.h>
#include "structure.h"
#include "coord3d.h"
#include "rmsd.h"


/** Initializes the coords vector as per the same vector on other. */
Structure::Structure(const Structure& other)
    : _coords(other._coords)
{}

/** Initializes the coords vector with size natoms. */
Structure::Structure(size_t natoms)
    : _coords(natoms)
{}

/** Standard assignment. */
Structure& Structure::operator = (const Structure& other)
{
    assert( get_natoms() == other.get_natoms() );
    
    assign(other);
    return *this;
}

/** Addition of Structures, done adding coord by coord. */
Structure& Structure::operator += (const Structure& other)
{
    std::vector<Coord3D>::iterator it;
    std::vector<Coord3D>::const_iterator it2;
    
    assert( get_natoms() == other.get_natoms() );
    
    for(it = _coords.begin(), it2 = other._coords.begin(); it != _coords.end(); ++it, ++it2)
        *it += *it2;
    
    return *this;
}

/** Add a delta Coord to each coord in the Backbone. */
Structure& Structure::operator += (const Coord3D& delta)
{
    std::vector<Coord3D>::iterator it;

    for(it = _coords.begin(); it != _coords.end(); ++it)
        *it += delta;
    
    return *this;
}

/** Multiply each coord by a given k. */
Structure& Structure::operator *= (float k)
{

//     using namespace std;
//     for_each( _coords.begin(), _coords.end(), bind2nd(struct Coord3d::operator *=, k) );

    std::vector<Coord3D>::iterator it;

    for(it = _coords.begin(); it != _coords.end();  ++it)
        *it *= k;
    
    return *this;
}

/** Alias for *= 1.0 / k. */
Structure& Structure::operator /= (float k)
{
    return operator *= (1.0/k);
}

/** Return the number of atoms in the Backbone. */
size_t Structure::get_natoms() const
{
    return _coords.size();
}

/** Get coord at a given index. */
const Coord3D& Structure::get_coord(size_t index) const
{
    return _coords[index];
}

/** Set coord at a given index. */
void Structure::set_coord(size_t index, const Coord3D& coord)
{
    _coords[index] = coord;
}

/** Standard assignment between Structures. */
void Structure::assign(const Structure& other)
{
    _coords = other._coords;
}

/** Get the geometric mean of the Structure. */
Coord3D Structure::center() const
{
    std::vector<Coord3D>::const_iterator it;
    Coord3D ret; /* Initialized to <0,0,0>. */
    
    /* Add each coord to the result. */
    for(it = _coords.begin(); it != _coords.end(); ++it)
        ret += *it;
    
    ret /= static_cast<Coord>(_coords.size());
    
    return ret;
}

/** Get the geometric mean of the Structure give a specific weights vector. */
Coord3D Structure::center(const std::vector<Coord>& weights) const
{
    assert( get_natoms() == weights.size() );

    std::vector<Coord3D>::const_iterator    coords_it;
    std::vector<Coord>::const_iterator      weights_it;
    Coord3D ret;
    Coord    tot_weight(0);
        
    for(coords_it = _coords.begin(), weights_it = weights.begin();
        coords_it != _coords.end();
        ++coords_it, ++weights_it)
    {
        tot_weight += (*weights_it);
        ret        += (*weights_it) * (*coords_it);
    }
    
    ret /= tot_weight;
    
    return ret;
}

/** Rotate and align to match a differente Backbone. */
Structure& Structure::rotalign_to(const Structure& other)
{
    rotalign(*this, other);
    return *this;
}

/** Calculate the RMSD to a different Structure. */
RMSD Structure::rmsd_to(const Structure& other) const
{
    return RMSD_between(*this, other);
}

/* Editing functions */
EditableCoordsStructure::EditableCoordsStructure(const Structure& other)
    : Structure(other)
{}

void EditableCoordsStructure::add_coord(const Coord3D& c)
{
    _coords.push_back(c);
}

void EditableCoordsStructure::clear()
{
    _coords.clear();
}

EditableCoordsStructure& EditableCoordsStructure::operator = (const Structure& struc)
{
    assign(struc);
    return *this;
}

void EditableCoordsStructure::set_size(size_t natoms)
{
    _coords.resize(natoms);
}
