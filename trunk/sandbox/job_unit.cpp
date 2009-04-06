#include "job_unit.h"

JobUnit::JobUnit()
{
    static unsigned long last_generated = 0;
    _id = ++last_generated;
}

unsigned long JobUnit::get_id() const
{
    return _id;
}