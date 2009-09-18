#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <vector>
#include <list>

#include <boost/thread/mutex.hpp>

#include "distributable_job.h"
#include "job_unit.h"
#include "clients_manager.h"
#include "scheduler_events.h"

namespace parallel_clusterer
{

    struct JobManagerEventProducer
    {
        virtual void job_queue_not_full_event()        = 0;
//         virtual void job_unit_completed_event(const std::string& msg) = 0;
    };

    struct JobManagerEventConsumer
    {
        virtual void handle_job_queue_not_full_event()         = 0;
//         virtual void handle_job_unit_completed_event(const std::string& msg)  = 0;
    };

    class JobQueueNotFullEvent : public Event
    {
        public:
            JobQueueNotFullEvent(JobManagerEventConsumer* const consumer);
        private:
            virtual void call();
            JobManagerEventConsumer* _interface;
    };
/*
    class JobUnitCompletedEvent : public Event
    {
        public:
            JobUnitCompletedEvent(ClientsManagerEventConsumer* const consumer, const std::string& msg);
        private:
            virtual void call();
            ClientsManagerEventConsumer* _interface;
            const std::string&       _msg;
    };

*/
    class JobManager :
           public ClientsManagerEventConsumer,
           public JobManagerEventConsumer,
           public JobManagerEventProducer,
           public DistributableJobEventConsumer,
           public Producer,
           public Consumer<Event>
    {
        public:
            static JobManager* get_instance();

            void   enqueue(DistributableJob* distjob);

            void   start_scheduler();
            void   stop_scheduler();

            void   inform_completion(const JobUnitID id,const std::string* message);

        private:
            /* Override these, as per -Weffc++ warnings */
            JobManager(const JobManager&);
            JobManager& operator=(const JobManager&);

            enum Status {kStopped, kPaused, kRunning};

            /*take this out of here*/
            static const size_t MAX_JOBUNITS_PENDING_SIZE = 11;

            JobManager();

            /*methods*/
            void              run_scheduler();

            DistributableJob* jobs_available();
            bool              job_queue_full(); //const

            void              create_another_job_unit();

            /* handling ClientsManager events */
            void              handle_free_client_event();
            void              handle_job_unit_completed_event(const JobUnitID id, const std::string* msg);

            /* handling DistributableJob events */
            void              handle_distributable_job_completed_event(DistributableJob* distjob);

            void              check_local_events();

            /* local events*/
            void              job_queue_not_full_event();
            void              handle_job_queue_not_full_event();

            /* Attr. */
            static JobManager*              _instance;

            bool                            _clients_available;

            ClientsManager*                 _clients_manager;

            std::list<DistributableJob *>          _producingJobs;
            std::list<DistributableJob *>          _waitingJobs;
            std::list<JobUnit *>                   _jobQueue;
            std::list<JobUnit *>                   _pendingList;
            std::map<JobUnitID,DistributableJob* > _ids_to_job_map;

            Status                          _status;

            boost::mutex                    _status_mutex;
            boost::mutex                    _distJobs_mutex;
            boost::mutex                    _jobUnits_mutex;
    };
}
#endif

