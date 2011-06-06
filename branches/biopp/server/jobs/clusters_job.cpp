/**
 * \file  clusters_job.cpp
 * \brief Implementation file providing class ClustersJob.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 *  Contents:       Implementation file for FDC providing class ClustersJob.
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
#include "clusters_job.h"
#include "protein.h"

using namespace fud;
using namespace clusterer;

ClustersJob::ClustersJob(ProteinDatabase& protein_db, const std::vector<ProteinID>& marked, std::vector<Cluster>& clusters, float cutoff) :
    DistributableJobImplementation(),
    _protein_db(protein_db),
    _atoms_in_a_protein(protein_db.get_atom_number()),
    _finished(false),
    _clusters(clusters),
    _cutoff(cutoff),
    _next_protein(0),
    _last_cluster_id(0)
{
    for (size_t i(0); i < marked.size(); ++i)
        _clusters.push_back(Cluster(_last_cluster_id++,marked[i], _atoms_in_a_protein));
}


const char* ClustersJob::get_name() const
{
    return "ClustersJob";
}

void ClustersJob::handle_results (JobUnitID id,InputMessage& input)
{
    std::vector<std::pair<ProteinID,ProteinID> > member_to_cluster;
    input >> member_to_cluster;

    for (std::vector<std::pair<ProteinID, ProteinID> >::iterator it(member_to_cluster.begin());
         it != member_to_cluster.end();
         ++it
        )
            _clusters[it->second].add_member(it->first);
}

DistributableJobStatus ClustersJob::get_status() const
{
    if (_finished)
        return FinishedGenerating;
    else
        return ReadyToProduce;
}

JobUnit* ClustersJob::produce_next_job_unit(JobUnitSize size)
{
    if ( _finished )
        return NULL;
    else
    {
        StreamingJobUnit* res = new StreamingJobUnit();

        (*res) << Clusters;

        // Add all the proteins with marked ids
        (*res) << _cutoff << fud_uint(_clusters.size());
        for (size_t i(0); i < _clusters.size(); ++i)
        {
            const Protein& protein = _protein_db[ _clusters[i].representative() ];
            //passes the ClusterID (i) as the ProteinID...
            (*res) << protein;
        }

        const ProteinID begin = _next_protein;
        ProteinID end;

        size_t c = 0;
        Protein* p = NULL;
        while ((c < size) && ((p = _protein_db.read(c)) != NULL))
        {
            ++c;
        }
        end = begin + c;

        if (p == NULL)
            --end;

        //how many there will be?
        (*res) <<  fud_uint(end - begin);

        for (size_t i(begin); i < end; ++i)
            (*res) << _protein_db[ i ];

        res->set_size(_clusters.size() + (end - begin));

        _next_protein = end;
        _finished     = p == NULL && _next_protein >= _protein_db.size() - 1;

        return res;
    }
}

