#include "common.h"
#include "client_proxy.h"

using namespace parallel_clusterer;

ClientID ClientProxy::_last_id = 0;

ClientProxy::ClientProxy() :
    _id(++_last_id)
{
}

ClientProxy::~ClientProxy()
{
}
