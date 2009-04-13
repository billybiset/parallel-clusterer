#include "job_unit.h"

JobUnitID JobUnit::_last_generated = 0;

JobUnit::JobUnit()
{
//     private static JobUnitID _last_generated = 0;
    _id = ++_last_generated;
}

JobUnitID JobUnit::get_id() const
{
    return _id;
}