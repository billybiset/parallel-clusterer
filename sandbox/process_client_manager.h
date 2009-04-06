#ifndef PROCESS_CLIENT_MANAGER_H
#define PROCESS_CLIENT_MANAGER_H

#include <vector>

#include "client_manager.h"
#include "job_unit.h"

class ProcessClientManager : ClientManager
{
    public:
        static ProcessClientManager* get_instance();

        virtual void register_client(Client* const client);
        virtual void deregister_client(Client* const client);

        virtual bool assign_job_unit(JobUnit* const job_unit);

        virtual void inform_completion(JobUnitID job_unit_id);

    protected:

        static const unsigned int INITIAL_MAX_CLIENTS = 10; /*in boinc, i will only use an interface to it*/

        ProcessClientManager();

        virtual Client* has_client_available_supporting(const char* const method_name) const;

    private:
        static ProcessClientManager* _instance;

        std::vector<Client *>        _registered_clients;
        unsigned int                 _registered_clients_last; /*last non valid, initialized to 0*/
};

#endif