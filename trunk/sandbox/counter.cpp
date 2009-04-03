#include <iostream>

#include "counter.h"
#include "job_unit.h"

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

Counter::CounterJobUnit::CounterJobUnit(unsigned int start,unsigned int amount)
{
    _count_start  = start;
    _count_amount = amount;
}

void Counter::CounterJobUnit::print_info() const
{
    using namespace std;
    cout << "+JobUnit : " << _count_start << ", " << _count_amount << endl;
}

// JobUnit* Counter::get_next_job_unit(size_t size)
 JobUnit* Counter::get_next_job_unit(unsigned int size)
{
    if ( ! finished() )
    {
        Counter::CounterJobUnit* res = new Counter::CounterJobUnit(_max_count,size);
        _max_count += size;

        return res;
    }
    else
        return 0;
}
