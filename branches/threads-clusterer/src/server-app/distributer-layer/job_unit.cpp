#include "job_unit.h"

using namespace parallel_clusterer;

JobUnitID JobUnit::_last_generated = 0;

JobUnit::JobUnit() :
    _id(++_last_generated)
{
}
