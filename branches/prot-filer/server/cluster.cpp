/**
 * \file  cluster.cpp
 * \brief Implementation file providing class Cluster.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 *  Contents:       Implementation file for FDC providing class Cluster.
 *
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

#include <limits>

#include "cluster.h"

using namespace clusterer;

Cluster::Cluster(ClusterID id, ProteinID representative, size_t natoms) :
    _id(id),
    _representative(representative),
    _members(),
    _geometric_mean(natoms),
    _distance_to_mean(std::numeric_limits<float>::max())
{
    for(size_t i(0); i < natoms; ++i)
        _geometric_mean[i]=Coord3d(.0,.0,.0);
}

size_t Cluster::get_size() const
{
    return 1u + _members.size();
}

ClusterID Cluster::get_id() const
{
    return _id;
}

ProteinID Cluster::representative() const
{
    return _representative;
}

const std::vector<ProteinID>& Cluster::members() const
{
    return _members;
}

const std::vector<Coord3d>& Cluster::geometric_mean() const
{
    return _geometric_mean;
}

void Cluster::calculate_mean()
{
    for (size_t i(0); i < _geometric_mean.size(); ++i)
        _geometric_mean[i] /= static_cast<float>(get_size());
}

void Cluster::add_member(ProteinID new_member)
{
    _members.push_back(new_member);
}

void Cluster::set_geometric_mean(const std::vector<Coord3d>& geometric_mean)
{
    _geometric_mean = geometric_mean;
}

void Cluster::set_representative(float distance, ProteinID new_representative)
{
    //assert(new_representative is in cluster);
    if (distance < _distance_to_mean)
    {
        _representative   = new_representative;
        _distance_to_mean = distance;
    }
}

void Cluster::add(const std::vector<Coord3d>& coords)
{
    assert(coords.size() == _geometric_mean.size());
    for (size_t i(0); i < _geometric_mean.size();++i)
        _geometric_mean[i] += coords[i];
}

/*
std::pair<std::vector<ProteinID>::iterator,std::vector<ProteinID>::iterator> Cluster::get_range_to_add(size_t size)
{
    std::vector<ProteinID>::iterator first( _members.begin() + _next_to_add );
    if (_next_to_add + size > get_size())
    {
        std::vector<ProteinID>::iterator second( _members.end() );
        _next_to_add = _members.size();
    }
    else
    {
        std::vector<ProteinID>::iterator second(  _members.begin() + _next_to_add + size  );
        _next_to_add += size;
    }
    return std::pair<std::vector<ProteinID>::iterator, std::vector<ProteinID>::iterator>(first, second);
}
*/