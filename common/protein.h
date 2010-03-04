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

// Rotalign code ------------------------------------------------------------

        #define DIM     3           /* Dimension of vectors     */

        typedef float           rvec[DIM];
        typedef double          dvec[DIM];
        typedef float           matrix[DIM][DIM];

        static inline void clear_mat(matrix a)
        {
            const float nul=0.0;

            a[0][0] = a[0][1] = a[0][2] = nul;
            a[1][0] = a[1][1] = a[1][2] = nul;
            a[2][0] = a[2][1] = a[2][2] = nul;
        }

        static inline void rotate(double a[][6], size_t i, size_t j, size_t k, size_t l,
                                double tau, double s)
        {
            const double g = a[i][j];
            const double h = a[k][l];

            a[i][j] = g - s * (h + g * tau);
            a[k][l] = h + s * (g - h * tau);
        }

        template <size_t JACOBI_DIM>
        static void jacobi(double a[][JACOBI_DIM],double d[],double v[][JACOBI_DIM])
        {
            double b [JACOBI_DIM];
            double z [JACOBI_DIM];

            for (size_t ip(0); ip < JACOBI_DIM; ++ip)
            {
                for (size_t iq(0); iq < JACOBI_DIM; ++iq)
                    v[ip][iq] = 0.0;

                v[ip][ip] = 1.0;
            }

            for (size_t ip(0); ip < JACOBI_DIM; ++ip)
            {
                b[ip] = d[ip] = a[ip][ip];
                z[ip] = 0.0;
            }

            for (size_t i(1); i <= 50; ++i)
            {
                double sm(0.0);

                for (size_t ip(0); ip < JACOBI_DIM-1; ++ip)
                {
                    for (size_t iq(ip+1); iq < JACOBI_DIM; ++iq)
                        sm += fabs(a[ip][iq]);
                }

                if (sm == 0.0)
                    return;

                double tresh;

                if (i < 4)
                    tresh = 0.2 * sm / mili::square(JACOBI_DIM);
                else
                    tresh = 0.0;

                for (size_t ip(0); ip < JACOBI_DIM-1; ++ip)
                {
                    for (size_t iq(ip+1); iq < JACOBI_DIM; ++iq)
                    {
                        const double g( 100.0 * fabs( a[ip][iq] ) );

                        const double fabs_d_ip = fabs(d[ip]);
                        const double fabs_d_iq = fabs(d[iq]);

                        //use mili::is_lossless_sum later
                        if (i > 4 && fabs_d_ip + g == fabs_d_ip && fabs_d_iq + g == fabs_d_iq )
                            a[ip][iq] = 0.0;
                        else if (fabs( a[ip][iq] ) > tresh)
                        {
                            double h( d[iq]-d[ip] );

                            double t;
                            const double fabs_h( fabs(h) );

                            //Idem
                            if (fabs_h + g == fabs_h)
                                t = ( a[ip][iq] ) / h;
                            else
                            {
                                const double theta( 0.5 * h / ( a[ip][iq] ) );

                                t = 1.0 / ( fabs(theta) + sqrt( 1.0 + mili::square(theta) ));

                                if (theta < 0.0)
                                    t = -t;
                            }

                            const double c( 1.0 / sqrt( 1 + mili::square(t) ) );
                            const double s( t * c);
                            const double tau( s / (1.0 + c) );

                            h = t * a[ip][iq];

                            z[ip] -= h;
                            z[iq] += h;
                            d[ip] -= h;
                            d[iq] += h;

                            a[ip][iq] = 0.0;

                            for (size_t j(0); j < ip; ++j)
                                rotate(a,j,ip,j,iq,tau,s);

                            for (size_t j(ip+1); j < iq; ++j)
                                rotate(a,ip,j,j,iq,tau,s);

                            for (size_t j(iq+1); j < JACOBI_DIM; ++j)
                                rotate(a,ip,j,iq,j,tau,s);

                            for (size_t j(0); j < JACOBI_DIM; ++j)
                                rotate(v,j,ip,j,iq,tau,s);

                        }
                    }
                }

                for (size_t ip(0); ip < JACOBI_DIM; ++ip)
                {
                    b[ip] +=  z[ip];
                    d[ip]  =  b[ip];
                    z[ip]  =  0.0;
                }
            }
        }


        inline void oprod(const rvec a,const rvec b,rvec c)
        {
            c[0] = a[1]*b[2]-a[2]*b[1];
            c[1] = a[2]*b[0]-a[0]*b[2];
            c[2] = a[0]*b[1]-a[1]*b[0];
        }


        inline void calc_fit_R(int natoms,float *w_rls,rvec *xp,rvec *x,matrix R)
        {
            const size_t DIM_DOUBLE(2*DIM);

            double d[2* DIM];
            matrix vh,vk,u;
            int    index;

            double omega[2*DIM][2*DIM];
            double om[2*DIM][2*DIM];

            for(size_t i(0); i < DIM_DOUBLE; ++i)
            {
                d[i] = 0;

                for(size_t j(0); j < DIM_DOUBLE; ++j)
                {
                    omega[i][j] = 0;
                    om[i][j]    = 0;
                }
            }

            /*calculate the matrix U*/
            clear_mat(u);

            for(int n(0); n < natoms; ++n)
            {
                const float mn(w_rls[n]);

                if (mn != 0.0)
                {
                    for(size_t c(0); c < DIM; ++c)
                        for(size_t r(0); r < DIM; ++r)
                            u[c][r] += mn * x[n][r] * xp[n][c];
                }
            }

            /*construct omega*/
            /*omega is symmetric -> omega==omega' */
            for(size_t r(0); r < DIM_DOUBLE; ++r)
            {
                for(size_t c(0); c <= r; ++c)
                {
                    if (r >= DIM && c < DIM)
                    {
                        omega[r][c] = u[r - DIM][c];
                        omega[c][r] = u[r - DIM][c];
                    }
                    else
                    {
                        omega[r][c] = 0;
                        omega[c][r] = 0;
                    }
                }
            }

            /*determine h and k*/
            jacobi<2 * DIM>(omega,d,om);
            /*float   **omega = input matrix a[0..n-1][0..n-1] must be symmetric
            *int     natoms = number of rows and columns
            *float      NULL = d[0]..d[n-1] are the eigenvalues of a[][]
            *float       **v = v[0..n-1][0..n-1] contains the vectors in columns
            */

            /* Copy only the first two eigenvectors */
            for(size_t j(0); j < 2; ++j)
            {
                index = 0;

                for(size_t i(1); i < DIM_DOUBLE; ++i)
                    if ( d[index] < d[i] )
                        index = i;

                d[index]=-10000;

                for(size_t i(0); i < DIM; ++i)
                {
                    vh[j][i] = M_SQRT2 * om[i][index];
                    vk[j][i] = M_SQRT2 * om[i+DIM][index];
                }
            }
            /* Calculate the last eigenvector as the outer-product of the first two.
            * This insures that the conformation is not mirrored and
            * prevents problems with completely flat reference structures.
            */
            oprod(vh[0],vh[1],vh[2]);
            oprod(vk[0],vk[1],vk[2]);

            /*determine R*/
            for(size_t r(0); r < DIM; ++r)
                for(size_t c(0); c < DIM; ++c)
                    R[r][c] = vk[0][r] * vh[0][c] +
                              vk[1][r] * vh[1][c] +
                              vk[2][r] * vh[2][c];
        }

        inline void do_fit(int natoms,float *w_rls,rvec *xp,rvec *x)
        {
            matrix R;

            /* Calculate the rotation matrix R */
            calc_fit_R(natoms,w_rls,xp,x,R);

            /*rotate X*/
            for(int j(0); j < natoms; ++j) //use int j because natoms is int
            {
                rvec x_old;

                for(size_t m(0); m < DIM; ++m)
                    x_old[m] = x[j][m];

                for(size_t r(0); r<DIM; ++r)
                {
                    x[j][r]=0;

                    for(size_t c(0); c < DIM; ++c)
                        x[j][r]+=R[r][c]*x_old[c];
                }
            }
        }

        inline void Coord3D2rvec(const Coord3d& coord3d, rvec rv)
        {
            rv[0] = coord3d.x;
            rv[1] = coord3d.y;
            rv[2] = coord3d.z;
        }


        inline rvec* structure2rvec_arr(const std::vector<Coord3d>& s)
        {
            const size_t ncoords = s.size();
            rvec* ret = new rvec[ncoords];

            for(size_t i = 0; i < ncoords; ++i)
                Coord3D2rvec(s[i], ret[i]);

            return ret;
        }

        inline void rvec2Coord3D(const rvec rv, Coord3d& coord3d)
        {
            coord3d.x = rv[0];
            coord3d.y = rv[1];
            coord3d.z = rv[2];
        }

        inline void rotalign_to(const std::vector<Coord3d>& reference)
        {
            assert( atoms() == reference.size() );

            const size_t NCOORDS = atoms();

            rvec *const vt = structure2rvec_arr(_atom_vector);
            rvec *const vr = structure2rvec_arr(reference);

            float *const rls = new float[NCOORDS];

            size_t i;

            for(i=0; i<NCOORDS; ++i)
                rls[i] = 1.0;

            do_fit(NCOORDS, rls, vr, vt);

            for(i=0; i<NCOORDS; ++i)
                rvec2Coord3D(vt[i],_atom_vector[i]);

            delete [] vt;
            delete [] vr;
            delete [] rls;
        }

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
                if (atoms() != b.atoms())
                    std::cerr << atoms() << " id " << get_id() << " != " << b.atoms() << " id " << b.get_id() <<  std::endl;

                assert(atoms() == b.atoms());

                return this->rmsd_to(b._atom_vector);
            }

            inline float rmsd_to(const std::vector<Coord3d>& b)
            {
                const size_t NCOORDS = atoms();

                float ret(0);

                #ifdef ROTALIGN
                    rotalign_to(b);
                #endif

                assert(atoms() == b.size());

                for(size_t i=0; i<NCOORDS; ++i)
                    ret += (((*this)[i].x - b[i].x) * ((*this)[i].x - b[i].x)) +
                        (((*this)[i].y - b[i].y) * ((*this)[i].y - b[i].y)) +
                        (((*this)[i].z - b[i].z) * ((*this)[i].z - b[i].z));

                ret = sqrt(ret/NCOORDS);

                return ret;
            }


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
