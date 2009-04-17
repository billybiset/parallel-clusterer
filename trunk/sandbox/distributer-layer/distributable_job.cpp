#include "distributable_job.h"
#include "distributer.h"
#include "job_unit.h"

DistributableJob::DistributableJob()
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
        ; //should be something like current_thread.pass()
    Distributer::get_instance()->stop_scheduler();
}