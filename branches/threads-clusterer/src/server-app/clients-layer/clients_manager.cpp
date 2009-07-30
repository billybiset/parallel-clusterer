#include <algorithm>
#include <list>
#include <syslog.h>

#include <boost/bind.hpp>
#include <boost/functional.hpp>
#include <boost/thread.hpp>

#include "distributer.h"
#include "clients_manager.h"
#include "client_proxy.h"

using namespace parallel_clusterer;

ClientsManager* ClientsManager::_instance = NULL;

ClientsManager::ClientsManager() :
    _client_proxies(),
    _client_proxies_mutex()
{
    _instance = this;
}

void ClientsManager::register_client(ClientProxy* client)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    syslog(LOG_NOTICE,"Registering client %u.",client->get_id());
    _client_proxies.push_back(client);
}

void ClientsManager::deregister_client(ClientProxy* client)
{
    boost::mutex::scoped_lock glock(_client_proxies_mutex);
    syslog(LOG_NOTICE,"Deregistering client %u.",client->get_id());
    _client_proxies.remove(client);
}

void ClientsManager::inform_completion(JobUnitID id,const std::string& message)
{
    Distributer::get_instance()->inform_completion(id,message);
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