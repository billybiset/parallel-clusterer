#ifndef DISTRIBUTER_H
#define DISTRIBUTER_H

#include <vector>

#include "distributable_job.h"
#include "job_unit.h"

enum SchedulerStatus {kStopped, kPaused, kRunning};

class Distributer
{
    public:
        static Distributer* get_instance();
        void   enqueue(DistributableJob* const distjob);
        void   start_scheduler();
        void   stop_scheduler();
    protected:
        Distributer();
        Distributer(const Distributer&);
        Distributer& operator= (const Distributer&);
    private:
        static Distributer*           _instance;
        std::vector<DistributableJob *> _distJobs;
        std::vector<JobUnit *>          _jobQueue;
        std::vector<JobUnit *>          _pendingList;
        enum SchedulerStatus            _status;
        DistributableJob* jobs_available();
};

#endif