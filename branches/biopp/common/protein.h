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

#define NO_BIO_MOLECULAR
#include "biopp/biopp.h"

namespace clusterer
{

using biopp::Coord3d;

typedef biopp::StructureID ProteinID;

enum JobID
{
    Representatives,
    Clusters,
    Adding,
    Centers
};

template <class> class ClusterID_Aspect;
typedef ClusterID_Aspect< biopp::StructureWithRotation > Protein;
//inline bostream& operator<< (bostream& bos, const Protein& structure);

template<class A>
class ClusterID_Aspect : public A
{
public:
    ClusterID_Aspect(const A& ref, ProteinID id) :
        A(ref),
        _id(id)
    {}

    ClusterID_Aspect()
    {}

    ClusterID_Aspect(const std::vector<Coord3d> &vector, const biopp::StructureID& id):
        A(vector, id),
        _id()
    {}

    float rmsd_to(const std::vector<Coord3d> &b)
    {
        return A::rmsd_to(b, useRotalignValue());
    }

    float rmsd_to(const ClusterID_Aspect& b)
    {
        return A::rmsd_to(b, useRotalignValue());
    }

    friend inline bostream& operator<< (bostream& bos, const Protein& structure)
    {
        bos << structure._id << structure._item_vector;
        return bos;
    }
    friend inline bistream& operator>> (bistream& bis, Protein& structure)
    {
        std::vector<Coord3d> v;
        biopp::StructureID id;
        bis >> id;
        bis >> v;
        structure = Protein(v, id);
        return bis;
    }
private:
    ProteinID _id; //Used to id the cluster it belongs to
    static bool useRotalignValue()
    {
#ifdef ROTALIGN
        return true;
#else
        return false;
#endif
    }

};

}

#endif
