/**
 * \file  protein.cpp
 * \brief Implementation of Protein class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009,2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Implementation file for FDC providing class Protein.
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

#include "protein.h"

using namespace clusterer;

void Protein::calc_fit_R(int natoms,float *w_rls,rvec *xp,rvec *x,matrix R)
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

void Protein::do_fit(int natoms,float *w_rls,rvec *xp,rvec *x)
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

void Protein::rotalign_to(const std::vector<Coord3d>& reference)
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

template <size_t JACOBI_DIM>
void Protein::jacobi(double a[][JACOBI_DIM],double d[],double v[][JACOBI_DIM])
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

float Protein::rmsd_to(const std::vector<Coord3d>& b)
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
