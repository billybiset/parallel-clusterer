#include <algorithm>
#include <list>
#include <syslog.h>

#include <boost/bind.hpp>
#include <boost/functional.hpp>
#include <boost/thread.hpp>

#include "clients_manager.h"
#include "client_proxy.h"

using namespace parallel_clusterer;

ClientsManager* ClientsManager::_instance = NULL;


FreeClientEvent::FreeClientEvent(ClientsManagerEventConsumer* const interface) :
    _interface(interface)
{
}

void FreeClientEvent::call()
{
    _interface->handle_free_client_event();
}

void ClientsManager::free_client_event()
{
    send_event(new FreeClientEvent(_interface));
}

JobUnitCompletedEvent::JobUnitCompletedEvent(ClientsManagerEventConsumer* const interface,
                                            const JobUnitID id, const std::string* msg) :
    _interface(interface),
    _id(id),
    _msg(msg)
{
}

void JobUnitCompletedEvent::call()
{
    _interface->handle_job_unit_completed_event(_id,_msg);
}

void ClientsManager::job_unit_completed_event(const JobUnitID id, const std::string* msg)
{
    send_event(new JobUnitCompletedEvent(_interface,id,msg));
}

ClientsManager::ClientsManager() :
    _client_proxies(),
    _client_proxies_mutex(),
    _interface(NULL)
{
    _instance = this;
}

void ClientsManager::register_client(ClientProxy* client)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    syslog(LOG_NOTICE,"Registering client %u.",client->get_id());
    _client_proxies.push_back(client);
    free_client_event();
}

void ClientsManager::deregister_client(ClientProxy* client)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    syslog(LOG_NOTICE,"Deregistering client %u.",client->get_id());
    _client_proxies.remove(client);
}

void ClientsManager::inform_completion(const JobUnitID id,const std::string* message)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    job_unit_completed_event(id, message);
    free_client_event(); //should follow this order
}


void ClientsManager::set_listener(ClientsManagerEventConsumer* const interface)
{
    _interface = interface;
}

bool ClientsManager::assign_job_unit (const JobUnit& job_unit)
{
    ClientProxy* client(get_available_client());
    if (client != NULL)
    {
        client->process(job_unit); //on the same thread, works asynchronously
//         if (client->busy())
//         {
//             syslog(LOG_NOTICE,"Pushing client %u to busy list.",client->get_id());
//             boost::mutex::scoped_lock glock(_client_proxies_mutex);
//             _free_clients.erase(client);
//             _busy_clients.insert(client);
//         }
        return true;
    }
    else
        return false;
}

ClientProxy* ClientsManager::get_available_client()
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    if (_client_proxies.size() == 0)
        return NULL;
    else
    {
        std::list<ClientProxy *>::const_iterator it;

        it = find_if (_client_proxies.begin(), _client_proxies.end(),
                    !boost::bind(&ClientProxy::busy, _1) );

        if (it != _client_proxies.end())
        {
            syslog(LOG_NOTICE,"Using client %u.",(*it)->get_id());
            return *it;
        }
        else
            return NULL;
    }
}



/* -------------------------------------------------------------------------------------------- */

/*
#include <algorithm>
#include <list>
#include <syslog.h>

#include <boost/bind.hpp>
#include <boost/functional.hpp>
#include <boost/thread.hpp>

#include "clients_manager.h"
#include "client_proxy.h"

using namespace parallel_clusterer;

ClientsManager* ClientsManager::_instance = NULL;


FreeClientEvent::FreeClientEvent(ClientsManagerEventConsumer* const interface) :
    _interface(interface)
{
}

void FreeClientEvent::call()
{
    _interface->handle_free_client_event();
}

JobUnitCompletedEvent::JobUnitCompletedEvent(ClientsManagerEventConsumer* const interface,
                                            const JobUnitID id, const std::string* msg) :
    _interface(interface),
    _id(id),
    _msg(msg)
{
}

void JobUnitCompletedEvent::call()
{
    _interface->handle_job_unit_completed_event(_id,_msg);
}

void ClientsManager::job_unit_completed_event(const JobUnitID id, const std::string* msg)
{
    send_event(new JobUnitCompletedEvent(_interface,id,msg));
}

void ClientsManager::free_client_event()
{
    send_event(new FreeClientEvent(_interface));
}

ClientsManager::ClientsManager() :
    _busy_clients(),
    _free_clients(),
    _client_proxies_mutex(),
    _ids_to_handlers(),
    _interface(NULL)
{
    _instance = this;
}

bool ClientsManager::assign_job_unit (const JobUnit& job_unit)
{
    ClientProxy* client(get_available_client());
    if (client != NULL)
    {
        _ids_to_handlers[job_unit.get_id()].insert(client);
        client->process(job_unit); //on the same thread, works asynchronously
        if (client->busy())
        {
            syslog(LOG_NOTICE,"Pushing client %u to busy list.",client->get_id());
            boost::mutex::scoped_lock glock(_client_proxies_mutex);
            _free_clients.erase(client);
            _busy_clients.insert(client);
        }
        return true;
    }
    else
        return false;
}

void ClientsManager::register_client(ClientProxy* client)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    syslog(LOG_NOTICE,"Registering client %u.",client->get_id());
    _free_clients.insert(client);
    free_client_event();
}

void ClientsManager::deregister_client(ClientProxy* client)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    syslog(LOG_NOTICE,"Deregistering client %u.",client->get_id());
    _free_clients.erase(client); //it'll be somewhere
    _busy_clients.erase(client);
}

void ClientsManager::inform_completion(const JobUnitID id,const std::string* message)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    std::set<ClientProxy*>::iterator it;

    for (it = _ids_to_handlers[id].begin(); it != _ids_to_handlers[id].end(); it++)
    {
//         *it->stop_processing(id); // Leave this... should eventually implement it
        syslog(LOG_NOTICE,"Pushing client to free: %u.",(*it)->get_id());
        _free_clients.insert(*it);
        _busy_clients.erase(*it);
    }
    job_unit_completed_event(id, message);
    free_client_event(); //should follow this order
}


void ClientsManager::set_listener(ClientsManagerEventConsumer* const interface)
{
    _interface = interface;
}

ClientProxy* ClientsManager::get_available_client()
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);

    std::set<ClientProxy *>::iterator it;
    for (it = _free_clients.begin(); it != _free_clients.end() && (*it)->busy() ; ++it)
    {
        //two logics for busyness :(
        _busy_clients.insert(*it);
        _free_clients.erase(it); //works?
    }
    if (it != _free_clients.end())
        return *it;
    else
        return NULL;

//     {
//         std::list<ClientProxy *>::const_iterator it;

//         it = find_if (_client_proxies.begin(), _client_proxies.end(),
//                     !boost::bind(&ClientProxy::busy, _1) );

//         if (it != _client_proxies.end())
//         {
//             syslog(LOG_NOTICE,"Using client %u.",(*it)->get_id());
//             return *it;
//         }
//         else
//             return NULL;
//     }

}
*/
