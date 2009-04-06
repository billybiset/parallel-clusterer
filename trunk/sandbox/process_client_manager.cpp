#include "process_client_manager.h"
#include "process_client.h"
#include "client.h"
#include "distributer.h"

#include <iostream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

ProcessClientManager* ProcessClientManager::_instance = 0;

ProcessClientManager* ProcessClientManager::get_instance () 
{
    if (_instance == 0)  // is it the first call?
    {  
        _instance = new ProcessClientManager; // create sole instance
    }
    return _instance; // address of sole instance
}

ProcessClientManager::ProcessClientManager() 
{
    _registered_clients = std::vector<Client*>(INITIAL_MAX_CLIENTS);
    _registered_clients_last = 0;
}

void ProcessClientManager::register_client(Client* const client)
{
    if (_registered_clients_last <= INITIAL_MAX_CLIENTS)
    {
        _registered_clients[_registered_clients_last++] = client;
        std::cout << "registering a new client!" << std::endl;
    }
    else
    {
        /*raise or something*/
    }
}

void ProcessClientManager::deregister_client(Client* const client)
{
}

void execute(Client* client,JobUnit* job_unit,void* return_data)
{
    client->process(job_unit,return_data);
}

bool ProcessClientManager::assign_job_unit(JobUnit* const job_unit)
{
    Client* client;

    if (client = has_client_available_supporting(job_unit->method_name_required()))
    {
        /*create a thread to simulate processing*/
        void* return_data;
        boost::thread thr1( boost::bind(execute,client,job_unit,return_data) ); 
        return true;
    }
    else
        return false;
}

void ProcessClientManager::inform_completion(JobUnitID job_unit_id)
{
    Distributer::get_instance()->inform_completion(job_unit_id);
}

Client* ProcessClientManager::has_client_available_supporting(const char* const method_name) const 
{
//     std::cout << "Seeing if someone supports " << method_name << ". And last registered is " << _registered_clients_last << std::endl;
    if (_registered_clients_last > 0)
    {
        Client* client = _registered_clients[_registered_clients_last-1]; // :P

        if (client->is_idle())
        {
            if (client->supports_method(method_name) )
                return client; /*WTF UGLY*/
            else
                return 0;
        }
        else
            return 0;
    }
    else
        return 0;
}
