/**
 * \file  clusterer_output.h
 * \brief Definition of ClustererOutput class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class ClustererOutput.
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

#ifndef CLUSTERER_OUTPUT_H
#define CLUSTERER_OUTPUT_H

#include <string>

#include "getopt_pp.h"

#include "protein_database.h"

namespace clusterer
{
    class ClustererOutput
    {
        public:
            ClustererOutput(GetOpt::GetOpt_pp& options) throw(const char*);

            void output_results(ProteinDatabase& protein_db, std::vector<Cluster>& clusters)  throw(const char*);

        private:
            void output_centers   (ProteinDatabase& protein_db, std::vector<Cluster>& clusters);
            void output_geocenters(ProteinDatabase& protein_db, std::vector<Cluster>& clusters);
            void output_clusters  (ProteinDatabase& protein_db, std::vector<Cluster>& clusters);

            void output_text(std::vector<Cluster>& clusters);

            inline bool should_output_centers()    const {return ! _centers_file_name.empty();    }
            inline bool should_output_geocenters() const {return ! _geocenters_file_name.empty(); }
            inline bool should_output_clusters()   const {return ! _clusters_file_name.empty();   }
            inline bool should_output_text()       const {return ! _text_file_name.empty();       }

            GetOpt::GetOpt_pp& _options;

            std::string _centers_file_name;
            std::string _geocenters_file_name;
            std::string _clusters_file_name;
            std::string _text_file_name;
    };
}

#endif
