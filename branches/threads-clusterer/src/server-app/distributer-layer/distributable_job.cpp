#include <iostream>

#include <boost/thread.hpp>

#include "distributable_job.h"
#include "job_manager.h"
#include "job_unit.h"

using namespace parallel_clusterer;

DistributableJob::DistributableJob() :
    _listener(JobManager::get_instance()->get_distributable_job_listener()),
    _completed(),
    _job_units_generated(0),
    _completed_mutex(),
    _job_units_generated_mutex()
{
    JobManager::get_instance()->enqueue(this);
}


void DistributableJob::run() const
{
    JobManager::get_instance()->start_scheduler();
}

void DistributableJob::wait_completion()
{
    boost::unique_lock<boost::mutex> lock(_completed_mutex);
    while(! finished())
        _condition.wait(lock);
}

bool DistributableJob::finished()
{
    boost::mutex::scoped_lock glock2(_job_units_generated_mutex);
    return finished_generating() && _completed.size() == _job_units_generated;
}

void DistributableJob::inform_generation()
{
    boost::mutex::scoped_lock glock(_job_units_generated_mutex);
    ++_job_units_generated;
}

void DistributableJob::process_results (JobUnitID id, const std::string* message)
{
    if (completion_accepted(id))
        handle_results(id,message);

    if (finished())
    {
        _condition.notify_all();
        _listener->distributable_job_completed_event(this);
    }

}

bool DistributableJob::completion_accepted(const JobUnitID& id)
{
    boost::mutex::scoped_lock glock(_completed_mutex);

    std::set<JobUnitID>::iterator it;
    it = _completed.find(id);

    if(it != _completed.end())
        return false;
    else
    {
        _completed.insert(id);
        return true;
    }
}
