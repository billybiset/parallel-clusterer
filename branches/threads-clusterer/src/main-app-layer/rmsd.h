/**
   File: rmsd.h
  
   System:         Parallel Clusterer
   Component Name: RMSD definitions and declarations
   Status:         Version 0.1 
   Language:       C++
  
   License: GNU General Public License ver. 3
  
   Author: Billy Biset
   E-Mail: billybiset AT gmail.com
  
   Description: Header file for Parallel Cluster providing definitions
                necessary for calculating RMSDs.
*/ 

#ifndef RMSD_H
#define RMSD_H

#include <vector>

typedef float    Distance;
typedef Distance RMSD;
typedef float    Mass;

typedef std::vector<Mass> Masses;

class Structure;

/* both structures (target and reference) shall be centered in 0,0,0 (that is, their centers 'removed') */

void rotalign(Structure& target, const Structure& reference);
RMSD RMSD_between(const Structure& a, const Structure& b);

void rotalign(Structure& target, const Structure& reference, const Masses& masses);
RMSD RMSD_between(const Structure& a, const Structure& b, const Masses& masses);

#endif
