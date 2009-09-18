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
    _clients_available(false),
    _clients_manager(create_clients_manager()),
    _producingJobs(),
    _waitingJobs(),
    _jobQueue(),
    _pendingList(),
    _ids_to_job_map(),
    _status(kStopped),
    _status_mutex(),
    _distJobs_mutex(),
    _jobUnits_mutex()
{
    _clients_manager->set_listener(this); //clarify this
    _clients_manager->set_consumer(this);
    set_consumer(this); //I'm the consumer of my own events ;)
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
    /*
    {
        std::list<DistributableJob *>::iterator it;

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
    */
}

bool JobManager::job_queue_full() //const
{
//     boost::mutex::scoped_lock glock(_jobUnits_mutex);
    boost::mutex::scoped_lock(_distJobs_mutex);
    return _jobQueue.size() >= MAX_JOBUNITS_QUEUE_SIZE;
}

void JobManager::stop_scheduler()
{
    _status = kStopped;
}

void JobManager::inform_completion(const JobUnitID id, const std::string* message)
{
    syslog(LOG_NOTICE,"JobUnit %u completed.",id);
    boost::mutex::scoped_lock(_distJobs_mutex); //always follow this order
//     boost::mutex::scoped_lock(_jobUnits_mutex);

    _ids_to_job_map[id]->process_results(id, message);

    delete message; //release the mem

    //remove from pending list
    std::list<JobUnit*>::iterator it;
    it = find_if(_pendingList.begin(),_pendingList.end(),
                boost::bind(&JobUnit::get_id, _1) == id);

    if (it != _pendingList.end())
        _pendingList.erase(it);
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
//             boost::mutex::scoped_lock sl(_jobUnits_mutex);
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
    boost::mutex::scoped_lock(_distJobs_mutex);
    _producingJobs.remove(distjob);
}

void JobManager::handle_free_client_event()
{
//     std::cerr << "FC" << std::endl;
    _clients_available = true;

    bool     res(true);
//     do
//     {
//         boost::mutex::scoped_lock gloc(_jobUnits_mutex);
    boost::mutex::scoped_lock(_distJobs_mutex);
        if (_jobQueue.empty())
        {
            if (_pendingList.empty())
                res = false; //nothing to assign
            else if ( ! ( res = _clients_manager->assign_job_unit(*_pendingList.front())) )
                _clients_available = false;
        }
        else
        {
            if ( ( res = _clients_manager->assign_job_unit(*_jobQueue.front()) ) )
            {
                syslog(LOG_NOTICE,"Sending JobUnit %u to pending list.",(_jobQueue.front()->get_id()) );
                _pendingList.push_back(_jobQueue.front());
                _jobQueue.pop_front();
            }
            else
                _clients_available = false;
        }
//     } while(res);
}

void JobManager::handle_job_unit_completed_event(const JobUnitID id, const std::string* msg)
{
    inform_completion(id,msg);
}

void JobManager::run_scheduler()
{
    syslog(LOG_NOTICE,"Starting scheduler.");
    try
    {
        Event* event;
        while (_status != kStopped)
        {
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


JobQueueNotFullEvent::JobQueueNotFullEvent(JobManagerEventConsumer* const interface) :
    _interface(interface)
{
}

void JobQueueNotFullEvent::call()
{
    _interface->handle_job_queue_not_full_event();
}

void JobManager::job_queue_not_full_event()
{
    send_event(new JobQueueNotFullEvent(this));
}

void JobManager::handle_job_queue_not_full_event()
{
//     static unsigned int calls(0);
//     std::cerr << "JQ nf" << calls++ << std::endl;
    while (! job_queue_full() && ! _producingJobs.empty())
        create_another_job_unit();
}


void JobManager::check_local_events()
{
    while (true)
    {
        if (! job_queue_full())
            job_queue_not_full_event();

        if (_clients_available)
            handle_free_client_event();

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

        //in peace?
//         if ( ( job_queue_full() || _producingJobs.empty() ) && ( ! _clients_available) )
//             sleep(0.1);
    }
}

void JobManager::start_scheduler() /*start the scheduler thread, return*/
{
    boost::mutex::scoped_lock(_status_mutex);
    if (_status == kStopped)
    {
        _status = kRunning;
        boost::thread thr1( boost::bind( &JobManager::run_scheduler, this ) );
        boost::thread thr2( boost::bind( &JobManager::check_local_events, this ) );
    }
}

void JobManager::enqueue(DistributableJob* distjob)
{
    boost::mutex::scoped_lock glock(_distJobs_mutex);
    distjob->set_listener(this);
    distjob->set_consumer(this);
    _waitingJobs.push_back(distjob);
}

