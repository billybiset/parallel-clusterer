#include "process_client.h"
#include "process_client_manager.h"
#include "counter.h"

#include <iostream> //ridiculous

#include <cstring>

#include "waiting.h"

ProcessClient::ProcessClient()
{
    ProcessClientManager::get_instance()->register_client(this);
    _status = kIdle;
}

bool ProcessClient::is_idle() const
{
    return _status == kIdle;
}

bool ProcessClient::is_busy() const
{
    return _status == kIdle;
}

void ProcessClient::count(unsigned int start, unsigned int amount)
{
    std::cout << "counting from " << start << " to " << start + amount << std::endl;
}

bool ProcessClient::supports_method(const char * const method_name) const
{
    return ! strncmp(method_name,"count",5);
}

bool ProcessClient::process(const JobUnit* const job_unit, const void * return_data)
{
    //assert job_unit->required_method_name == "count"

//     count(job_unit->get_start(),job_unit->get_amount);

    _status = kBusy; /*this will have a mutex or something*/

    std::cout << "PROCESSING!!" << std::endl;
    if ( ! fork())
    {
        wait(1000);
        _status = kIdle;
    }

/*
    if (is_idle())
        std::cout << "idle" << std::endl;
    else
        std::cout << "busy" << std::endl;
*/

    return_data = 0;

    return        0;
}