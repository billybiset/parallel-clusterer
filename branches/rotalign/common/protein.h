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
// Rotalign

    #define DIM     3           /* Dimension of vectors     */

    typedef float           real;
    typedef real            rvec[DIM];
    typedef double          dvec[DIM];
    typedef real            matrix[DIM][DIM];

    static inline void clear_mat(matrix a)
    {
        const float nul=0.0;

        a[0][0]=a[0][1]=a[0][2]=nul;
        a[1][0]=a[1][1]=a[1][2]=nul;
        a[2][0]=a[2][1]=a[2][2]=nul;
    }

    #define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau);


    template <int JACOBI_DIM>
    inline void jacobi(double a[][JACOBI_DIM],double d[],double v[][JACOBI_DIM],int *nrot)
    {
    int j,i;
    int iq,ip;
    double tresh,theta,tau,t,sm,s,h,g,c;

    double b [JACOBI_DIM];
    double z [JACOBI_DIM];

    for (ip=0; ip<JACOBI_DIM; ip++) {
        for (iq=0; iq<JACOBI_DIM; iq++) v[ip][iq]=0.0;
        v[ip][ip]=1.0;
    }
    for (ip=0; ip<JACOBI_DIM;ip++) {
        b[ip]=d[ip]=a[ip][ip];
        z[ip]=0.0;
    }
    *nrot=0;
    for (i=1; i<=50; i++) {
        sm=0.0;
        for (ip=0; ip<JACOBI_DIM-1; ip++) {
        for (iq=ip+1; iq<JACOBI_DIM; iq++)
            sm += fabs(a[ip][iq]);
        }
        if (sm == 0.0) {
        return;
        }
        if (i < 4)
        tresh=0.2*sm/(JACOBI_DIM*JACOBI_DIM);
        else
        tresh=0.0;
        for (ip=0; ip<JACOBI_DIM-1; ip++) {
        for (iq=ip+1; iq<JACOBI_DIM; iq++) {
            g=100.0*fabs(a[ip][iq]);
            if (i > 4 && fabs(d[ip])+g == fabs(d[ip])
                && fabs(d[iq])+g == fabs(d[iq]))
            a[ip][iq]=0.0;
            else if (fabs(a[ip][iq]) > tresh) {
            h=d[iq]-d[ip];
            if (fabs(h)+g == fabs(h))
                t=(a[ip][iq])/h;
            else {
                theta=0.5*h/(a[ip][iq]);
                t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
                if (theta < 0.0) t = -t;
            }
            c=1.0/sqrt(1+t*t);
            s=t*c;
            tau=s/(1.0+c);
            h=t*a[ip][iq];
            z[ip] -= h;
            z[iq] += h;
            d[ip] -= h;
            d[iq] += h;
            a[ip][iq]=0.0;
            for (j=0; j<ip; j++)
                ROTATE(a,j,ip,j,iq);
            for (j=ip+1; j<iq; j++)
                ROTATE(a,ip,j,j,iq);
            for (j=iq+1; j<JACOBI_DIM; j++)
                ROTATE(a,ip,j,iq,j);
            for (j=0; j<JACOBI_DIM; j++)
                ROTATE(v,j,ip,j,iq);
            ++(*nrot);
            }
        }
        }
        for (ip=0; ip<JACOBI_DIM; ip++) {
        b[ip] +=  z[ip];
        d[ip]  =  b[ip];
        z[ip]  =  0.0;
        }
    }
    }


    inline void oprod(const rvec a,const rvec b,rvec c)
    {
        c[0]=a[1]*b[2]-a[2]*b[1];
        c[1]=a[2]*b[0]-a[0]*b[2];
        c[2]=a[0]*b[1]-a[1]*b[0];
    }


    inline void calc_fit_R(int natoms,float *w_rls,rvec *xp,rvec *x,matrix R)
    {
        int    c,r,n,j,i,irot;
        double d[2*DIM],xnr,xpc;
        matrix vh,vk,u;
        float   mn;
        int    index;
        float   max_d;

        double omega[2*DIM][2*DIM];
        double om[2*DIM][2*DIM];

        for(i=0; i<2*DIM; i++) {
            d[i]=0;
            for(j=0; j<2*DIM; j++) {
            omega[i][j]=0;
            om[i][j]=0;
            }
        }

        /*calculate the matrix U*/
        clear_mat(u);
        for(n=0;(n<natoms);n++)
            if ((mn = w_rls[n]) != 0.0)
            for(c=0; (c<DIM); c++) {
            xpc=xp[n][c];
            for(r=0; (r<DIM); r++) {
            xnr=x[n][r];
            u[c][r]+=mn*xnr*xpc;
            }
            }

        /*construct omega*/
        /*omega is symmetric -> omega==omega' */
        for(r=0; r<2*DIM; r++)
            for(c=0; c<=r; c++)
            if (r>=DIM && c<DIM) {
                omega[r][c]=u[r-DIM][c];
                omega[c][r]=u[r-DIM][c];
            } else {
                omega[r][c]=0;
                omega[c][r]=0;
            }

        /*determine h and k*/
        jacobi<2*DIM>(omega,d,om,&irot);
        /*float   **omega = input matrix a[0..n-1][0..n-1] must be symmetric
        *int     natoms = number of rows and columns
        *float      NULL = d[0]..d[n-1] are the eigenvalues of a[][]
        *float       **v = v[0..n-1][0..n-1] contains the vectors in columns
        *int      *irot = number of jacobi rotations
        */

        index=0; /* For the compiler only */

        /* Copy only the first two eigenvectors */
        for(j=0; j<2; j++) {
            max_d=-1000;
            for(i=0; i<2*DIM; i++)
            if (d[i]>max_d) {
                max_d=d[i];
                index=i;
            }
            d[index]=-10000;
            for(i=0; i<DIM; i++) {
            vh[j][i]=M_SQRT2*om[i][index];
            vk[j][i]=M_SQRT2*om[i+DIM][index];
            }
        }
        /* Calculate the last eigenvector as the outer-product of the first two.
        * This insures that the conformation is not mirrored and
        * prevents problems with completely flat reference structures.
        */
        oprod(vh[0],vh[1],vh[2]);
        oprod(vk[0],vk[1],vk[2]);

        /*determine R*/
        for(r=0; r<DIM; r++)
            for(c=0; c<DIM; c++)
            R[r][c] = vk[0][r]*vh[0][c] +
                    vk[1][r]*vh[1][c] +
                    vk[2][r]*vh[2][c];
    }


    inline void do_fit(int natoms,float *w_rls,rvec *xp,rvec *x)
    {
        int    j,m,r,c;
        matrix R;
        rvec   x_old;

        /* Calculate the rotation matrix R */
        calc_fit_R(natoms,w_rls,xp,x,R);

        /*rotate X*/
        for(j=0; j<natoms; j++)
        {
            for(m=0; m<DIM; m++)
                x_old[m]=x[j][m];

            for(r=0; r<DIM; r++)
            {
                x[j][r]=0;

                for(c=0; c<DIM; c++)
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
        {
            Coord3d c;
            rvec2Coord3D(vt[i], c);

            _atom_vector[i] = c;
        }

        delete [] vt;
        delete [] vr;
        delete [] rls;
    }

// ~Rotalign


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
                float ret0;

                for(size_t i=0; i<NCOORDS; ++i)
                    ret += (((*this)[i].x - b[i].x) * ((*this)[i].x - b[i].x)) +
                        (((*this)[i].y - b[i].y) * ((*this)[i].y - b[i].y)) +
                        (((*this)[i].z - b[i].z) * ((*this)[i].z - b[i].z));

                ret0 = sqrt(ret/NCOORDS);

                #ifndef NOROTALIGN
                    rotalign_to(b);
                #endif

                ret = .0;

                assert(atoms() == b.size());

                for(size_t i=0; i<NCOORDS; ++i)
                    ret += (((*this)[i].x - b[i].x) * ((*this)[i].x - b[i].x)) +
                        (((*this)[i].y - b[i].y) * ((*this)[i].y - b[i].y)) +
                        (((*this)[i].z - b[i].z) * ((*this)[i].z - b[i].z));

                ret = sqrt(ret/NCOORDS);

                return ret;
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
