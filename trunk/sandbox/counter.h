#ifndef COUNTER_H
#define COUNTER_H

#include <cstring>

#include "distributable_job.h"
#include "job_unit.h"

class Counter : public DistributableJob
{
    public:
        Counter(unsigned int amount);

        virtual bool finished() const;
//         virtual JobUnit* get_next_job_unit(size_t size);
         virtual JobUnit* get_next_job_unit(unsigned int size);

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
                virtual const char * method_name_required() const { return "count";}
            private:
                unsigned int _count_start;
                unsigned int _count_amount;
        };
};

#endif