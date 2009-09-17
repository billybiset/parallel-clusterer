#include <fstream>
#include <iostream>

#include "counter.h"
#include "job_unit.h"
#include "binstream.h"

using namespace parallel_clusterer;

Counter::Counter(const char* file_name) :
    DistributableJob(),
    _total_count(0),
    _file()
{
    _file.open(file_name);
}

bool Counter::finished_generating() const
{
    return _file.eof(); // file is empty
}

Counter::CounterJobUnit::CounterJobUnit(char* message,JobUnitSize size) :
    JobUnit(),
    _message(message,size)
{
    set_size(size);
}

const std::string& Counter::CounterJobUnit::get_message() const
{
    return _message;
}

const char* Counter::get_name() const
{
    return "Counter";
}

void Counter::process_results (JobUnitID id, const std::string* message)
{
    if (completion_accepted(id)) // this line should be in DistJob.cpp somewhere
    { //deliver result
        BIStream bis(*message);
        size_t count;
        bis >> count;
        _total_count += count;
    }
}

void Counter::output_results() const
{
    std::cout << "Final count: " << _total_count << std::endl;
}

bool Counter::ready_to_produce() const
{
    return true;
}

JobUnit* Counter::get_next_job_unit(JobUnitSize size)
{
    if ( ! finished_generating() )
    {
        char message[size];
        _file.read (message, size);
        JobUnit* res = new CounterJobUnit(message,_file.gcount());

        inform_generation();
        return res;
    }
    else
        return NULL;
}
