#ifndef DISTRIBUTABLE_JOB_H
#define DISTRIBUTABLE_JOB_H

#include "job_unit.h"

class DistributableJob
{
    public:
        virtual JobUnit*  get_next_job_unit (int size) = 0;
        void              run();
        bool              finished();
    protected:
        DistributableJob();
};

#endif