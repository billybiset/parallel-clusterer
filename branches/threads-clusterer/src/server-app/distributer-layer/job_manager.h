#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <vector>
#include <list>

#include <boost/thread/mutex.hpp>

#include "distributable_job.h"
#include "job_unit.h"
#include "clients_manager.h"
#include "events.h"
#include "synchronized_containers.h"

namespace parallel_clusterer
{
    struct JobManagerEventHandler
    {
        //ClientsManager events
        virtual void handle_free_client_event()                                          = 0;
        virtual void handle_job_unit_completed_event(JobUnitID* id, std::string* msg)    = 0;

        //DistributableJob events
        virtual void handle_distributable_job_completed_event(DistributableJob* distjob) = 0;
    };

    class JobManager :
        private ClientsManagerListener,
        private DistributableJobListener,
        private JobManagerEventHandler
    {
        public:
            static JobManager* get_instance();

            void   enqueue(DistributableJob* distjob);

            void   start_scheduler();
            void   stop_scheduler();

        private:
            /* Override these, as per -Weffc++ warnings */
            JobManager(const JobManager&);
            JobManager& operator=(const JobManager&);

            enum Status {kStopped, kPaused, kRunning};

            JobManager();

            /*methods*/
            void              run_scheduler();

            DistributableJob* jobs_available();
            bool              job_queue_full(); //const

            void              create_another_job_unit();

            void              inform_completion(const JobUnitID id,const std::string* message);

            /* Enqueuing ClientsManager events */
            virtual void      free_client_event();
            virtual void      job_unit_completed_event(JobUnitID* id, std::string* msg);

            /* Enqueuing DistributableJob events */
            virtual void      distributable_job_completed_event(DistributableJob* distjob);


            /* handling ClientsManager events */
            virtual void handle_free_client_event();
            virtual void handle_job_unit_completed_event(JobUnitID* id, std::string* msg);

            /* handling DistributableJob events */
            virtual void handle_distributable_job_completed_event(DistributableJob* distjob);


            void              handle_new_job_event();

            /* local events*/
            void              job_queue_not_full_event();
            void              handle_job_queue_not_full_event();

            /* Attr. */
            static JobManager*              _instance;

            ClientsManager*                 _clients_manager;

            std::list<DistributableJob *>          _producingJobs;
            std::list<DistributableJob *>          _waitingJobs;
            std::list<JobUnit *>                   _jobQueue;
            std::list<JobUnit *>                   _pendingList;
            std::map<JobUnitID,DistributableJob* > _ids_to_job_map;

            Status                          _status;

            boost::mutex                    _mutex;

            LockingQueue<DeferredEvent<JobManagerEventHandler> *>    _event_queue;
    };
}
#endif

