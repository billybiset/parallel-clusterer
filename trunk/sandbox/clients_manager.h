#ifndef CLIENTS_MANAGER_H
#define CLIENTS_MANAGER_H

#include "client.h"

class ClientsManager
{
    public:
        virtual bool    assign_job_unit  (JobUnit* const job_unit)       = 0;
        virtual void    inform_completion(JobUnitID      job_unit_id)    = 0;
    protected:
        virtual void    register_client  (Client*  const client)         = 0;
        virtual void    deregister_client(Client*  const client)         = 0;

};

#endif
