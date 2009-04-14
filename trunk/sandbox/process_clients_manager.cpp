#include <iostream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "process_clients_manager.h"
#include "process_client.h"
#include "client.h"
#include "distributer.h"

ProcessClientsManager* ProcessClientsManager::_instance = NULL;

ProcessClientsManager* ProcessClientsManager::get_instance () 
{
    if (_instance == NULL)  // is it the first call?
    {  
        _instance = new ProcessClientsManager; // create sole instance
    }
    return _instance; // address of sole instance
}

ProcessClientsManager::ProcessClientsManager() 
{
    _registered_clients = std::list<Client*>();
}

void ProcessClientsManager::register_client(Client* const client)
{
    _registered_clients.push_back(client);
    std::cout << "registering a new client!" << std::endl;
}

void ProcessClientsManager::deregister_client(Client* const client)
{
}

void execute(Client* client,JobUnit* job_unit,void* return_data)
{
    client->process(job_unit,return_data);
}

bool ProcessClientsManager::assign_job_unit(JobUnit* const job_unit)
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

void ProcessClientsManager::inform_completion(JobUnitID job_unit_id)
{
    Distributer::get_instance()->inform_completion(job_unit_id);
}

Client* ProcessClientsManager::has_client_available_supporting(const MethodDescriptor method_name) const 
{
    if (! _registered_clients.empty())
    {
        std::list<Client *>::const_iterator it;

        it = find_if (_registered_clients.begin(), _registered_clients.end(), 
                      boost::bind(&Client::is_idle_and_supports, _1, method_name) );

        if (it != _registered_clients.end()) 
            return *it;
        else
            return NULL;
    }
    else
        return NULL;
}
