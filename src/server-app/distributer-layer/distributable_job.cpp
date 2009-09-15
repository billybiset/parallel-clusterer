#include <iostream>

#include <boost/thread.hpp>

#include "distributable_job.h"
#include "job_manager.h"
#include "job_unit.h"

using namespace parallel_clusterer;

DistributableJob::DistributableJob() :
    _completed(),
    _job_units_generated(0),
    _completed_mutex(),
    _job_units_generated_mutex()
{
    JobManager::get_instance()->enqueue(this);
}

DistributableJobCompletedEvent::DistributableJobCompletedEvent(DistributableJobEventConsumer* interface,
                                                                DistributableJob* distjob) :
    _interface(interface),
    _distjob(distjob)
{
}

void DistributableJobCompletedEvent::call()
{
    _interface->handle_distributable_job_completed_event(_distjob);
}

void DistributableJob::distributable_job_completed_event(DistributableJob* distjob)
{
    send_event(new DistributableJobCompletedEvent(_interface,this));
}

void DistributableJob::set_listener(DistributableJobEventConsumer* const interface)
{
    _interface = interface;
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

bool DistributableJob::completion_accepted(const JobUnitID& id)
{
    boost::mutex::scoped_lock glock(_completed_mutex);

    std::set<JobUnitID>::iterator it;
    it = _completed.find(id);

    if(it != _completed.end())
        return false;
    else
    {
        _condition.notify_all();

        _completed.insert(id);

        if (finished())
            distributable_job_completed_event(this);

        return true;
    }
}
