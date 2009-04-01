#include <iostream>

#include "counter.h"
#include "job_unit.h"

Counter::Counter(int amount)
    : DistributableJob()
{
    _amount = amount;
    _max_count = 0;
}

bool Counter::finished()
{
    return _max_count >= _amount;
}

Counter::CounterJobUnit::CounterJobUnit(int start, int amount)
{
    _count_start  = start;
    _count_amount = amount;
}

void Counter::CounterJobUnit::print_info()
{
    using namespace std;
    cout << "Count start : " << _count_start << ". Count amount : " << _count_amount << endl;
}

JobUnit* Counter::get_next_job_unit(int size)
{
    if ( ! finished() )
    {
        Counter::CounterJobUnit* res = new Counter::CounterJobUnit(_max_count,size);
        _max_count += size;

        return (JobUnit *) res;
    }
    else
        return 0;
}
