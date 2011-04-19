/**
 * \file  clusterer_main.cpp
 * \brief Main file for the server side of the Clusterer.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009,2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Main file for the server side of the Clusterer.
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

/**
 * @mainpage FDC. FuDePAN Distributed Clusterer
 *
 * @authors Guillermo Biset, Daniel Gutson
 *
 * @section intro Introduction
 *
 * FDC is a protein clusterer application that uses the <a href="http://fud.googlecode.com"> FuD </a>
 * framework for work distribution.
 *
 * The clusterer is comprised by 4 DistributableJob instances:
 * - RepresentativesJob
 * - ClustersJob
 * - AddingJob
 * - CentersJob
 *
 *
 * This project is part of
 * <A HREF="http://www.fudepan.org.ar"> FuDePAN </A>
 *
 *
 * The projects' webpage is located at
 * <A HREF="http://parallel-clusterer.googlecode.com"> GoogleCode </A>
 *
 * <hr>
 * @section notes release.notes
 * release.notes
 * <hr>
 * @section requirements requirements
 * To compile FDC, you need:
 * - GetOpt
 * - MiLi
 * - Boost libraries
 *
 * <hr>
 * @todo Many ...
 * @todo ... many things
 *
 */

#include <string>
#include <iostream>
#include <iomanip>

#include "prot-filer/format_filer.h"
#include "getopt_pp.h"

#include "protein_database.h"

#include "jobs/representatives_job.h"
#include "jobs/clusters_job.h"
#include "jobs/adding_job.h"
#include "jobs/centers_job.h"

#include "clusterer_output.h"

using namespace fud;
using namespace GetOpt;
using namespace clusterer;
using prot_filer::Coord3DReaderFactory;
using prot_filer::Coord3DSeqReaderFactory;

const size_t DEFAULT_PORT   = 31337;
const float  DEFAULT_CUTOFF = 0.15;
const size_t DEFAULT_MAX_NUMBER_OF_ELEMENTS = 100000;

const std::string DEFAULT_INPUT_FORMAT("xtc");
const std::string DEFAULT_CACHE_POLICY("protein_sliding_window");


void show_help()
{
    std::cout << "Valid options are:\n"
        "\t-c --cutoff                  Set cutoff. Default=" << DEFAULT_CUTOFF << std::endl <<
        "\t-e --centers      [optional] Output XTC file name for the centers.\n"
        "\t-g --geo          [optional] Output XTC file name for the geometric centers.\n"
        "\t-m --means        [optional] Use geometric means to calculate new cluster representatives.\n"
        "\t-h --help                    Show this help and exits.\n"
        "\t-i --input                   Input xtc file.\n"
        "\t-f --input_format [optional] Format type (compressed or xtc). Default=" << DEFAULT_INPUT_FORMAT << ".\n"
        "\t-l --clusters     [optional] Specifies the prefix of the XTC files, each per cluster, \n"
        "\t                             being the first frame of each XTC the center of the cluster,\n"
        "\t                             e.g.: \"cluster\" will generate cluster_0.xtc and so on.\n"
        "\t-o --output       [optional] Output text file, listing the number of each cluster.\n"
        "\t-s --stats        [optional] Output some statistical data to text file.\n"
        "\t-a --cache_policy [optional] Specify the cache_policy. Available options are: full_cache \n"
        "\t                             and protein_sliding_window (default, only available with \n"
        "\t                             compressed format).\n"
        "\t-M --max_cache_elements [optional] \n"
        "\t                             Specify the maximum number of elements a cache can hold. \n"
        "\t                             The restriction will be ignored when working with full_cache \n"
        "\t                             policy.\n"
        "\t                             Default: " << DEFAULT_MAX_NUMBER_OF_ELEMENTS<< "\n"
        ""
        "\n"
        "Some kind of output must be specified, so either -e, -g, -l, -o or -s shall be present.\n\n"
    ;
}


int main(int argc, char** argv)
{
    size_t port(DEFAULT_PORT);

    float cutoff(DEFAULT_CUTOFF);

    std::string input_file;
    std::string input_format(DEFAULT_INPUT_FORMAT);
    std::string cache_policy(DEFAULT_CACHE_POLICY);
    size_t max_number_of_elements(DEFAULT_MAX_NUMBER_OF_ELEMENTS);
    GetOpt_pp options(argc, argv);

    if (options >> OptionPresent('h', "help"))
        show_help();
    else
    {
        if (! (options >> OptionPresent('i',"input")) )
        {
            std::cerr << "ERROR: pleasy specify input file.\n\n";
            show_help();
        }
        else
        {
            options >> Option('p', "port", port)        >>
                       Option('c', "cutoff", cutoff)    >>
                       Option('i', "input", input_file) >>
                       Option('f', "input_format", input_format, DEFAULT_INPUT_FORMAT) >>
                       Option('a', "cache_policy", cache_policy, DEFAULT_CACHE_POLICY) >>
                       Option('M', "max_cache_elements", max_number_of_elements, DEFAULT_MAX_NUMBER_OF_ELEMENTS);

            try
            {
                ClustererOutput output(options);

                ProteinDatabase* db = new ProteinDatabase(input_format, input_file, cache_policy, max_number_of_elements );

                std::vector<Cluster> clusters;

                RepresentativesJob* repjob = new RepresentativesJob(*db,cutoff);
                repjob->run();
                repjob->wait_completion();

                ClustersJob*  clusters_job = new ClustersJob(*db, repjob->get_marked_ids_vector(), clusters, cutoff);
                clusters_job->run();
                clusters_job->wait_completion();

                if (options >> OptionPresent('m',"means") )
                {
                    AddingJob*      adding_job = new AddingJob(*db, clusters, cutoff);
                    adding_job->run();
                    adding_job->wait_completion();

                    CentersJob*      centers_job = new CentersJob(*db, clusters, cutoff);
                    centers_job->run();
                    centers_job->wait_completion();

                    std::vector<ProteinID> new_reps;
                    for (size_t i(0); i < clusters.size(); ++i)
                        new_reps.push_back(clusters[i].representative());

                    clusters.clear();

                    ClustersJob* last_run = new ClustersJob(*db,new_reps, clusters, cutoff);
                    last_run->run();
                    last_run->wait_completion();
                }

                if ( options >> OptionPresent('g', "geo")) //for output purposes
                {
                    AddingJob*   last_addition = new AddingJob(*db, clusters, cutoff);
                    last_addition->run();
                    last_addition->wait_completion();
                }

                output.output_results(*db,clusters,cutoff);

                db->close();
                delete db;
                Coord3DReaderFactory::destroy_instance();
                Coord3DSeqReaderFactory::destroy_instance();
            }
            catch(const char* msg)
            {
                std::cerr << "Error in main application: " << msg << std::endl;
            }
        }
    }
    return 0;
}
