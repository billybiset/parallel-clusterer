#ifndef DISTRIBUTABLE_JOB_H
#define DISTRIBUTABLE_JOB_H

#include "job_unit.h"

class DistributableJob
{
    public:
        virtual JobUnit*  get_next_job_unit (unsigned int size) = 0;
//         virtual JobUnit*  get_next_job_unit (size_t size) = 0;

        void  run()              const;
        void  wait_completion()  const;

        virtual bool  finished() const = 0;

    protected:
        DistributableJob();
};

#endif