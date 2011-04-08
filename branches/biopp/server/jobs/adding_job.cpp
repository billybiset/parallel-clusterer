/**
 * \file  adding_job.cpp
 * \brief Implementation file providing class AddingJob.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 *  Contents:       Implementation file for FDC providing class AddingJob.
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

extern "C"
{
    #include "xdrfile.h"
}

#include "mili/mili.h"
#include "adding_job.h"
#include "protein.h"

using namespace fud;
using namespace clusterer;

AddingJob::AddingJob(ProteinDatabase& protein_db, std::vector<Cluster>& clusters, float cutoff) :
    DistributableJobImplementation(),
    _protein_db(protein_db),
    _clusters(clusters),
    _atoms_in_a_protein(protein_db.get_atom_number()),
    _cutoff(cutoff),
    _current_cluster(0),
    _next_protein(0),
    _added_total_for_cluster(0)
{
}


const char* AddingJob::get_name() const
{
    return "AddingJob";
}

void AddingJob::handle_results (JobUnitID id,InputMessage& input)
{
    ClusterID cluster_id;
    size_t    added_amount;
    std::vector<Coord3d> added;

    input >> cluster_id >> added_amount >> added;

    _added_total_for_cluster[cluster_id] += added_amount;

    Cluster& cluster(_clusters[cluster_id]);

    cluster.add(added);

    if (_added_total_for_cluster[cluster_id] >= cluster.get_size())
        cluster.calculate_mean();
}

DistributableJobStatus AddingJob::get_status() const
{
    if (_current_cluster == (_clusters.size()) )
        return FinishedGenerating;
    else
        return ReadyToProduce;
}

JobUnit* AddingJob::produce_next_job_unit(JobUnitSize size)
{
    ProteinID last;

    Cluster& cluster(_clusters[_current_cluster]);

    if (_next_protein == 0)
        _added_total_for_cluster.push_back(0);

    if (_next_protein + size > cluster.members().size())
        last = cluster.members().size();
    else
        last = _next_protein + size;

    JobUnit* res = new AddingJobJobUnit(_protein_db, cluster, _cutoff, _next_protein, last);

    _next_protein = last;

    if (_next_protein >= cluster.members().size())
    {
        _next_protein = 0;
        ++_current_cluster;
    }

    return res;
}


/*------- JobUnit methods -------*/

AddingJob::AddingJobJobUnit::AddingJobJobUnit(ProteinDatabase& db, Cluster& cluster, float cutoff, ProteinID first, ProteinID last) :
    JobUnit(),
    _bos()
{
    _bos << Adding;

    if (first == 0)
    {
        _bos << cluster.get_id() << fud_uint( (last - first) + 1);
        _bos << db[ cluster.representative() ];
        set_size((last - first) + 1);
    }
    else
    {
        _bos << cluster.get_id() << fud_uint(last - first);
        set_size(last - first);
    }

    for (ProteinID i(first); i < last; ++i)
        _bos << db[ cluster.members()[i] ];
}

const std::string& AddingJob::AddingJobJobUnit::get_message() const
{
    return _bos.str();
}
