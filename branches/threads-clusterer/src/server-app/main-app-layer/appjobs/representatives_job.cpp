/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           representatives_job.cpp
    Contents:       Implementation file for Parallel Cluster providing 
                    class RepresentativesJob, which is a DistributableJob
                    to obtain an initial set of representatives. When
                    this job is finished, all other structures that
                    are not in the representatives set will be at a
                    distance shorter than some cutoff constant to one
                    of the representatives. Also found here is the
                    implementation of embedded (private) class 
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

#include <iostream>

#include "representatives_job.h"
#include "job_unit.h"
#include "common.h"

using namespace parallel_clusterer;

RepresentativesJob::RepresentativesJob(MarkedStructuresDB* sdb) :
    DistributableJob(),
    _db(sdb)
{
//     std::cout << "created repjob, unmarked size is " << _db->get_unmarked_size() << std::endl;
}

bool RepresentativesJob::finished_generating() const
{
    return _db->get_unmarked_size() == 0;//code
}

RepresentativesJob::RepresentativesJobUnit::RepresentativesJobUnit(std::list<Structure *> * marked_list, std::list<Structure *> * unmarked_list) : 
    JobUnit()
{
    _marked_list   = *marked_list;
    _unmarked_list = *unmarked_list;
}

JobUnit* RepresentativesJob::get_next_job_unit(JobUnitSize size)
{
    if ( ! finished_generating() )
    {
        //code here

//         std::list<Structure *> marked_list

        if (_db->get_marked_size() > size)
            throw "Marked size too big, increment max job unit size!";

        size_t size_diff(size - _db->get_marked_size());

        std::list<Structure *> * marked_list = new std::list<Structure *>(_db->get_marked_list());

//         std::list<Structure *>::iterator unmarked_list_begin = _db->get_unmarked_list().begin();

//         std::list<Structure *>::iterator unmarked_list_end = _db->get_unmarked_list().begin();

//         std::cout << "size diff is " << size_diff << " original size is " << size << " list size is " <<  _db->get_unmarked_list().size() << std::endl;

        std::list<Structure *> * unmarked_list = new std::list<Structure *>();

        size_t i;
        for (i = 0; i < size_diff; i++)
        {
            unmarked_list->push_front(_db->get_unmarked_list().front());
            _db->get_unmarked_list().pop_front();
        }

        std::cout << "created a job unit with " << marked_list->size() + unmarked_list->size() << " elements." << std::endl;

//         std::list<Structure *> unmarked_list(unmarked_list_begin,unmarked_list_end);

//         RepresentativesJob::RepresentativesJobUnit* res = new RepresentativesJob::RepresentativesJobUnit(marked_list,unmarked_list);
        RepresentativesJobUnit* res = new RepresentativesJobUnit(marked_list, unmarked_list);

        inform_generation();
        return res;
    }
    else
        return NULL;
}

