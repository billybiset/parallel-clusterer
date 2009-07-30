#include <iostream>

#include <boost/thread.hpp>

#include "distributable_job.h"
#include "distributer.h"
#include "job_unit.h"

using namespace parallel_clusterer;

DistributableJob::DistributableJob() :
    _completed(),
    _job_units_generated(0),
    _completed_mutex(),
    _job_units_generated_mutex()
{
    Distributer::get_instance()->enqueue(this); 
}

void DistributableJob::run() const
{
    Distributer::get_instance()->start_scheduler();
}

void DistributableJob::wait_completion()
{
    while (! finished())
        boost::this_thread::yield();
}

bool DistributableJob::finished()
{
    boost::mutex::scoped_lock glock(_completed_mutex);
    boost::mutex::scoped_lock glock2(_job_units_generated_mutex);
    return finished_generating() && _completed.size() == _job_units_generated;
}

void DistributableJob::inform_generation()
{
    boost::mutex::scoped_lock glock(_job_units_generated_mutex);
    ++_job_units_generated;
}

bool DistributableJob::completion_accepted(const JobUnitID& id)
{
    boost::mutex::scoped_lock glock(_completed_mutex);

    std::set<JobUnitID>::iterator it;
    it = _completed.find(id);

    if(it != _completed.end())
    {
        return false;
    }
    else
    {
        _completed.insert(id);
//         ++_job_units_completed; //USE _completed.size()
//         std::cout << "Gen: " << _job_units_generated << ". Compl: " << _job_units_completed <<std::endl;
        return true;
    }
}
