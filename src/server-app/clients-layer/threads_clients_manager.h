#ifndef THREADS_CLIENTS_MANAGER_H
#define THREADS_CLIENTS_MANAGER_H

#include <list>

#include "clients_manager.h"
#include "job_unit.h"

namespace parallel_clusterer
{
    class ThreadsClientsManager : ClientsManager
    {
        public:
            static ThreadsClientsManager* get_instance();

            virtual ~ThreadsClientsManager(){};

            virtual bool assign_job_unit(JobUnit* job_unit);

            virtual void inform_completion(const JobUnit* job_unit);

            virtual void register_client  (Client* client);
            virtual void deregister_client(Client* client);

        private:

            ThreadsClientsManager();

            virtual Client* has_client_available_supporting(const MethodDescriptor method_name) const;

            /*Attributes*/
            static ThreadsClientsManager* _instance;
            std::list<Client *>           _registered_clients;
    };
}
#endif