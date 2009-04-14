#ifndef PROCESS_CLIENTS_MANAGER_H
#define PROCESS_CLIENTS_MANAGER_H

#include <list>

#include "clients_manager.h"
#include "job_unit.h"

class ProcessClientsManager : ClientsManager
{
    public:
        static ProcessClientsManager* get_instance();

        virtual void register_client(Client* const client);
        virtual void deregister_client(Client* const client);

        virtual bool assign_job_unit(JobUnit* const job_unit);

        virtual void inform_completion(JobUnitID job_unit_id);

    protected:

        static const unsigned int INITIAL_MAX_CLIENTS = 10; /*in boinc, i will only use an interface to it*/

        ProcessClientsManager();

        virtual Client* has_client_available_supporting(const MethodDescriptor method_name) const;

    private:
        static ProcessClientsManager* _instance;

        std::list<Client *>                  _registered_clients;
        std::list<Client *>::const_iterator  _registered_clients_last; /*last non valid, initialized to 0*/
};

#endif