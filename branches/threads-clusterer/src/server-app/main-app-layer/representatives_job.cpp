#include <iostream>

#include "representatives_job.h"
#include "job_unit.h"
#include "common.h"

using namespace parallel_clusterer;

RepresentativesJob::RepresentativesJob(Structures_DB* sdb) :
    DistributableJob(),
    _db(sdb)
{
//     std::cout << "created repjob, unmarked size is " << _db->get_unmarked_size() << std::endl;
}

bool RepresentativesJob::finished_generating() const
{
    return _db->get_unmarked_size() == 0;//code
}

RepresentativesJob::RepresentativesJobUnit::RepresentativesJobUnit(std::list<Structure *> marked_list, std::list<Structure *> unmarked_list) : 
    JobUnit()
{
    _marked_list   = marked_list;
    _unmarked_list = _unmarked_list;
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

        std::list<Structure *> marked_list(_db->get_marked_list());

//         std::list<Structure *>::iterator unmarked_list_begin = _db->get_unmarked_list().begin();

//         std::list<Structure *>::iterator unmarked_list_end = _db->get_unmarked_list().begin();

//         std::cout << "size diff is " << size_diff << " original size is " << size << " list size is " <<  _db->get_unmarked_list().size() << std::endl;

        std::list<Structure *> unmarked_list();

        size_t i;
        for (i = 0; i < size_diff; i++)
        {
//             unmarked_list.push_front(_db->get_unmarked_list().front());
            _db->get_unmarked_list().pop_front();

//             std::cout << "." << std::endl;
//             ++unmarked_list_end;
        }

        std::cout << "after iters " << std::endl;

//         std::list<Structure *> unmarked_list(unmarked_list_begin,unmarked_list_end);

        RepresentativesJob::RepresentativesJobUnit* res = new RepresentativesJob::RepresentativesJobUnit(marked_list,unmarked_list);

        inform_generation();
        return res;
    }
    else
        return NULL;
}

