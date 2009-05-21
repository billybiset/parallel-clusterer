/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           async_io_clients_manager.h
    Contents:       Header file for Parallel Cluster providing class 
                    AsyncIOClientsManager and embedded (private) class
                    AsyncIOClientProxy. These classes handle communication
                    with clients using Boost.Asio.

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

#ifndef ASYNC_IO_CLIENTS_MANAGER_H
#define ASYNC_IO_CLIENTS_MANAGER_H

#include <boost/asio.hpp>

#include "clients_manager.h"
#include "client_proxy.h"
#include "job_unit.h"

using boost::asio::ip::tcp;

namespace parallel_clusterer
{
    class AsyncIOClientsManager : public ClientsManager
    {
        public:
            AsyncIOClientsManager();

            ~AsyncIOClientsManager(){};

        private:
            virtual bool  assign_job_unit  (      JobUnit* job_unit);
            virtual void  inform_completion(const JobUnit* job_unit);

            virtual void  register_client  (ClientProxy* client);
            virtual void  deregister_client(ClientProxy* client);

            class AsyncIOClientProxy : public ClientProxy
            {
                public:
                ~AsyncIOClientProxy() {};
                protected:
                private:
            };

            virtual ClientProxy* create_client_proxy();

            void run_server();

            /* attr.*/
            boost::asio::io_service _io;
    };

    ClientsManager* create_clients_manager();
}
#endif
