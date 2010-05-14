/**
 * \file  clusterer_output.cpp
 * \brief Implementation of ClustererOutput class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009,2010 Guillermo Biset, FuDePAN
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
#include "prot-filer/format_filer.h"
static const string output_format = "xtc";

using namespace GetOpt;
using namespace clusterer;

ClustererOutput::ClustererOutput(GetOpt_pp& options) throw(const char*) :
    _options(options),
    _centers_file_name(),
    _geocenters_file_name(),
    _clusters_file_name(),
    _text_file_name(),
    _stats_file_name(),
    _input_file_name()
{
    options >> Option('e',"centers",  _centers_file_name)
    >> Option('g',"geo",   _geocenters_file_name)
    >> Option('l',"clusters",_clusters_file_name)
    >> Option('o',"output",      _text_file_name)
    >> Option('s',"stats",      _stats_file_name)
    >> Option('i',"input",      _input_file_name) ;

    if((!should_output_centers() ) && (!should_output_geocenters()) &&
        (!should_output_clusters()) && (!should_output_text()) && (!should_output_stats()) )
    {
        throw("Specify an output option from [g,l,e,o,s].");
    }
}

bool ClustererOutput::should_rotalign_clusters() const
{
    bool ret;
    _options >> OptionPresent('r',"rotalign", ret);
    return ret;
}

void ClustererOutput::output_results(ProteinDatabase& protein_db,std::vector<Cluster>& clusters, float cutoff) throw(const char*)
{
    if (should_output_centers())
        output_centers(protein_db,clusters);

    if (should_output_geocenters())
        output_geocenters(protein_db,clusters);

    if (should_output_clusters())
        output_clusters(protein_db,clusters);

    if (should_output_text())
        output_text(clusters);

    if (should_output_stats())
        output_stats(protein_db, clusters,cutoff);
}

void ClustererOutput::output_centers(ProteinDatabase& protein_db,std::vector<Cluster>& clusters)
{
    FormatFiler * output = FilerFactory::get_instance()->create(output_format);

    output->open_write(_centers_file_name,protein_db.get_box(),protein_db.get_precision());

    for(size_t cluster(0); cluster < clusters.size(); ++cluster)
        output->write( protein_db[ clusters[cluster].representative() ] );

    output->close();
    delete output;
    FilerFactory::destroy_instance();
}

void ClustererOutput::output_geocenters(ProteinDatabase& protein_db,std::vector<Cluster>& clusters)
{
    FormatFiler * output = FilerFactory::get_instance()->create(output_format);
    output->open_write(_geocenters_file_name,protein_db.get_box(),protein_db.get_precision());

    for(size_t cluster(0); cluster < clusters.size(); ++cluster)
        output->write( clusters[cluster].geometric_mean() );

    output->close();
    delete output;
    FilerFactory::destroy_instance();
}

void ClustererOutput::output_clusters(ProteinDatabase& protein_db, std::vector<Cluster>& clusters)
{
    for (size_t cluster(0); cluster < clusters.size(); ++cluster)
    {
        std::stringstream file_name;
        file_name << _clusters_file_name << '_' << cluster << ".xtc";

        FormatFiler * output = FilerFactory::get_instance()->create(output_format);
        output->open_write(file_name.str(),protein_db.get_box(),protein_db.get_precision());

        output->write( protein_db[ clusters[cluster].representative() ]);

        const std::vector<ProteinID>& members( clusters[ cluster ].members() );

        for (size_t protein(0); protein < members.size() ;++protein)
        {
            if (should_rotalign_clusters())
                protein_db[ members[protein] ].rotalign_to( protein_db[ clusters[cluster].representative() ] );

            output->write( protein_db[ members[protein] ] );
        }
        output->close();
        delete output;
    }
    FilerFactory::destroy_instance();
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

void ClustererOutput::output_stats(ProteinDatabase& protein_db, std::vector<Cluster>& clusters, float cutoff)
{
    using namespace std;

    ofstream output_file;

    output_file.open (_stats_file_name.c_str());

    output_file << "Statistical information on clustering results for file " << _input_file_name << ':' << endl << endl;

    output_file << "Metrics:" << endl;
    output_file << "    Total Structures: " << protein_db.size() << endl;
    output_file << "    Used Cutoff: " << cutoff << endl;
    output_file << "    Total Clusters: " << clusters.size() << endl;
    output_file << "    Cluster Sizes:" << endl;

    size_t min_size = clusters[0].get_size();
    size_t max_size = clusters[0].get_size();
    double acum     = 0;

    for (size_t cluster(0); cluster < clusters.size(); ++cluster)
    {
        const size_t size = clusters[ cluster ].get_size();

        acum    += size;

        min_size = min(min_size, size);
        max_size = max(max_size, size);

        output_file << "        " << cluster << ". " << size << endl;
    }

    output_file << "    Min Size: " << min_size << endl;
    output_file << "    Max Size: " << max_size << endl;
    output_file << "    Average:  " << acum / clusters.size() << endl << endl;

    output_file << "Results:" << endl;

    output_file << "    Cluster Representatives:" << endl;
    for (size_t cluster(0); cluster < clusters.size(); ++cluster)
        output_file << "        " << cluster << ". " << clusters[ cluster ].representative() << endl;

    output_file << "    Maximum member-to-representative distance: ";

    float  max_dist(.0);
    size_t members_out(0);

    for (size_t i(0); i < clusters.size(); ++i)
    {
        const Cluster& cluster( clusters[i] );

        const Protein& representative( protein_db[ clusters[i].representative() ] );

        for (size_t m(0); m < cluster.members().size(); ++m)
        {
            Protein& member( protein_db[ cluster.members()[m] ] );

            const float distance = member.rmsd_to(representative);

            max_dist = max(max_dist, distance);

            if (distance > cutoff)
                ++members_out;
        }
    }

    output_file << max_dist << endl;
    output_file << "    Amount of members over cutoff: " << members_out << endl;
    output_file.close();

}