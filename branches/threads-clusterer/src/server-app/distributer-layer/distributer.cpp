#include <syslog.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/functional.hpp>

#include <boost/thread/mutex.hpp>

#include "distributer.h"
#include "clients_manager.h"
#include "distributable_job.h"

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
    _pendingList(),
    _ids_to_job_map(),
    _status(kStopped),
    _status_mutex(),
    _distJobs_mutex(),
    _jobQueue_mutex(),
    _pendingList_mutex()
{
}

DistributableJob* Distributer::jobs_available() //will eventually change policy
{
//     boost::mutex::scoped_lock glock(_distJobs_mutex);
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

void Distributer::inform_completion(const JobUnitID& id, const std::string& message)
{
    boost::mutex::scoped_lock(_pendingList_mutex);
    _ids_to_job_map[id]->process_results(id, message);

    //remove from pending list
    std::list<JobUnit*>::iterator it;
    it = find_if(_pendingList.begin(),_pendingList.end(),
                boost::bind(&JobUnit::get_id, _1) == id);

    if (it != _pendingList.end())
        _pendingList.remove(*it);
    else
        syslog(LOG_NOTICE,"Finished JobUnit %u was not in pending list.",id);
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
        {
            syslog(LOG_NOTICE,"Job %s created a Job Unit with id %u and size %u.",
                              job->get_name(),job_unit->get_id(),job_unit->get_size());

            _ids_to_job_map[job_unit->get_id()] = job;
            _jobQueue.push_back(job_unit); //just enqueue
        }
    }
}

void Distributer::run_scheduler()
{
    syslog(LOG_NOTICE,"Starting scheduler.");
    _clients_manager->initialize();
    try
    {
        while (_status != kStopped)
        {
            {
                boost::mutex::scoped_lock glock(_jobQueue_mutex);
                /*assign job unit if not empty, generate otherwise*/
                if(_jobQueue.size() < MAX_JOBUNITS_QUEUE_SIZE)
                    create_another_job_unit();

                if(! _jobQueue.empty()) /*this policy will be changed*/
                {
                    JobUnit* job_unit = _jobQueue.front();                   /*process a job unit from queue*/

                    // lock pending list before assigning, to prevent processing results before modifying it
                    boost::mutex::scoped_lock glock2(_pendingList_mutex);
                    if (_clients_manager->assign_job_unit(*job_unit))
                    {
                        _pendingList.push_back(job_unit);
                        _jobQueue.pop_front();
                    }
                }
                else
                {
                    boost::mutex::scoped_lock glock2(_pendingList_mutex);
                    if (! _pendingList.empty())
                    {
                        if (_clients_manager->assign_job_unit(*_pendingList.front()))
                        {
                            _pendingList.push_back(_pendingList.front());
                            _pendingList.pop_front();
                        }
                    }
                }
            } // release lock from job queue
            { // check to see if there is a finished job, to remove it
                boost::mutex::scoped_lock glock(_distJobs_mutex);

                std::list<DistributableJob*>::const_iterator it;
                it = find_if (_distJobs.begin(), _distJobs.end(),
                            boost::bind(&DistributableJob::finished_generating, _1) );

                if (it != _distJobs.end()) // ok, it is finished generating
                    _distJobs.remove(*it);
            }
            _clients_manager->do_tasks();
        } /*while*/
    }
    catch(const std::exception& e)
    {
        syslog(LOG_NOTICE,"Error in scheduler: %s",e.what());
        _status = kStopped;
    }
}

void Distributer::start_scheduler() /*start the scheduler thread, return*/
{
    boost::mutex::scoped_lock(_status_mutex);
    if (_status == kStopped)
    {
        _status = kRunning;
        boost::thread thr1( boost::bind( &Distributer::run_scheduler, this ) );
    }
}

void Distributer::enqueue(DistributableJob* const distjob)
{
    boost::mutex::scoped_lock glock(_distJobs_mutex);
    _distJobs.push_back(distjob);
}


