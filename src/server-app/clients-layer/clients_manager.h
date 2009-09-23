/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN

    This file is part of the Parallel Clusterer Project.

    File:           clients_manager.h
    Contents:       Header file for Parallel Cluster providing abstract class
                    ClientsManager, which is an interface for handling
                    registration and communication with clients. Also, here
                    the method create_clients_manager is declared, which will
                    later be implemented in the implementation file in one of
                    ClientsManager descendants.


    System:         Parallel Clusterer
    Language:       C++

    Author:         Guillermo Biset
    E-Mail:         billybiset AT gmail.com

    Parallel Clusterer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Parallel Clusterer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Parallel Clusterer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CLIENTS_MANAGER_H
#define CLIENTS_MANAGER_H

#include <set>
#include <boost/thread.hpp>

#include "client_proxy.h"
#include "job_unit.h"
#include "events.h"

namespace parallel_clusterer
{
    struct ClientsManagerListener
    {
        virtual void free_client_event()                                          = 0;
        virtual void job_unit_completed_event(JobUnitID id, std::string* message) = 0;
    };

    class ClientsManager
    {
        public:
            /* Interface for base classes, can't put it protected */
            void inform_completion(JobUnitID id, std::string* message);

            void free_client_event();

            inline static ClientsManager* get_instance() {return _instance;}

            void  deregister_client(ClientProxy* client);

            /* Interface for Job Manager */
            void set_listener(ClientsManagerListener* const listener);

            bool assign_job_unit  (const JobUnit& job_unit);

        protected:
            ClientsManager();
            virtual ~ClientsManager(){};

            virtual ClientProxy* get_available_client();

            void  register_client  (ClientProxy* client);

        private:
            /*
            std::set<ClientProxy*>          _busy_clients;
            std::set<ClientProxy*>          _free_clients;
            */
            std::list<ClientProxy*>         _client_proxies;
            boost::mutex                    _client_proxies_mutex;

            std::map<JobUnitID,std::set<ClientProxy*> > _ids_to_handlers;

            static ClientsManager*          _instance;

            ClientsManagerListener*         _listener;
    };

    /**
      * To be implemented, will be linked with the apropriate concrete
      * ClientsManager's method.
      */
    ClientsManager* create_clients_manager();
}
#endif

