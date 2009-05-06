/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           structure.h
    Contents:       Header file for Parallel Cluster providing class Structure.

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

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <vector>

#include "coord3d.h"
#include "common.h"

namespace parallel_clusterer
{
    /**
      * Class Structure : a Protein Backbone. Stores a list of coordenates corresponding
      * to atoms in a Protein Backbone.
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

    class EditableCoordsStructure : public Structure
    {
    public:
        EditableCoordsStructure(){}
        EditableCoordsStructure(const Structure& other);
        
        void add_coord(const Coord3D& c);
        void clear();
        EditableCoordsStructure& operator = (const Structure& other);
        
        EditableCoordsStructure& operator = (const EditableCoordsStructure& other)
        {
            return operator = (static_cast<const Structure&>(other));
        }
        
        void set_size(size_t natoms);
    };
}


#endif