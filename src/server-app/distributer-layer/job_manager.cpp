#include <syslog.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/functional.hpp>

#include <boost/thread/mutex.hpp>

#include "job_manager.h"
#include "clients_manager.h"
#include "distributable_job.h"

using namespace parallel_clusterer;

JobManager* JobManager::_instance = NULL; // initialize pointer

JobManager* JobManager::get_instance ()
{
    if (_instance == NULL)  // is it the first call?
    {
        _instance = new JobManager; // create sole instance
    }
    return _instance; // address of sole instance
}

JobManager::JobManager() :
    _clients_manager(create_clients_manager()),
    _producingJobs(),
    _waitingJobs(),
    _jobQueue(),
    _pendingList(),
    _ids_to_job_map(),
    _status(kStopped),
    _status_mutex(),
    _distJobs_mutex(),
    _jobQueue_mutex(),
    _pendingList_mutex()
{
    _clients_manager->set_listener(this);
    _clients_manager->set_consumer(this);
}

DistributableJob* JobManager::jobs_available() //will eventually change policy
{
//     boost::mutex::scoped_lock glock(_distJobs_mutex);
    if (_producingJobs.empty())
        return NULL;
    else
    {
        std::list<DistributableJob *>::const_iterator it;

        it = find_if (_producingJobs.begin(), _producingJobs.end(),
                      !boost::bind(&DistributableJob::finished_generating, _1) );

        if (it != _producingJobs.end())
        {
            _producingJobs.remove(*it);
            _producingJobs.push_back(*it);
            return *it;
        }
        else
            return NULL;
    }
}

bool JobManager::job_queue_full() //const
{
    return _jobQueue.size() >= MAX_JOBUNITS_QUEUE_SIZE;
}

void JobManager::stop_scheduler()
{
    _status = kStopped;
}

void JobManager::inform_completion(const JobUnitID& id, const std::string& message)
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

void JobManager::create_another_job_unit()
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

void JobManager::free_client_event()
{
    std::cerr << "Free client " << std::endl;
}

void JobManager::run_scheduler()
{
    syslog(LOG_NOTICE,"Starting scheduler.");
    _clients_manager->initialize();
    try
    {
        Event* event;
        while (_status != kStopped)
        {
            check_local_events();
            event = wait_for_event();
            if (event != NULL)
            {
                event->call();
                delete event;
            }
        } /*while*/
    }
    catch(const std::exception& e)
    {
        syslog(LOG_NOTICE,"Error in scheduler: %s",e.what());
        _status = kStopped;
    }
}

void JobManager::check_local_events()
{
    std::list<DistributableJob*>::iterator it;
    for (it = _waitingJobs.begin(); it != _waitingJobs.end(); it++)
    {
        if ((*it)->ready_to_produce())
        {
            std::cerr << "Pushing a job to the back" << std::endl;
            _producingJobs.push_back(*it);
            it = _waitingJobs.erase(it);
        }
    }
}

void JobManager::start_scheduler() /*start the scheduler thread, return*/
{
    boost::mutex::scoped_lock(_status_mutex);
    if (_status == kStopped)
    {
        _status = kRunning;
        boost::thread thr1( boost::bind( &JobManager::run_scheduler, this ) );
    }
}

void JobManager::enqueue(DistributableJob* const distjob)
{
    boost::mutex::scoped_lock glock(_distJobs_mutex);
    _waitingJobs.push_back(distjob);
}

