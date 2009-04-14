#ifndef COUNTER_H
#define COUNTER_H

#include "distributable_job.h"
#include "job_unit.h"
#include "common.h"

using namespace parallel_clusterer;

class Counter : public DistributableJob
{
    public:
        Counter(unsigned int amount);

        virtual bool finished() const;
        virtual bool finished_generating() const;

        virtual JobUnit* get_next_job_unit(JobUnitSize size);

        virtual void  inform_completion(JobUnitID job_unit_id);

    private:
        unsigned int _max_count;
        unsigned int _amount;

        unsigned int _job_units_generated;
        unsigned int _job_units_completed;

        class CounterJobUnit : public JobUnit
        {
            public:
                CounterJobUnit(unsigned int start,unsigned int amount);
                virtual void print_info() const;
                virtual const char* method_name_required() const { return "count";}

            private:
                unsigned int      _count_start;
                unsigned int      _count_amount;
        };
};

#endif