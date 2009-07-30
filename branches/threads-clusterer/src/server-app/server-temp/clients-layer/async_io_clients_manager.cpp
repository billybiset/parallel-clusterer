/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           async_io_clients_manager.cpp
    Contents:       Implementation file for Parallel Cluster providing class 
                    AsyncIOClientsManager and embedded (private) class
                    AsyncIOClientProxy. These classes handle communication
                    with clients using Boost.Asio. Here is also implemented
                    the create_clients_manager method, later linked at compile
                    time.

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

#include <iostream> //del
#include <syslog.h>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <vector>
#include <sstream>

#include "common.h"
#include "clients_manager.h"
#include "async_io_clients_manager.h"
#include "job_unit.h"
#include "binstream.h"

using namespace parallel_clusterer;
using boost::asio::ip::tcp;

void AsyncIOClientsManager::run_server()
{
   try
    {
        tcp::acceptor acceptor(_io, tcp::endpoint(tcp::v4(), 31337));

        for (;;)
        {
            boost::mutex::scoped_lock glock(_sockets_mutex);

            tcp::socket* sock = new tcp::socket(_io);

            _sockets.push_back(sock);
            acceptor.accept(*sock);

            BOStream bos;
            bos << ConnectionSuccess;

            boost::system::error_code ec;

            boost::asio::write(*sock, boost::asio::buffer(bos.str()) ,
                boost::asio::transfer_all(), ec);

//             boost::thread sign(&AsyncIOClientsManager::register_client,this, new AsyncIOClientProxy(_sockets.back()));
            if (! ec)
                register_client(new AsyncIOClientProxy(sock));
            else
                syslog(LOG_NOTICE,"Error accepting client connection.");
        }
    }
    catch (std::exception& e)
    {
        syslog(LOG_NOTICE,"Error(run_server): %s",e.what());
        std::cerr << e.what() << std::endl;
    }
}


AsyncIOClientsManager::AsyncIOClientsManager() :
    _io(),
    _sockets(),
    _sockets_mutex()
{
    // To connect clients
    boost::thread dispatcher( boost::bind( &AsyncIOClientsManager::run_server, this));

    //To hear results
//     boost::thread listener( boost::bind( &AsyncIOClientsManager::run_listener, this));
}

void AsyncIOClientsManager::AsyncIOClientProxy::handle_response(ResponseCode code, JobUnitID id)
{
//     boost::mutex::scoped_lock glock(_proxy_mutex);
    if (code == JobUnitCompleted) // get result
    {
        char size_buf[RESPONSE_HEADER_LENGTH];
        _socket->receive(boost::asio::buffer(size_buf,RESPONSE_HEADER_LENGTH));

        BIStream bis2(std::string(size_buf,RESPONSE_HEADER_LENGTH));
        JobUnitSize size;
        bis2 >> size;
        if (size > 0)
        {
            char msg[size];
            _socket->receive(boost::asio::buffer(msg,size));
            ClientsManager::get_instance()->inform_completion(id,std::string(msg,size));
        }
    }
}

AsyncIOClientsManager::AsyncIOClientProxy::AsyncIOClientProxy(tcp::socket* socket) :
    _socket(socket),
    _state(kIdle),
    _proxy_mutex(),
    ClientProxy()
{
}

bool  AsyncIOClientsManager::assign_job_unit  (JobUnit* job_unit)
{
    ClientProxy* client(get_available_client());
    if (client != NULL)
    {
        // client->process(job_unit); //but on a different thread
        boost::thread do_job(boost::bind(&ClientProxy::process,client,job_unit));
        return true;
    }
    else
        return false;
}

void AsyncIOClientsManager::AsyncIOClientProxy::process(JobUnit* job_unit)
{
    try{
        boost::mutex::scoped_lock glock(_proxy_mutex);
        _state = kBusy;
        // send job_unit through the socket
        size_t sent;
        sent = boost::asio::write(*_socket, boost::asio::buffer(job_unit->serialize()));

        syslog(LOG_NOTICE,"Sending Job Unit %u to Client %u",job_unit->get_id(),get_id());
    }
    catch(std::exception& e)
    {
        syslog(LOG_NOTICE,"Error(process/write): %s.",e.what());
        destroy();
    }
    char code_buf[RESPONSE_HEADER_LENGTH];
    try
    {
        boost::mutex::scoped_lock glock(_proxy_mutex);
        _socket->receive(boost::asio::buffer(code_buf,RESPONSE_HEADER_LENGTH));
        BIStream bis(std::string(code_buf,RESPONSE_HEADER_LENGTH));

        ResponseCode code;
        bis >> code;
        syslog(LOG_NOTICE,"Client %u received response with code %u about Job Unit %u.",get_id(),code,job_unit->get_id());

        handle_response(code,job_unit->get_id());
        _state = kIdle;
    }
    catch(std::exception& e)
    {
        syslog(LOG_NOTICE,"Error(process/read): %s.",e.what());
        destroy();
    }

}

void AsyncIOClientsManager::AsyncIOClientProxy::destroy()
{
    boost::mutex::scoped_lock glock(_proxy_mutex);
    ClientsManager::get_instance()->deregister_client(this);
}

bool AsyncIOClientsManager::AsyncIOClientProxy::busy() const
{
    // Don't lock _proxy_mutex here!!! or it'll use only one client at a time.
    return _state == kBusy;
}

namespace parallel_clusterer
{
    ClientsManager* create_clients_manager()
    {
        return new AsyncIOClientsManager();
    }
}
