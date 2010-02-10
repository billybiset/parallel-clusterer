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

    for (std::vector<std::pair<ProteinID, ProteinID> >::iterator it(member_to_cluster.begin()); it != member_to_cluster.end(); ++it)
        _clusters[it->second].add_member(it->first);
}

void ClustersJob::output_results() const
{
    std::cout << "Total Clusters: " << _clusters.size() << std::endl;
    size_t acum(0);
    for (size_t i(0); i < _clusters.size(); ++i)
    {
        const Cluster& cluster( _clusters[i]);
        std::cout << "Cluster " << std::setw(4) << i << ". Representative : "  << std::setw(8) << cluster.representative() << ". Members : "  << std::setw(8) << cluster.members().size() << '.' << std::endl;
        acum += cluster.get_size();
        //uncomment to view all representatives.
//         std::cout << "Cluster  " << i << " :-----------------------------------------------------------" << std::endl;
//         std::cout << "     Representative : " << cluster.representative() << std::endl << "Members : [";
//         for (size_t m(0); m < cluster.members().size(); ++m)
//             std::cout << cluster.members()[m] << ( (m == cluster.members().size()-1) ? ']':',');
//         std::cout << std::endl;

    }
    std::cout << "Total proteins in clusters: " << acum << std::endl;
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
    std::pair<size_t,size_t> range_pos(_protein_db.generate_elements(_next_protein,size));

    if (range_pos.first != range_pos.second)
    {
        JobUnit* res = new ClustersJobJobUnit(_protein_db, _protein_db.get_iterator_pair(range_pos.first,range_pos.second) ,_cutoff,_clusters);

        _next_protein = range_pos.second;

        _finished    = _next_protein >= _protein_db.size();

        return res;
    }
    else
    {
        _finished = true;
        return NULL;
    }
}

/*------- JobUnit methods -------*/
ClustersJob::ClustersJobJobUnit::ClustersJobJobUnit(ProteinDatabase& db, IteratorRange range, float cutoff, const std::vector<Cluster>& clusters) :
    JobUnit(),
    _bos()
{
    _bos << Clusters;

    // Add all the proteins with marked ids
    _bos << cutoff << clusters.size();
    for (size_t i(0); i < clusters.size(); ++i)
        _bos << ProteinRefWithClusterID(db[clusters[i].representative()] , i); //ugly, but fast
        //passes the ClusterID (i) as the ProteinID...

    // Add proteins that are not marked or checked
    _bos << std::distance(range.first,range.second);
    for (ProteinIterator it(range.first); it != range.second; ++it)
        _bos << *it;

    set_size(clusters.size() + std::distance(range.first,range.second));

//If I don't want to send duplicates (uncomment these)
/*
    _bos << unmarked_found_in_range;

    for (size_t i(vector_slice.first); i < vector_slice.second; ++i) //if in marked vector, don't add
        if ( find(db->get_marked_ids().begin(),db->get_marked_ids().end(), i) == db->get_marked_ids().end())
            _bos << db->get_proteins()[i];

    set_size(db->get_marked_ids().size() + unmarked_found_in_range);
*/
}

const std::string& ClustersJob::ClustersJobJobUnit::get_message() const
{
    return _bos.str();
}
