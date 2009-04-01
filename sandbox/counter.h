#ifndef COUNTER_H
#define COUNTER_H

#include "distributable_job.h"
#include "job_unit.h"

class Counter : public DistributableJob
{
    public:
        Counter(int amount);
        bool finished();
        JobUnit* get_next_job_unit(int size);

    private:
        int _max_count;
        int _amount;

        class CounterJobUnit : JobUnit
        {
            public:
                CounterJobUnit(int start, int amount);
                void print_info();
            private:
                int _count_start;
                int _count_amount;
        };
};

#endif