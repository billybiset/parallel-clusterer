#ifndef DISTRIBUTABLE_JOB_H
#define DISTRIBUTABLE_JOB_H

#include "job_unit.h"

#include "common.h"

using namespace parallel_clusterer;

class DistributableJob
{
    public:
        void  run()              const;
        void  wait_completion()  const;

        virtual void      inform_completion (JobUnitID    job_unit_id) = 0;
        virtual JobUnit*  get_next_job_unit (JobUnitSize  size)        = 0;

        virtual bool  finished()            const = 0;
        virtual bool  finished_generating() const = 0;

    protected:
        DistributableJob();
};

#endif