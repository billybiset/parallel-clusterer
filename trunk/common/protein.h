/**
 * \file  protein.h
 * \brief Definition of Protein class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class Protein.
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


#ifndef PROTEIN_H
#define PROTEIN_H

#include <vector>
#include <stdlib.h>
#include <math.h>

#include "mili/mili.h"

typedef size_t ProteinID;

namespace clusterer
{

    // forward these
    class Protein;
    class ProteinRefWithClusterID;
    inline mili::bostream& operator<< (mili::bostream& bos, const ProteinRefWithClusterID& protein);
    inline mili::bostream& operator<< (mili::bostream& bos, const Protein& protein);
    inline mili::bistream& operator>> (mili::bistream& bis, Protein& protein);

    enum JobID
    {
        Representatives,
        Clusters,
        Adding,
        Centers
    };

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

    class Protein
    {
        friend class ProteinRefWithClusterID; //herein lie monsters
        public:
            inline Protein() :    // shouldn't use this constructor
                _atom_vector(0),
                _id(0)
            {
            }

            inline Protein(ProteinID id, size_t atoms):
                _atom_vector(atoms),
                _id(id)
            {
            }

            inline Protein(const Protein& other):
                _atom_vector(other._atom_vector),
                _id(other._id)
            {
            }

            inline Protein(const std::vector<Coord3d>& vector,const ProteinID& id):
                _atom_vector(vector),
                _id(id)
            {
            }

            inline size_t atoms() const
            {
                return _atom_vector.size();
            }

            inline ProteinID get_id() const
            {
                return _id;
            }

            inline float rmsd_to(const Protein& b) const
            {
                if (atoms() != b.atoms())
                    std::cerr << atoms() << " id " << get_id() << " != " << b.atoms() << " id " << b.get_id() <<  std::endl;

                assert(atoms() == b.atoms());

                return this->rmsd_to(b._atom_vector);
            }


            inline float rmsd_to(const std::vector<Coord3d>& b) const
            {
                assert(atoms() == b.size());

                const size_t NCOORDS = atoms();

                float ret(0);

                for(size_t i=0; i<NCOORDS; ++i)
                    ret += (((*this)[i].x - b[i].x) * ((*this)[i].x - b[i].x)) +
                        (((*this)[i].y - b[i].y) * ((*this)[i].y - b[i].y)) +
                        (((*this)[i].z - b[i].z) * ((*this)[i].z - b[i].z));

                return sqrt(ret/NCOORDS);
            }

            friend inline mili::bostream& operator<< (mili::bostream& bos, const Protein& protein);
            friend inline mili::bistream& operator>> (mili::bistream& bis, Protein& protein);

            inline Coord3d& operator[](size_t index)
            {
                return _atom_vector[index];
            }

            inline const Coord3d& operator[](size_t index) const
            {
                return _atom_vector[index];
            }

            inline Coord3d& front()
            {
                return _atom_vector.front();
            }

            inline const Coord3d& front() const
            {
                return _atom_vector.front();
            }

        private:
            std::vector<Coord3d> _atom_vector;
            ProteinID            _id;
    };

    class ProteinRefWithClusterID
    {
        public:
            ProteinRefWithClusterID(const Protein& ref, ProteinID id) :
                _vec(ref._atom_vector),
                _id(id)
            {
            }

            friend inline mili::bostream& operator<< (mili::bostream& bos, const ProteinRefWithClusterID& protein);

        private:
            const std::vector<Coord3d>& _vec;
            ProteinID      _id;
    };

    inline mili::bostream& operator<< (mili::bostream& bos, const ProteinRefWithClusterID& protein)
    {
        bos << protein._id << protein._vec;

        return bos;
    }


    inline mili::bostream& operator<< (mili::bostream& bos, const Protein& protein)
    {
        bos << protein._id << protein._atom_vector;

        return bos;
    }

    inline mili::bistream& operator>> (mili::bistream& bis, Protein& protein)
    {
        std::vector<Coord3d> v;
        ProteinID id;
        bis >> id;
        bis >> v;
        protein = Protein(v,id);

        return bis;
    }
}

#endif
