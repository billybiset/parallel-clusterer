#include "common.h"
#include "client_proxy.h"
#include "clients_manager.h"

using namespace parallel_clusterer;

ClientID ClientProxy::_last_id = 0;

ClientProxy::ClientProxy() :
    _id(++_last_id)
{
}

ClientProxy::~ClientProxy()
{
}

void ClientProxy::i_am_free()
{
    ClientsManager::get_instance()->free_client_event();
}