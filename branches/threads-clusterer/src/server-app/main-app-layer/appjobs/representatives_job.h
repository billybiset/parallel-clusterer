/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           representatives_job.h
    Contents:       Header file for Parallel Cluster providing class 
                    RepresentativesJob, which is a DistributableJob
                    to obtain an initial set of representatives. When
                    this job is finished, all other structures that
                    are not in the representatives set will be at a
                    distance shorter than some cutoff constant to one
                    of th representatives. Also found here is the
                    declaration of embedded (private) class 
                    RepresentativesJobUnit.

    System:         Parallel Clusterer
    Language:       C++
  
    Author:         Guillermo Biset
    E-Mail:         billybiset AT gmail.com
  
    Parallel Clusterer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Parallel Clusterer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Parallel Clusterer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef REPRESENTATIVES_JOB_H
#define REPRESENTATIVES_JOB_H

#include <list>

#include "marked_structuresDB.h"
#include "distributable_job.h"
#include "job_unit.h"
#include "common.h"

namespace parallel_clusterer
{

    class RepresentativesJob : public DistributableJob
    {
        public:
            RepresentativesJob(MarkedStructuresDB* sdb);

            virtual ~RepresentativesJob(){};
        private:
            virtual bool     finished_generating() const;
            virtual JobUnit* get_next_job_unit(JobUnitSize size);

            class RepresentativesJobUnit : public JobUnit
            {
                public:
                    RepresentativesJobUnit(std::list<Structure *> * marked_list, std::list<Structure *> * unmarked_list);

                private:
                    virtual const char* method_name_required() const { return "reps";}
                    virtual ~RepresentativesJobUnit(){};

                    std::list<Structure *> _marked_list;
                    std::list<Structure *> _unmarked_list;
            };

            MarkedStructuresDB* const _db;
    };
}

#endif