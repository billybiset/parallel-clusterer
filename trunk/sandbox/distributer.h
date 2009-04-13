#ifndef DISTRIBUTER_H
#define DISTRIBUTER_H

#include <vector>

#include "distributable_job.h"
#include "job_unit.h"
#include "clients_manager.h"
#include "client.h"

class Distributer
{
    public:
        static Distributer* get_instance();
        void   enqueue(DistributableJob* const distjob);
        void   start_scheduler();
        void   stop_scheduler();

        void   inform_completion(JobUnitID job_unit_id);

        enum Status {kStopped, kPaused, kRunning};

    protected:

        static const unsigned int MAX_DISTJOBS_QUEUE_SIZE   = 1; /*yes, will be bigger, maybe limitless :P */
        static const unsigned int MAX_JOBUNITS_QUEUE_SIZE   = 11;
        static const unsigned int MAX_JOBUNITS_PENDING_SIZE = 11;
        static const unsigned int JOB_UNIT_SIZE             = 10;

        Distributer();

    private:
        /*methods*/

        void              run_scheduler();

        DistributableJob* jobs_available();
        bool              job_queue_full()  const;
        bool              job_queue_empty() const;

        void create_another_job_unit();

        unsigned int      next(unsigned int index) const;
        unsigned int      prev(unsigned int index) const;

        /* Attr. */
        static Distributer*             _instance;
        std::vector<DistributableJob *> _distJobs;
        std::vector<JobUnit *>          _jobQueue;       /*circular queue*/
        unsigned int                    _jobQueuesLast;  /*last non valid, initialized to 0*/
        unsigned int                    _jobQueuesFirst; /*first valid, initialized to 0*/
        std::vector<JobUnit *>          _pendingList;
        enum Status                     _status;
};

#endif