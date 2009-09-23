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
    _mutex(),
    _event_queue()
{
    _clients_manager->set_listener(this);
}

DistributableJob* JobManager::jobs_available() //will eventually change policy
{
    if (_producingJobs.empty())
        return NULL;
    else
    {
        DistributableJob* ret = _producingJobs.front();
        _producingJobs.pop_front();
        _producingJobs.push_back(ret);
        return ret;
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

void JobManager::create_another_job_unit()
{
    DistributableJob* job;

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

        if (job->finished_generating())
        {
            _producingJobs.remove(job);
            _waitingJobs.push_back(job);
        }
    }
}

void JobManager::handle_distributable_job_completed_event(DistributableJob* distjob)
{
    {
        boost::mutex::scoped_lock(_mutex);
        _producingJobs.remove(distjob);
    }
    handle_new_job_event();
    handle_job_queue_not_full_event();
    handle_free_client_event();
}

void JobManager::free_client_event()
{
    _event_queue.push(new_event(&JobManagerEventHandler::handle_free_client_event));
}

void JobManager::job_unit_completed_event(JobUnitID id, std::string* msg)
{
    _event_queue.push(new_event(&JobManagerEventHandler::handle_job_unit_completed_event, id, msg));
}

void JobManager::distributable_job_completed_event(DistributableJob* distjob)
{
    _event_queue.push(new_event(&JobManagerEventHandler::handle_distributable_job_completed_event,distjob));
}

void JobManager::handle_free_client_event()
{
    handle_new_job_event();             //check for waiting jobs to see if they can produce
    handle_job_queue_not_full_event();  //check to see if the job queue needs filling
    boost::mutex::scoped_lock(_mutex);

    if (_jobQueue.empty())
    {
        if (! _pendingList.empty())
        {
            if ( _clients_manager->assign_job_unit(*_pendingList.front()) )
            {
                //send this one to the back, act as Round Robin
                _pendingList.push_back(_pendingList.front());
                _pendingList.pop_front();
            }
            else
                syslog(LOG_NOTICE,"Error sending JobUnit %u from Pending List to a client.",_pendingList.front()->get_id());
        }
    }
    else
    {
        if ( _clients_manager->assign_job_unit(*_jobQueue.front()) )
        {
            syslog(LOG_NOTICE,"Sending JobUnit %u to pending list.",(_jobQueue.front()->get_id()) );
            _pendingList.push_back(_jobQueue.front());
            _jobQueue.pop_front();
        }
        else
            syslog(LOG_NOTICE,"Error sending JobUnit %u from Job Queue to a client.",_jobQueue.front()->get_id());
    }
    handle_job_queue_not_full_event();
}

void JobManager::handle_job_unit_completed_event(JobUnitID id, std::string* message)
{
    boost::mutex::scoped_lock(_mutex);
    syslog(LOG_NOTICE,"JobUnit %u completed.",id);

    try
    {
        //generates exception if _ids_to_job_map[id] is not defined
        mili::find(_ids_to_job_map,id)->process_results(id, message);

        //remove from pending list
        std::list<JobUnit*>::iterator it;
        it = find_if(_pendingList.begin(),_pendingList.end(),
                    boost::bind(&JobUnit::get_id, _1) == id);

        if (it != _pendingList.end())
            _pendingList.erase(it);
        else
            syslog(LOG_NOTICE,"Finished JobUnit %u was not in pending list.",id);
    }
    catch(const std::exception& e)
    {
        syslog(LOG_NOTICE,"Error: %s.",e.what());
    }
    delete message; //release the mem
}

void JobManager::run_scheduler()
{
    syslog(LOG_NOTICE,"Starting scheduler.");
    try
    {
        Event<JobManagerEventHandler>* event;
        while (_status != kStopped)
        {
            event = _event_queue.wait_for_element();
            if (event != NULL)
            {
                event->call(this);
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

void JobManager::handle_job_queue_not_full_event()
{
    boost::mutex::scoped_lock(_mutex);
    while (! job_queue_full() && ! _producingJobs.empty())
        create_another_job_unit();
}

void JobManager::start_scheduler() /*start the scheduler thread, return*/
{
    boost::mutex::scoped_lock(_mutex);
    if (_status == kStopped)
    {
        _status = kRunning;
        boost::thread thr1( boost::bind( &JobManager::run_scheduler, this ) );
    }
}

void JobManager::handle_new_job_event()
{
    boost::mutex::scoped_lock(_mutex);
    if (! _waitingJobs.empty())
    {
        std::list<DistributableJob*>::iterator it;
        for (it = _waitingJobs.begin(); it != _waitingJobs.end(); it++)
        {
            if ((*it)->ready_to_produce())
            {
                _producingJobs.push_back(*it);
                it = _waitingJobs.erase(it); //still works with list.end()
            }
        }
    }
//     if (i sent one to producing)
//         handle_free_client_event();  //mmm!?
}

void JobManager::enqueue(DistributableJob* distjob)
{
    boost::mutex::scoped_lock glock(_mutex);
    _waitingJobs.push_back(distjob);
}

