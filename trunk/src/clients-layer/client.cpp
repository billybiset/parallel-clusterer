#include "client.h"

using namespace parallel_clusterer;

Client::Client() :
    _status(kIdle),
    _status_mutex()
{
}

void Client::set_status(Status new_status)
{
    boost::mutex::scoped_lock glock(_status_mutex);
    _status = new_status;
}

inline Client::Status Client::get_status() const
{
    return _status;
}

