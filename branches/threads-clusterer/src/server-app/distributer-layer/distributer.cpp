#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/functional.hpp>

#include <boost/thread/mutex.hpp>

#include "distributer.h"
#include "clients_manager.h"

using namespace parallel_clusterer;

Distributer* Distributer::_instance = NULL; // initialize pointer

Distributer* Distributer::get_instance () 
{
    if (_instance == NULL)  // is it the first call?
    {  
        _instance = new Distributer; // create sole instance
    }
    return _instance; // address of sole instance
}

Distributer::Distributer() :
    _clients_manager(create_clients_manager()),
    _distJobs(),
    _jobQueue(),
    _pendingList(MAX_JOBUNITS_PENDING_SIZE),
    _status(kStopped),
    _distJobs_mutex(),
    _jobQueue_mutex()
{
}

DistributableJob* Distributer::jobs_available() //will eventually change policy
{
    if (_distJobs.empty())
        return NULL;
    else
    {
        std::list<DistributableJob *>::const_iterator it;

        it = find_if (_distJobs.begin(), _distJobs.end(), 
                      !boost::bind(&DistributableJob::finished_generating, _1) );

        if (it != _distJobs.end()) 
            return *it;
        else
            return NULL;
    }
}

bool Distributer::job_queue_full() //const
{
    return _jobQueue.size() >= MAX_JOBUNITS_QUEUE_SIZE;
}

void Distributer::stop_scheduler()
{
    _status = kStopped;
}

void Distributer::inform_completion(const JobUnit* job_unit)
{
    boost::mutex::scoped_lock glock(_distJobs_mutex);

    std::list<DistributableJob *>::const_iterator it;

    it = find_if (_distJobs.begin(), _distJobs.end(), 
                    !boost::bind(&DistributableJob::finished, _1) );

    if (it != _distJobs.end()) 
        (*it)->inform_completion(job_unit->get_id());

//     _distJobs.front() //CHANGE, TODO, see below!! (bellow?)
//     if ( _distJobs.front()->finished_generating() ) //i need to know to which distjob this j_u corresponds!!
    if ( _distJobs.front()->finished() )
        _distJobs.pop_front();
}

void Distributer::create_another_job_unit()
{
    DistributableJob* job;
    boost::mutex::scoped_lock glock(_distJobs_mutex);

    job = jobs_available();
    if ( job != NULL )
    {
        JobUnit* job_unit;
        job_unit = job->get_next_job_unit(JOB_UNIT_SIZE);
        if ( job_unit != NULL )
            _jobQueue.push_back(job_unit); //just enqueue
    }
}

void Distributer::run_scheduler()
{
    _status = kRunning;
    while (_status != kStopped)
    {
        boost::mutex::scoped_lock glock(_jobQueue_mutex);
        /*assign job unit if not empty full, generate otherwise*/
        if(! _jobQueue.empty()) /*this policy will be changed*/
        {
            JobUnit* job_unit = _jobQueue.front();                   /*process a job unit from queue*/

            if (_clients_manager->assign_job_unit(job_unit))
                _jobQueue.pop_front();
            else if ( ! job_queue_full() )
                create_another_job_unit();
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
    boost::mutex::scoped_lock glock(_distJobs_mutex);
    _distJobs.push_back(distjob);
}


