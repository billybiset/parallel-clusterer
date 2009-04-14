#include <iostream>

#include "counter.h"
#include "job_unit.h"
#include "common.h"

using namespace parallel_clusterer;

Counter::Counter(unsigned int amount)
    : DistributableJob()
{
    _amount              = amount;
    _max_count           = 0;
    _job_units_generated = 0;
    _job_units_completed = 0;

}

bool Counter::finished() const
{
    return (_max_count >= _amount) && (_job_units_completed == _job_units_generated);
}

bool Counter::finished_generating() const
{
    return (_max_count >= _amount);
}

Counter::CounterJobUnit::CounterJobUnit(unsigned int start,unsigned int amount)
    : JobUnit()
{
    _count_start  = start;
    _count_amount = amount;
}

void Counter::CounterJobUnit::print_info() const
{
    using namespace std;
    cout << "+JobUnit " << get_id() << ". Start is " << _count_start << ", amount is " << _count_amount << "." << endl;
}

JobUnit* Counter::get_next_job_unit(JobUnitSize size)
{
    if ( ! finished_generating() )
    {
        if (_max_count + size > _amount)
            size = _amount - _max_count;

        Counter::CounterJobUnit* res = new Counter::CounterJobUnit(_max_count,size);
        _max_count += size;

        ++_job_units_generated;
        return res;
    }
    else
        return NULL;
}

void Counter::inform_completion(JobUnitID job_unit_id)
{
    ++_job_units_completed;
}
