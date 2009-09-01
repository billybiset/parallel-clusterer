#ifndef JOB_MANAGER_H
#define JOB_MANAGER_H

#include <vector>
#include <list>

#include <boost/thread/mutex.hpp>

#include "distributable_job.h"
#include "job_unit.h"
#include "clients_manager.h"

namespace parallel_clusterer
{
    class JobManager : private ClientsManager::ClientsListener
    {
        public:
            static JobManager* get_instance();

            void   enqueue(DistributableJob* distjob);

            void   start_scheduler();
            void   stop_scheduler();

            void   inform_completion(const JobUnitID& id,const std::string& message);
            void   client_is_free();

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

            /* Attr. */
            static JobManager*             _instance;

            ClientsManager*                 _clients_manager;

            std::list<DistributableJob *>         _distJobs;
            std::list<JobUnit *>                  _jobQueue;
            std::list<JobUnit *>                  _pendingList;
            std::map<JobUnitID,DistributableJob*> _ids_to_job_map;

            Status                          _status;

            boost::mutex                    _status_mutex;
            boost::mutex                    _distJobs_mutex;
            boost::mutex                    _jobQueue_mutex;
            boost::mutex                    _pendingList_mutex;
    };
}
#endif