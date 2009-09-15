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

#include <list>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "common.h"
#include "clients_manager.h"
#include "client_proxy.h"
#include "job_unit.h"

using boost::asio::ip::tcp;

namespace parallel_clusterer
{
    class AsyncIOClientsManager : public ClientsManager
    {
        public:
            AsyncIOClientsManager(const size_t& port);

            ~AsyncIOClientsManager(){};
        private:
            class AsyncIOClientProxy : public ClientProxy
            {
                public:
                    AsyncIOClientProxy(boost::asio::io_service& io_service);
                    virtual ~AsyncIOClientProxy() {};

                    tcp::socket& socket();
                private:
                    virtual void process(const JobUnit& job_unit);
                    virtual bool busy() const;

                    void handle_response(ResponseCode code,JobUnitID id);

                    /* Asynchronous handlers*/
                    void handle_send   (const boost::system::error_code& ec);
                    void handle_receive(const boost::system::error_code& ec);

                    void destroy();

                    tcp::socket  _socket;
                    ClientState  _state;
                    boost::mutex _proxy_mutex;

                    char      _code_buf[RESPONSE_HEADER_LENGTH];
                    JobUnitID _current_id;
            };

            /* Asynchronous handler, calls itself recursively */
            void handle_accept (const boost::system::error_code& ec,AsyncIOClientProxy* client);

            /* To register clients */
            static void run_acceptor_thread(AsyncIOClientsManager* obj);

            void run_and_die();

            void _async_accept();

            /* attr.*/
            boost::asio::io_service _io_service;
            tcp::acceptor           _acceptor;
    };

}
#endif
