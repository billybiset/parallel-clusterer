#include <boost/thread.hpp>

#include "distributable_job.h"
#include "distributer.h"
#include "job_unit.h"

using namespace parallel_clusterer;

DistributableJob::DistributableJob() :
    _job_units_generated(0),
    _job_units_completed(0)
{
    Distributer::get_instance()->enqueue(this); 
}

void DistributableJob::run() const
{
    Distributer::get_instance()->start_scheduler();
}

void DistributableJob::wait_completion() const
{
    while (! finished())
        boost::this_thread::yield(); //set up a boost::condition_variable , and do a wait() here on that!
    Distributer::get_instance()->stop_scheduler();
}

bool DistributableJob::finished() const
{
    return finished_generating() && _job_units_completed == _job_units_generated;
}

void DistributableJob::inform_generation()
{
    ++_job_units_generated;
}

void DistributableJob::inform_completion(JobUnitID job_unit_id)
{
    ++_job_units_completed;
}
