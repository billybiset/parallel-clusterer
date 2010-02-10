/**
 * \file  clusters_job.h
 * \brief Definition of ClustersJob class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class ClustersJob.
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

#ifndef CLUSTERS_JOB_H
#define CLUSTERS_JOB_H

#include <vector>
#include <fstream>
#include <string>

#include "fud/fud.h"
#include "protein.h"
#include "protein_database.h"

namespace clusterer
{
    using namespace fud;
    class ClustersJob : public DistributableJobImplementation
    {
        public:
            ClustersJob(ProteinDatabase& protein_db, const std::vector<ProteinID>& marked, std::vector<Cluster>& clusters, float cutoff);

            void        output_results()       const;

            virtual ~ClustersJob(){};
        private:
            virtual void                   handle_results (JobUnitID id,InputMessage& input);
            virtual JobUnit*               produce_next_job_unit(JobUnitSize size);

            virtual DistributableJobStatus get_status()    const;
            virtual const char*            get_name()      const;

            class ClustersJobJobUnit : public JobUnit
            {
                public:
                    ClustersJobJobUnit(ProteinDatabase& db, IteratorRange range, float cutoff, const std::vector<Cluster>& clusters);

                private:
                    virtual ~ClustersJobJobUnit(){};

                    virtual const std::string& get_message()    const;

                    OutputMessage _bos;
            };

            ProteinDatabase&       _protein_db;
            int                    _atoms_in_a_protein;
            bool                   _finished;
            std::vector<Cluster>&  _clusters;
            float                  _cutoff;

            ProteinID              _next_protein;
            size_t                 _last_cluster_id;
    };
}

#endif