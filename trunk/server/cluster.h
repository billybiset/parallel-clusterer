/**
 * \file  cluster.h
 * \brief Definition of Cluster class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class Cluster.
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

#ifndef CLUSTER_H
#define CLUSTER_H

#include <vector>

#include "protein.h"

typedef size_t ClusterID;

namespace clusterer
{
    class Cluster
    {
        public:
            Cluster(ClusterID id, ProteinID representative,size_t natoms);

            void add_member(ProteinID new_member);

            void set_geometric_mean(const std::vector<Coord3d>& geometric_mean);

            void set_representative(float distance, ProteinID new_representative);

            const std::vector<ProteinID>& members() const;

            ProteinID              representative() const;

            size_t                       get_size() const;

            ClusterID                      get_id() const;

            const std::vector<Coord3d>& geometric_mean() const;

            void add(const std::vector<Coord3d>& coords);

            void calculate_mean();

        private:
            ClusterID               _id;
            ProteinID               _representative;
            std::vector<ProteinID>  _members;
            std::vector<Coord3d>    _geometric_mean;
            float                   _distance_to_mean;
    };
}

#endif