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

#include "common.h"
#include "structure.h"

namespace parallel_clusterer
{
    /* both structures (target and reference) shall be centered in 0,0,0
      (that is, their centers 'removed') */
    void rotalign(parallel_clusterer::Structure& target, const parallel_clusterer::Structure& reference);
    RMSD RMSD_between(const parallel_clusterer::Structure& a, const parallel_clusterer::Structure& b);

    void rotalign(parallel_clusterer::Structure& target, const parallel_clusterer::Structure& reference, const Masses& masses);
    RMSD RMSD_between(const parallel_clusterer::Structure& a, const parallel_clusterer::Structure& b, const Masses& masses);
}
#endif
