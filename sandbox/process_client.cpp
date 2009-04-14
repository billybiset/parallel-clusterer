#include <iostream> //ridiculous

#include <string>

#include <boost/thread/mutex.hpp>

#include "process_client.h"
#include "process_clients_manager.h"
#include "counter.h"

#include "waiting.h"

ProcessClient::ProcessClient()
{
//     boost::mutex::scoped_lock unlock(_status_mutex);
    ProcessClientsManager::get_instance()->register_client(this);
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

bool ProcessClient::supports_method(const MethodDescriptor& method_name) const
{
    return ! method_name.compare("count"); //strncmp(method_name,"count",5);
}

void ProcessClient::set_status(const enum Status new_status)
{
    _status = new_status;
}

bool ProcessClient::process(const JobUnit* const job_unit, const void * return_data)
{
    //assert job_unit->required_method_name == "count"

//     boost::thread::lock();
    set_status(kBusy); /*this will have a mutex or something*/
    boost::mutex::scoped_lock lock(_status_mutex);

    std::cout << "processing " << job_unit->get_id() << std::endl;
    wait(2000);
    set_status(kIdle);
    std::cout << "finishing " << job_unit->get_id() << std::endl;
    ProcessClientsManager::get_instance()->inform_completion(job_unit->get_id());

    return_data = NULL;

    return        false;
}