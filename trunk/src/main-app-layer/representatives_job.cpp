#include <iostream>

#include "representatives_job.h"
#include "job_unit.h"
#include "common.h"

using namespace parallel_clusterer;

RepresentativesJob::RepresentativesJob(Structures_DB* sdb) :
    DistributableJob(),
    _structures_db(sdb)
{
}

bool RepresentativesJob::finished_generating() const
{
    return false;//code
}

RepresentativesJob::RepresentativesJobUnit::RepresentativesJobUnit() : 
    JobUnit()
{
}

JobUnit* RepresentativesJob::get_next_job_unit(JobUnitSize size)
{
    if ( ! finished_generating() )
    {
        //code here
        
        RepresentativesJob::RepresentativesJobUnit* res = new RepresentativesJob::RepresentativesJobUnit();

        inform_generation();
        return res;
    }
    else
        return NULL;
}

