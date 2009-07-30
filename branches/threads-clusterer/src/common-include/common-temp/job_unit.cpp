#include <sstream>

#include <iostream>

#include "binstream.h"
#include "common.h"
#include "job_unit.h"

using namespace parallel_clusterer;

JobUnitID JobUnit::_last_generated = 0;

JobUnit::JobUnit() :
    _id(++_last_generated)
{
}

void  JobUnit::set_size(JobUnitSize size)
{
    _size = size;
}

/* All JobUnits are serialized the same way. */
const std::string JobUnit::serialize() const
{
    /* header + message */
    BOStream bs;
    bs << get_id() << get_message(); /*Inserting the string adds its length, completing the header.*/

    return bs.str();
}
