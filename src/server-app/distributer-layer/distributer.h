#ifndef DISTRIBUTER_H
#define DISTRIBUTER_H

#include <vector>
#include <list>

#include <boost/thread/mutex.hpp>

#include "distributable_job.h"
#include "job_unit.h"
#include "clients_manager.h"

namespace parallel_clusterer
{
    class Distributer
    {
        public:
            static Distributer* get_instance();
 
            void   enqueue(DistributableJob* distjob);

            void   start_scheduler();
            void   stop_scheduler();

            void   inform_completion(const JobUnit* job_unit);

        private:

            enum Status {kStopped, kPaused, kRunning};

            static const size_t MAX_DISTJOBS_QUEUE_SIZE   = 10; /*yes, will be bigger, maybe limitless :P */
            static const size_t MAX_JOBUNITS_QUEUE_SIZE   = 11;
            static const size_t MAX_JOBUNITS_PENDING_SIZE = 11;
            static const size_t JOB_UNIT_SIZE             = 50;

            Distributer();

            /*methods*/
            void              run_scheduler();

            DistributableJob* jobs_available();
            bool              job_queue_full(); //const

            void              create_another_job_unit();

            /* Attr. */
            static Distributer*             _instance;

            ClientsManager*                 _clients_manager;

            std::list<DistributableJob *>   _distJobs;
            std::list<JobUnit *>            _jobQueue;
            std::vector<JobUnit *>          _pendingList;

            Status                          _status;

            boost::mutex                    _distJobs_mutex;
            boost::mutex                    _jobQueue_mutex;
    };
}
#endif