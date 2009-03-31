/**
   File: structure.h
  
   System:         Parallel Clusterer
   Component Name: Structure class
   Status:         Version 0.1 
   Language:       C++
  
   License: GNU General Public License ver. 3
  
   Author: Billy Biset
   E-Mail: billybiset AT gmail.com
  
   Description: Header file for Parallel Cluster providing class Structure
*/ 

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <vector>

#include "coord3d.h"

/** 
 * Root Mean Square Deviation. Represents a measure of distance 
 * between the backbones of two superimposed proteins.
 */
typedef float RMSD; 

/**
 * Class Structure a Protein Backbone. Stores a list of coordenates corresponding to atoms in a Protein Backbone.
 */
class Structure 
{
    public:
        /**
         * Standard Constructor.
         * Takes the number of atoms in the backbone and initializes the corresponding coords to <0,0,0>.
         * @param natoms : Number of atoms in the backbone.
         */
        explicit Structure(size_t natoms);

        /**
         * Alternate Constructor.
         * Initialization by means of a different Structure.
         * @param other : A different Structure, 'this' will be a coord by coord copy of other.
         */
        Structure(const Structure& other);        

        /** Assignment. */
        Structure& operator  = (const Structure& other); 

        /** Coord by coord addition. */
        Structure& operator += (const Structure& other); 

        /** Displacement. Move each coord by a given delta. */
        Structure& operator += (const Coord3D& delta);   

        /** Multiply each coord member by k. */
        Structure& operator *= (float k);

        /** Alias for 'this *= 1 / k'. */
        Structure& operator /= (float k);                

        /** 
         * Rotate and align atoms to match the Backbone of another Structure.
         * @param other : A different Structure, 'this' will be rotated and aligned.
         * Note : Both 'this' and other have to be centered. 
         */
        Structure& rotalign_to(const Structure& other); 

        /**  Calculate the Root Mean Square Distance to another Structure. */
        RMSD rmsd_to(const Structure& other) const;
        
        /** Returns the number of atoms in the Backbone. */
        size_t get_natoms() const;

        /** Returns the coordenate at 'index'. */
        const Coord3D& get_coord(size_t index) const;

        /**
         * Change coordinate at a given index.
         * @param index : Index of coordinate to be modified.
         * @param coord : New coordinate.
         */
        void set_coord(size_t index, const Coord3D& coord);

        /** Returns the geometric mean of the Structure. */
        Coord3D center() const;

        /** Returns the geometric mean of the Structure given specific atom weights. */
        Coord3D center(const std::vector<Coord>& weights) const;

        /* To remove the center, just do 'struc += -struc.center()' */

    protected:
        Structure() {} 
        /** Standard assignment between Structures. */
        void assign(const Structure& other);

        /** Coordinates of atoms in the Backbone. */
        std::vector<Coord3D> _coords;
};

#endif