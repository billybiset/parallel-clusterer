/**
 * \file  clusterer_output.cpp
 * \brief Implementation of ClustererOutput class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Implementation file for FDC providing class ClustererOutput.
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

#include <iostream>
#include <fstream>
#include <algorithm>

#include "clusterer_output.h"
#include "outputs/xtc_output.h"

using namespace GetOpt;
using namespace clusterer;

ClustererOutput::ClustererOutput(GetOpt_pp& options) throw(const char*) :
    _options(options),
    _centers_file_name(),
    _geocenters_file_name(),
    _clusters_file_name(),
    _text_file_name()
{
    options >> Option('e',"centers",  _centers_file_name)
            >> Option('g',"geo",   _geocenters_file_name)
            >> Option('l',"clusters",_clusters_file_name)
            >> Option('o',"output",      _text_file_name) ;

    if((!should_output_centers() ) && (!should_output_geocenters()) &&
       (!should_output_clusters()) && (!should_output_text()) )
    {
         throw("Specify an output option from [g,l,e,o].");
    }
}

void ClustererOutput::output_results(ProteinDatabase& protein_db,std::vector<Cluster>& clusters) throw(const char*)
{
    if (should_output_centers())
        output_centers(protein_db,clusters);

    if (should_output_geocenters())
        output_geocenters(protein_db,clusters);

    if (should_output_clusters())
        output_clusters(protein_db,clusters);

    if (should_output_text())
        output_text(clusters);
}

void ClustererOutput::output_centers(ProteinDatabase& protein_db,std::vector<Cluster>& clusters)
{
    XtcOutput output(_centers_file_name.c_str(),protein_db.get_box(),protein_db.get_precision());

    for(size_t cluster(0); cluster < clusters.size(); ++cluster)
        output.add( protein_db[ clusters[cluster].representative() ] );

    output.finish();
}

void ClustererOutput::output_geocenters(ProteinDatabase& protein_db,std::vector<Cluster>& clusters)
{
    XtcOutput output(_geocenters_file_name.c_str(),protein_db.get_box(),protein_db.get_precision());

    for(size_t cluster(0); cluster < clusters.size(); ++cluster)
        output.add( clusters[cluster].geometric_mean() );

    output.finish();
}

void ClustererOutput::output_clusters(ProteinDatabase& protein_db, std::vector<Cluster>& clusters)
{
    for (size_t cluster(0); cluster < clusters.size(); ++cluster)
    {
        std::stringstream file_name;
        file_name << _clusters_file_name << '_' << cluster << ".xtc";

        XtcOutput output(file_name.str().c_str(),protein_db.get_box(),protein_db.get_precision());

        output.add( protein_db[ clusters[cluster].representative() ]);

        const std::vector<ProteinID>& members( clusters[ cluster ].members() );

        for (size_t protein(0); protein < members.size() ;++protein)
            output.add( protein_db[ members[protein] ] );
    }
}

void ClustererOutput::output_text(std::vector<Cluster>& clusters)
{
    std::vector<ClusterID> centers;
    for (size_t cluster(0); cluster < clusters.size(); ++cluster)
        centers.push_back(clusters[cluster].representative());

    std::sort(centers.begin(), centers.end());

    std::ofstream output_file;

    output_file.open (_text_file_name.c_str());

    for (size_t cluster(0); cluster < centers.size(); ++cluster)
        output_file << centers[cluster] << std::endl;

    output_file.close();
}
