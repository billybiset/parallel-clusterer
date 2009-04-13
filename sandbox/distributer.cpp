#include "distributer.h"
#include "process_clients_manager.h"
#include "client.h" /*later change to process_client.h*/

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

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

void Distributer::inform_completion(JobUnitID job_unit_id)
{
    _distJobs[0]->inform_completion(job_unit_id);
}

void Distributer::create_another_job_unit()
{
    DistributableJob* job;
    if (job = jobs_available())
    {
        JobUnit* job_unit;
        if ( job_unit = job->get_next_job_unit(JOB_UNIT_SIZE) )
        {
            //just enqueue
            job_unit->print_info();
            _jobQueue[_jobQueuesLast]=job_unit;
            _jobQueuesLast = next(_jobQueuesLast);
        }
    }
}


void Distributer::run_scheduler()
{
    _status = kRunning;
    while (_status != kStopped)
    {
        /*assign job unit if not empty full, generate otherwise*/
        if(! job_queue_empty()) /*this policy will be changed*/
        {
//                 std::cout << "JobQueue not empty! [" << _jobQueuesFirst << ".." << _jobQueuesLast << "]" << std::endl;

            JobUnit* job_unit = _jobQueue[_jobQueuesFirst];                   /*process a job unit from queue*/
            ProcessClientsManager* cm = ProcessClientsManager::get_instance();  /*Talk to the client mgr.*/

            if (cm->assign_job_unit(job_unit))
                _jobQueuesFirst = next(_jobQueuesFirst);
            else if ( ! job_queue_full() )
                create_another_job_unit();
//             else
//                 std::cout << "no can do" << std::endl;

                /* send to pending , remove from queue,etc... */
//                 else
//                     std::cout << "no client avail :(" << std::endl;
        }
        else
            create_another_job_unit();
    } /*while*/
}

void Distributer::start_scheduler() /*start the scheduler thread, return*/
{
    boost::thread thr1( boost::bind( &Distributer::run_scheduler, this ) );
}

void Distributer::enqueue(DistributableJob* const distjob)
{
    _distJobs[0] = distjob; /*yeah yeah, i know*/
}


