#include "distributable_job.h"
#include "distributer.h"
#include "job_unit.h"

DistributableJob::DistributableJob()
{
    Distributer::get_instance()->enqueue(this); 
}

void DistributableJob::run()
{
    Distributer::get_instance()->start_scheduler();
}