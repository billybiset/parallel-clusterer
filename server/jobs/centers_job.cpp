/**
 * \file  centers_job.cpp
 * \brief Implementation file providing class CentersJob.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 *  Contents:       Implementation file for FDC providing class CentersJob.
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

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <syslog.h>


#include "mili/mili.h"
#include "centers_job.h"
#include "protein.h"

using namespace fud;
using namespace clusterer;

CentersJob::CentersJob(ProteinDatabase& protein_db, std::vector<Cluster>& clusters, float cutoff) :
    DistributableJobImplementation(),
    _protein_db(protein_db),
    _clusters(clusters),
    _atoms_in_a_protein(protein_db.get_atom_number()),
    _finished(false),
    _cutoff(cutoff),
    _current_cluster(0),
    _next_protein(0),
    _proteins_visited(0)
{
}


const char* CentersJob::get_name() const
{
    return "CentersJob";
}

void CentersJob::handle_results (JobUnitID id,InputMessage& input)
{
    ClusterID cluster_id;
    size_t    proteins_visited;
    ProteinID closest_id;
    float     distance;

    input >> cluster_id >> proteins_visited >> closest_id >> distance;

    _proteins_visited[cluster_id] += proteins_visited;

    _clusters[cluster_id].set_representative(distance,closest_id);
}

void CentersJob::output_results() const
{
    std::cout << "Total Clusters: " << _clusters.size() << std::endl;
    size_t acum(0);
    for (size_t i(0); i < _clusters.size(); ++i)
    {
        const Cluster& cluster( _clusters[i] );
        std::cout << "Cluster " << std::setw(4) << i << ". Representative : "  << std::setw(8) << cluster.representative() << ". Members : "  << std::setw(8) << cluster.members().size() << '.' << std::endl;
        acum += cluster.get_size();

    }
    std::cout << "Total proteins in clusters: " << acum << std::endl;

}

DistributableJobStatus CentersJob::get_status() const
{
    if (_current_cluster == _clusters.size() )
        return FinishedGenerating;
    else
        return ReadyToProduce;
}

JobUnit* CentersJob::produce_next_job_unit(JobUnitSize size)
{
    ProteinID last;

    Cluster& cluster( _clusters[_current_cluster]);

    if (_next_protein == 0)
        _proteins_visited.push_back(0);

    if (_next_protein + size > cluster.members().size())
        last = cluster.members().size();
    else
        last = _next_protein + size;

    JobUnit* res = new CentersJobJobUnit(_protein_db, cluster, _cutoff, cluster.geometric_mean(), _next_protein, last);

    _next_protein = last;

    if (_next_protein >= cluster.members().size())
    {
        _next_protein = 0;
        ++_current_cluster;
    }

    return res;
}

/*------- JobUnit methods -------*/

CentersJob::CentersJobJobUnit::CentersJobJobUnit(ProteinDatabase& db, Cluster& cluster, float cutoff,
    const std::vector<Coord3d>& geometric_mean, ProteinID first, ProteinID last) :
        JobUnit(),
        _bos()
{
    _bos << Centers << cutoff << cluster.get_id() << geometric_mean ;

    if (first == 0)
    {
        _bos << (last - first) + 1;
        _bos << db[ cluster.representative() ];
        set_size((last - first) + 1);
    }
    else
    {
        _bos << last - first;
        set_size(last - first);
    }

    for (ProteinID i(first); i < last; ++i)
        _bos << db[ cluster.members()[i] ];
}

const std::string& CentersJob::CentersJobJobUnit::get_message() const
{
    return _bos.str();
}
