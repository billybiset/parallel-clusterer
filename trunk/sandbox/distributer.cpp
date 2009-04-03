#include "distributer.h"
#include "process_client_manager.h"
#include "client.h" /*later change to process_client.h*/

#include <iostream>

Distributer* Distributer::_instance = 0; // initialize pointer

Distributer* Distributer::get_instance () 
{
    if (_instance == 0)  // is it the first call?
    {  
        _instance = new Distributer; // create sole instance
    }
    return _instance; // address of sole instance
}

Distributer::Distributer() 
{
    _distJobs       = std::vector<DistributableJob*>(MAX_DISTJOBS_QUEUE_SIZE); /*bigger later*/
    _jobQueue       = std::vector<JobUnit*>(MAX_JOBUNITS_QUEUE_SIZE);
    _jobQueuesLast  = 0;
    _jobQueuesFirst = 0;
    _pendingList    = std::vector<JobUnit*>(MAX_JOBUNITS_PENDING_SIZE);
    _status         = kStopped;
}

DistributableJob* Distributer::jobs_available()
{
    return _distJobs[0]; //BAD: reallly!
}

/*to implement the circular queue*/
unsigned int Distributer::next(unsigned int index) const
{
    if (index == MAX_JOBUNITS_QUEUE_SIZE)
        return 0;
    else
        return index + 1;
}

/*to implement the circular queue*/
unsigned int Distributer::prev(unsigned int index) const
{
    if (index == 0)
        return MAX_JOBUNITS_QUEUE_SIZE;
    else
        return index - 1;
}

/*circular queue implementation of full*/
bool Distributer::job_queue_full() const
{
    return next(_jobQueuesLast) == _jobQueuesFirst;
}

/*circular queue implementation of empty*/
bool Distributer::job_queue_empty() const
{
    return _jobQueuesLast == _jobQueuesFirst;
}

void Distributer::stop_scheduler()
{
    _status = kStopped;
}

void Distributer::start_scheduler() /*this is truly ugly*/
{
    if (! fork())
    {
        _status = kRunning;
        while (_status != kStopped)
        {
            /*assign job unit if not empty full, generate otherwise*/
            if(! job_queue_empty()) /*this policy will be changed*/
            {
//                 std::cout << "JobQueue not empty! [" << _jobQueuesFirst << ".." << _jobQueuesLast << "]" << std::endl;

                JobUnit* job_unit = _jobQueue[_jobQueuesFirst];                   /*process a job unit from queue*/
                ProcessClientManager* cm = ProcessClientManager::get_instance();  /*Talk to the client mgr.*/

                if (cm->assign_job_unit(job_unit))
                    _jobQueuesFirst = next(_jobQueuesFirst);
                    /* send to pending , remove from queue,etc... */
//                 else
//                     std::cout << "no client avail :(" << std::endl;
            }
            else
            {
                DistributableJob* job;
                if (job = jobs_available())
                {
                    JobUnit* job_unit;
                    if ( job_unit = job->get_next_job_unit(10) )
                    {
                        //just enqueue
                        job_unit->print_info();
                        _jobQueue[_jobQueuesLast]=job_unit;
                        _jobQueuesLast = next(_jobQueuesLast);
                    }
                }
            } /*job_queue_full*/
        } /*while*/
    } /*fork*/
}

/*                    else
                    {
                        std::cout << "The DistJob " << job << " has no more job units." << std::endl;
                        _status = kStopped; //obviously this will be changed
                    }*/
/*                else
                {
                    std::cout << "no distributable jobs available." << std::endl;
                    _status = kStopped; //obviously this will be changed
                }*/


void Distributer::enqueue(DistributableJob* const distjob)
{
    _distJobs[0] = distjob;
}


