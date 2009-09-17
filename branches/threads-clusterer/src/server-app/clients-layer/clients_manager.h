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
#include "scheduler_events.h"

namespace parallel_clusterer
{
    struct ClientsManagerEventProducer
    {
        virtual void free_client_event()        = 0;
        virtual void job_unit_completed_event(const JobUnitID id, const std::string* msg) = 0;
    };

    struct ClientsManagerEventConsumer
    {
        virtual void handle_free_client_event()         = 0;
        virtual void handle_job_unit_completed_event(const JobUnitID id, const std::string* msg)  = 0;
    };

    class FreeClientEvent : public Event
    {
        public:
            FreeClientEvent(ClientsManagerEventConsumer* const consumer);
        private:
            virtual void call();
            ClientsManagerEventConsumer* _interface;
    };

    class JobUnitCompletedEvent : public Event
    {
        public:
            JobUnitCompletedEvent(ClientsManagerEventConsumer* const consumer,
                                    const JobUnitID id, const std::string* msg);
        private:
            virtual void call();
            ClientsManagerEventConsumer* _interface;
            const JobUnitID              _id;
            const std::string*           _msg;
    };

    class ClientsManager : public Producer, public ClientsManagerEventProducer
    {
        public:
            void inform_completion(const JobUnitID id,const std::string* message);

            void set_listener(ClientsManagerEventConsumer* const interface);

            virtual bool  assign_job_unit  (const JobUnit& job_unit);
            virtual void  deregister_client(ClientProxy* client);

            inline static ClientsManager* get_instance() {return _instance;}

        protected:
            ClientsManager();
            virtual ~ClientsManager(){};

            virtual ClientProxy* get_available_client();

            virtual void  register_client  (ClientProxy* client); //implemented here

        private:
            virtual void free_client_event();
            virtual void job_unit_completed_event(const JobUnitID id, const std::string* msg);

            std::set<ClientProxy*>          _busy_clients;
            std::set<ClientProxy*>          _free_clients;
            boost::mutex                    _client_proxies_mutex;

            std::map<JobUnitID,std::set<ClientProxy*> > _ids_to_handlers;

            static ClientsManager*          _instance;

            ClientsManagerEventConsumer*    _interface;
    };

    /**
      * To be implemented, will be linked with the apropriate concrete
      * ClientsManager's method.
      */
    ClientsManager* create_clients_manager();
}
#endif

