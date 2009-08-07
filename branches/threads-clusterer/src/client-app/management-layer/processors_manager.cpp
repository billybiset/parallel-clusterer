#include <iostream>

#include "distribution_client.h"
#include "processors_manager.h"
#include "client_processor.h"
#include "job_unit.h"

using namespace parallel_clusterer;

DistributionClient* DistributionClient::_instance = NULL;

ProcessorsManager* ProcessorsManager::_instance = NULL; // initialize pointer

ProcessorsManager* ProcessorsManager::get_instance () 
{
    if (_instance == NULL)  // is it the first call?
        _instance = new ProcessorsManager; // create sole instance

    return _instance; // address of sole instance
}

void ProcessorsManager::wrap_process(const std::string& message)
{
    DistributionClient::get_instance()->inform_result(_processor->process(message));
}

// bool ProcessorsManager::deliver_message(const std::string& message)
// boost::thread ProcessorsManager::deliver_message(const std::string& message)
void ProcessorsManager::deliver_message(const std::string& message)
{
//     boost::thread wrapper(boost::bind(&ProcessorsManager::wrap_process,this,message));
    wrap_process(message);
}

ProcessorsManager::ProcessorsManager() :
    _processor()
{
}

void ProcessorsManager::register_processor(ClientProcessor* client_processor)
{
    _processor = client_processor;
}

const std::string& ProcessorsManager::get_return_message() const
{
    return _processor->get_return_message();
}