/**
 * \file  protein.h
 * \brief Definition of Protein class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009,2010 Guillermo Biset, FuDePAN
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
#include "coord3d.h"

namespace clusterer
{

    typedef size_t ProteinID;

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

    class Protein
    {

        // Rotalign code ------------------------------------------------------------

        //Type definitions:

        #define DIM     3           /* Dimension of vectors     */

        typedef float           rvec[DIM];
        typedef double          dvec[DIM];
        typedef float           matrix[DIM][DIM];

        //Simple functions:

        void clear_mat(matrix a);

        void oprod(const rvec a,const rvec b,rvec c);

        void rotate(double a[][6], size_t i, size_t j, size_t k, size_t l, double tau, double s);

        //Rotation implementation functions:

        void calc_fit_R(int natoms,float *w_rls,rvec *xp,rvec *x,matrix R);

        template <size_t JACOBI_DIM>
        void jacobi(double a[][JACOBI_DIM],double d[],double v[][JACOBI_DIM]);

        void do_fit(int natoms,float *w_rls,rvec *xp,rvec *x);

        void rotalign_to(const std::vector<Coord3d>& reference);

        // Cast functions:

        void Coord3D2rvec(const Coord3d& coord3d, rvec rv);

        rvec* structure2rvec_arr(const std::vector<Coord3d>& s);

        void rvec2Coord3D(const rvec rv, Coord3d& coord3d);

        // ~Rotalign code ------------------------------------------------------------

        friend class ProteinRefWithClusterID; //herein lie monsters

        public: // of class Protein

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

            inline float rmsd_to(const Protein& b)
            {
                assert(atoms() == b.atoms());

                return this->rmsd_to(b._atom_vector);
            }

            float rmsd_to(const std::vector<Coord3d>& b);

            inline void rotalign_to(const Protein& other)
            {
                rotalign_to(other._atom_vector);
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
            ProteinID                   _id; //Used to id the cluster it belongs to
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
