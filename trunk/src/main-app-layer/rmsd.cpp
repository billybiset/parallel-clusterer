/**
   File: rmsd.cpp
  
   System:         Parallel Clusterer
   Component Name: RMSD definitions and declarations
   Status:         Version 0.1 
   Language:       C++
  
   License: GNU General Public License ver. 3
  
   Author: Billy Biset
   E-Mail: billybiset AT gmail.com
  
   Description: Implementation file for Parallel Cluster providing methods
                necessary for calculating RMSDs.
*/ 

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
//#include <exception>
#include <stdexcept>
#include <assert.h>

#include "rmsd.h"
#include "coord3d.h"
#include "structure.h"

using std::runtime_error;

/* First version: copy-paste each GMX function as-is. So it can be stand-alone for now. */

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b) )
#endif

/* Defines for vector indexing. */
#define XX  0
#define YY  1
#define ZZ  2
#define DIM 3       /* Vectors dimensions. */

#define FARGS 0,__FILE__,__LINE__

#ifndef M_SQRT2
#define M_SQRT2 sqrt(2.0)
#endif

typedef float           real;
typedef real            rvec[DIM];
typedef double          dvec[DIM];
typedef real            matrix[DIM][DIM];


void gmx_fatal(int f_errno,const char *file,int line,const char *fmt,...)
{
    fprintf(stderr, "ERROR in file %s line %d\n", file, line); /* TODO: improve.  */
}

static inline void oprod(const rvec a,const rvec b,rvec c)
{
  c[XX]=a[YY]*b[ZZ]-a[ZZ]*b[YY];
  c[YY]=a[ZZ]*b[XX]-a[XX]*b[ZZ];
  c[ZZ]=a[XX]*b[YY]-a[YY]*b[XX];
}

static FILE* debug = 0;

static inline void clear_mat(matrix a)
{
  const real nul=0.0;
  
  a[XX][XX]=a[XX][YY]=a[XX][ZZ]=nul;
  a[YY][XX]=a[YY][YY]=a[YY][ZZ]=nul;
  a[ZZ][XX]=a[ZZ][YY]=a[ZZ][ZZ]=nul;
}

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
  a[k][l]=h+s*(g-h*tau);

template <int JACOBI_DIM>    
static void jacobi(double a[][JACOBI_DIM],double d[],double v[][JACOBI_DIM],int *nrot)
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
          for (j=0; j<ip; j++) {
            ROTATE(a,j,ip,j,iq)
      }
          for (j=ip+1; j<iq; j++) {
            ROTATE(a,ip,j,j,iq)
            }
          for (j=iq+1; j<JACOBI_DIM; j++) {
            ROTATE(a,ip,j,iq,j)
            }
          for (j=0; j<JACOBI_DIM; j++) {
            ROTATE(v,j,ip,j,iq)
            }
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
  gmx_fatal(FARGS,"Error: Too many iterations in routine JACOBI\n");
}


static void calc_fit_R(int natoms,real *w_rls,rvec *xp,rvec *x,matrix R)
{
  int    c,r,n,j,i,irot;
  double d[2*DIM],xnr,xpc;
  matrix vh,vk,u;
  real   mn;
  int    index;
  real   max_d;

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
  /*real   **omega = input matrix a[0..n-1][0..n-1] must be symmetric
   *int     natoms = number of rows and columns
   *real      NULL = d[0]..d[n-1] are the eigenvalues of a[][]
   *real       **v = v[0..n-1][0..n-1] contains the vectors in columns
   *int      *irot = number of jacobi rotations
   */
  
  if (debug && irot==0) fprintf(debug,"IROT=0\n");
  
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

static void do_fit(int natoms,real *w_rls,rvec *xp,rvec *x)
{
  int    j,m,r,c;
  matrix R;
  rvec   x_old;

  /* Calculate the rotation matrix R */
  calc_fit_R(natoms,w_rls,xp,x,R);

  /*rotate X*/
  for(j=0; j<natoms; j++) {
    for(m=0; m<DIM; m++)
      x_old[m]=x[j][m];
    for(r=0; r<DIM; r++) {
      x[j][r]=0;
      for(c=0; c<DIM; c++)
        x[j][r]+=R[r][c]*x_old[c];
    }
  }
}

#if 0
static void calc_rms(int nind, int nframes, 
             real **dtot, real **dtot2, 
             real **rmsmat,  real *rmsmax,
             real **rmscmat, real *rmscmax, 
             real **meanmat, real *meanmax)
{
  int     i,j;
  real    mean, mean2, rms, rmsc;
/* N.B. dtot and dtot2 contain the total distance and the total squared
 * distance respectively, BUT they return RMS and the scaled RMS resp.
 */

  *rmsmax=-1000;
  *rmscmax=-1000;
  *meanmax=-1000;

  for(i=0; (i<nind-1); i++) {
    for(j=i+1; (j<nind); j++) {
      mean =dtot[i][j]/nframes;
      mean2=dtot2[i][j]/nframes;
      rms=sqrt(max(0,mean2-mean*mean));
      rmsc=rms/mean;
      if (mean > *meanmax) *meanmax=mean;
      if (rms  > *rmsmax ) *rmsmax =rms;
      if (rmsc > *rmscmax) *rmscmax=rmsc;
      meanmat[i][j]=meanmat[j][i]=mean;
      rmsmat[i][j] =rmsmat[j][i] =rms;
      rmscmat[i][j]=rmscmat[j][i]=rmsc;
    }
  }
}
#endif

// FuDePAN -> GMX type conversion routines
static void Coord3D2rvec(const Coord3D& coord3d, rvec rv)
{
    rv[XX] = coord3d.x;
    rv[YY] = coord3d.y;
    rv[ZZ] = coord3d.z; 
}

static void rvec2Coord3D(const rvec rv, Coord3D& coord3d)
{
    coord3d.x = rv[XX];
    coord3d.y = rv[YY];
    coord3d.z = rv[ZZ];
}

static rvec* structure2rvec_arr(const Structure& s)
{
    const size_t natoms = s.get_natoms();
    rvec* ret = new rvec[natoms];
    
    for(size_t i = 0; i < natoms; ++i)
        Coord3D2rvec(s.get_coord(i), ret[i]);
    
    return ret;
}

void rotalign(Structure& target, const Structure& reference)
{
    assert(target.get_natoms() == reference.get_natoms());
    const size_t NCOORDS = target.get_natoms();    

    rvec *const vt = structure2rvec_arr(target),
             *const vr = structure2rvec_arr(reference);
    real *const rls = new real[NCOORDS];
    size_t i;
    

    for(i=0; i<NCOORDS; ++i)
        rls[i] = 1.0;
    
    do_fit(NCOORDS, rls, vr, vt);

    for(i=0; i<NCOORDS; ++i)
    {
        Coord3D c;
        rvec2Coord3D(vt[i], c);
        
        target.set_coord(i, c);
    }

    delete [] vt;
    delete [] vr;
    delete [] rls;
}

RMSD RMSD_between(const Structure& a, const Structure& b)
{
    assert(a.get_natoms() == b.get_natoms());
    
    const size_t NCOORDS = a.get_natoms();    
    RMSD  ret(0);
    
    for(size_t i=0; i<NCOORDS; ++i)
    {        
        ret += a.get_coord(i).distance2_to(b.get_coord(i));
    }
    
    return sqrt(ret/NCOORDS);
}

void rotalign(Structure& target, const Structure& ref, const Masses& masses)
{
    throw runtime_error("rotalign(Masses) not implemented");
}

RMSD RMSD_between(const Structure& a, const Structure& b, const Masses& masses)
{
    throw runtime_error("RMSD_between(const Structure& a, const Structure& b, const std::vector<Mass>& weights) not implemented");
    return 0.0;
}
