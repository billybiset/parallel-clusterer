#include <iostream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "threads_clients_manager.h"
#include "clusterer_threads_client.h"
#include "client.h"
#include "distributer.h"

using namespace parallel_clusterer;

ThreadsClientsManager* ThreadsClientsManager::_instance = NULL;

ThreadsClientsManager* ThreadsClientsManager::get_instance () 
{
    if (_instance == NULL)  // is it the first call?
    {  
        _instance = new ThreadsClientsManager; // create sole instance
    }
    return _instance; // address of sole instance
}

ThreadsClientsManager::ThreadsClientsManager() :
    _registered_clients()
{
}

void ThreadsClientsManager::register_client(Client* client)
{
    _registered_clients.push_back(client);
}

void ThreadsClientsManager::deregister_client(Client* client)
{
}

void execute(Client* client,JobUnit* job_unit)
{
    client->process(job_unit);
}

bool ThreadsClientsManager::assign_job_unit(JobUnit* job_unit)
{
    Client* client;

    client = has_client_available_supporting(job_unit->method_name_required());
    if ( client != NULL )
    {   /*create a thread to simulate processing*/
        boost::thread thr1( boost::bind(execute,client,job_unit) ); 
    }

    return client != NULL;
}

void ThreadsClientsManager::inform_completion(const JobUnit* job_unit)
{
    Distributer::get_instance()->inform_completion(job_unit);
}

Client* ThreadsClientsManager::has_client_available_supporting(const MethodDescriptor method_name) const 
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
