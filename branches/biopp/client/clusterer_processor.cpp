/**
 * \file  clusterer_processor.cpp
 * \brief Implementation file providing class ClustererProcessor.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 *  Contents:       Implementation file for Parallel Cluster providing class
 *                  ClustererProcessor. This type of client is designed to solve
 *                  job units from each of the concrete distributable
 *                  jobs of the clusterer.
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

#include <string>

#include <unistd.h>

#include <vector>
#include <list>

#include "mili/mili.h"

#include "clusterer_processor.h"
#include "protein.h"

using namespace fud;
using namespace clusterer;

const float Zero = 1e-4; // to handle rounding problems

ClustererProcessor::ClustererProcessor() :
    ClientProcessor()
{
}

bool ClustererProcessor::representatives_process(InputMessage& input, OutputMessage& output)
{
    float cutoff;
    std::vector<Protein> marked;
    std::vector<Protein> unmarked;
    input >> cutoff;
    input >> marked;
    input >> unmarked;

//     std:: cerr << "Read " << marked.size() << " marked, and " << unmarked.size() << " unmared." << std::endl;

    std::vector<ProteinID> marked_ids(0);

    std::vector<Protein>::iterator it;
    for (it = unmarked.begin(); it != unmarked.end(); ++it)
    {
        bool inside(false);
        std::vector<Protein>::iterator m(marked.begin());
        while ( !inside && m != marked.end())
            inside = (*it).rmsd_to(*(m++)) <= cutoff;

        if (! inside)
        {
            marked.push_back(*it);
            marked_ids.push_back((*it).get_id());
        }
    }

    output << marked_ids;
    return true;
}

bool ClustererProcessor::clusters_process(InputMessage& input, OutputMessage& output)
{
    float cutoff;
    std::vector<Protein> marked;
    std::vector<Protein> unmarked;
    input >> cutoff;
    input >> marked;
    input >> unmarked;

//     for (size_t i(0); i < marked.size(); ++i)
//         if (marked[i].atoms() != 15)
//             std::cerr << marked[i].atoms() << "!!!!!!" << std::endl;

//     for (size_t i(0); i < unmarked.size(); ++i)
//         if (unmarked[i].atoms() != 15)
//             std::cerr << unmarked[i].atoms() << "!!!!!!" << std::endl;


    // <member_id, representative_id >
    if (marked.size() != 0 && unmarked.size() != 0)
    {
        std::vector<std::pair<ProteinID,ProteinID> > member_to_cluster(0);

        std::vector<Protein>::iterator it;
        for (it = unmarked.begin(); it != unmarked.end(); ++it)
        {
            std::vector<Protein>::iterator closest(marked.begin());
            float distance( (*it).rmsd_to(*closest) );
            for (std::vector<Protein>::iterator m(closest + 1); m != marked.end(); ++m)
            {
                float new_distance( (*it).rmsd_to(*m));
                if (new_distance < distance)
                {
                    distance = new_distance;
                    closest  = m;
                }
            }
            if (distance > Zero) //to weed out duplicates
                member_to_cluster.push_back( std::pair<ProteinID,ProteinID>( (*it).get_id(), (*closest).get_id() ) );
        }

        output << member_to_cluster;
    }
    else
        output << 0u;

    return true;
}

bool ClustererProcessor::adding_process(InputMessage& input, OutputMessage& output)
{
    std::vector<Protein> cluster;
    ProteinID cluster_id;

    input >> cluster_id >> cluster;

//     std::cerr << "Adding " << cluster.size() << " proteins in cluster " << cluster_id << std::endl;

    size_t atoms(cluster[0].items());

    //initialize
    std::vector<Coord3d> added(atoms);
    for (size_t i(0); i < added.size() ; ++i)
        added[i] = Coord3d(.0,.0,.0);

    //add all these
    for (size_t i(0); i < cluster.size() ; ++i)
        for (size_t a(0); a < atoms; ++a)
            added[a] += cluster[i][a];

    output << cluster_id << cluster.size() << added;

    return true;
}

bool ClustererProcessor::centers_process(InputMessage& input, OutputMessage& output)
{
    float                cutoff;
    size_t               cluster_id; //I don't use ClusterID so I don't need to include the .h
    std::vector<Coord3d> geometric_mean;
    std::vector<Protein> cluster;

    input >> cutoff >> cluster_id >> geometric_mean >> cluster;

//     std::cerr << "Centers received cluster " << cluster_id << std::endl;

//     std::cerr << "Centers received " << cluster.size() << " proteins." << std::endl;

    size_t closest_id(0);
    float  closest_distance(cluster[0].rmsd_to(geometric_mean));

    for (size_t i(1); i < cluster.size(); ++i)
    {
        float new_distance(cluster[i].rmsd_to(geometric_mean));
        if (closest_distance > new_distance)
        {
            closest_distance = new_distance;
            closest_id       = i;
        }
    }

    output << cluster_id << cluster.size() << cluster[closest_id].get_id() << closest_distance;

    return true;
}



bool ClustererProcessor::process(InputMessage& input, OutputMessage& output)
{
    JobID job_id;
    input >> job_id;
    switch (job_id)
    {
        case Representatives : return representatives_process(input,output); break;
        case Clusters        : return clusters_process(input,output);        break;
        case Adding          : return adding_process(input,output);          break;
        case Centers         : return centers_process(input,output);         break;
    }
    return false;
}
