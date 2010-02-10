/**
 * \file  adding_job.h
 * \brief Definition of AddingJob class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class AddingJob.
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

#ifndef ADDING_JOB_H
#define ADDING_JOB_H

#include <vector>
#include <fstream>
#include <string>

#include "fud/fud.h"
#include "protein.h"
#include "cluster.h"
#include "protein_database.h"

namespace clusterer
{
    using namespace fud;
    class AddingJob : public DistributableJobImplementation
    {
        public:
            AddingJob(ProteinDatabase& protein_db, std::vector<Cluster>& clusters, float cutoff);

            void        output_results()      const;

            virtual ~AddingJob(){};
        private:
            virtual void                   handle_results (JobUnitID id,InputMessage& input);
            virtual JobUnit*               produce_next_job_unit(JobUnitSize size);

            virtual DistributableJobStatus get_status()    const;
            virtual const char*            get_name()      const;

            class AddingJobJobUnit : public JobUnit
            {
                public:
                    AddingJobJobUnit(ProteinDatabase& db, Cluster& cluster, float cutoff, ProteinID first, ProteinID last);

                private:
                    virtual ~AddingJobJobUnit(){};

                    virtual const std::string& get_message()    const;

                    OutputMessage _bos;
            };

            ProteinDatabase&            _protein_db;
            std::vector<Cluster>&       _clusters;
            int                         _atoms_in_a_protein;
            bool                        _finished;
            float                       _cutoff;
            size_t                      _current_cluster;
            ProteinID                   _next_protein;
            std::vector<size_t>         _added_total_for_cluster;
    };
}

#endif