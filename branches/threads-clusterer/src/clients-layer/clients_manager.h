#ifndef CLIENTS_MANAGER_H
#define CLIENTS_MANAGER_H

#include "client.h"

namespace parallel_clusterer
{
    class ClientsManager
    {
        public:
            virtual bool  assign_job_unit  (      JobUnit* job_unit) = 0;
            virtual void  inform_completion(const JobUnit* job_unit) = 0;

            virtual ~ClientsManager(){};

        protected:
            virtual void  register_client  (Client* client) = 0;
            virtual void  deregister_client(Client* client) = 0;
    };
}
#endif
