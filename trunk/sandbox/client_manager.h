#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <cstring>

#include "client.h"

struct ClientManager
{
    virtual void    register_client(Client* const client)    = 0;
    virtual void    deregister_client(Client* const client)  = 0;
    virtual bool    assign_job_unit(JobUnit* const job_unit) = 0;

};

#endif
