/**
 * \file  representatives_job.h
 * \brief Definition of RepresentativesJob class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class RepresentativesJob.
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


#ifndef REPRESENTATIVES_JOB_H
#define REPRESENTATIVES_JOB_H

#include <vector>
#include <fstream>
#include <string>

#include "fud/fud.h"
#include "protein.h"
#include "protein_database.h"

namespace clusterer
{
    using namespace fud;
    class RepresentativesJob : public DistributableJobImplementation
    {
        public:
            RepresentativesJob(ProteinDatabase& protein_db, float cutoff);

            void        output_results()      const;

            const std::vector<ProteinID>&  get_marked_ids_vector() const;

            virtual ~RepresentativesJob(){};
        private:
            virtual void                   handle_results (JobUnitID id,InputMessage& input);
            virtual JobUnit*               produce_next_job_unit(JobUnitSize size);

            virtual DistributableJobStatus get_status()    const;
            virtual const char*            get_name()      const;

            ProteinDatabase&       _protein_db;
            int                    _atoms_in_a_protein; //can this be size_t?
            std::vector<ProteinID> _marked_ids;
            ProteinID              _next_protein;
            bool                   _finished;
            float                  _cutoff;
    };
}

#endif