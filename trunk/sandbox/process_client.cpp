#include "process_client.h"
#include "process_client_manager.h"
#include "counter.h"

#include <iostream> //ridiculous

#include <cstring>

#include "waiting.h"

#include <boost/thread/mutex.hpp>

ProcessClient::ProcessClient()
{
    ProcessClientManager::get_instance()->register_client(this);
//     boost::mutex::scoped_lock unlock(_status_mutex);
    _status = kIdle;
}

bool ProcessClient::is_idle() const
{
    return _status == kIdle;
}

bool ProcessClient::is_busy() const
{
    return _status == kBusy;
}

void ProcessClient::count(unsigned int start, unsigned int amount)
{
    std::cout << "counting from " << start << " to " << start + amount << std::endl;
}

bool ProcessClient::supports_method(const char * const method_name) const
{
    return ! strncmp(method_name,"count",5);
}

void ProcessClient::set_status(const enum ClientStatus new_status)
{
    _status = new_status;
}

enum ClientStatus ProcessClient::get_status() const
{
    return _status;
}

bool ProcessClient::process(const JobUnit* const job_unit, const void * return_data)
{
    //assert job_unit->required_method_name == "count"
//     count(job_unit->get_start(),job_unit->get_amount);

//     boost::thread::lock();
    set_status(kBusy); /*this will have a mutex or something*/
    boost::mutex::scoped_lock lock(_status_mutex);

    std::cout << "processing " << job_unit->get_id() << std::endl;
    wait(2000);
    set_status(kIdle);
    std::cout << "finishing " << job_unit->get_id() << std::endl;
    ProcessClientManager::get_instance()->inform_completion(job_unit->get_id());

    return_data = 0;

    return        0;
}