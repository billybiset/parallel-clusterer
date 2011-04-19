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
    using biopp::JobID;
    using biopp::Representatives;
    using biopp::Clusters;
    using biopp::Adding;
    using biopp::Centers;

    typedef biopp::StructureID ProteinID;
    typedef biopp::StructureWithRotationAndClusterID Protein;

    //// forward these
    class ProteinRefWithClusterID;
    inline mili::bostream& operator<< (mili::bostream& bos, const ProteinRefWithClusterID& protein);

    class ProteinRefWithClusterID
    {
        public:
            ProteinRefWithClusterID(const Protein& ref, ProteinID id) :
                _vec(ref._item_vector),
                _id(id)
            {}

            friend inline mili::bostream& operator<< (mili::bostream& bos, const ProteinRefWithClusterID& protein)
            {
                bos << protein._id << protein._vec;
                return bos;
            }

        private:
            const std::vector<Coord3d>& _vec;
            ProteinID                   _id; //Used to id the cluster it belongs to
    };
}

#endif
