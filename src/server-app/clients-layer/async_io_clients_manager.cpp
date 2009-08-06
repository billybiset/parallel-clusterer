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

#include <syslog.h>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "common.h"
#include "clients_manager.h"
#include "async_io_clients_manager.h"
#include "job_unit.h"
#include "binstream.h"

using namespace parallel_clusterer;
using boost::asio::ip::tcp;

AsyncIOClientsManager::AsyncIOClientsManager(const size_t& port) :
    _io_service(),
    _acceptor(_io_service, tcp::endpoint(tcp::v4(), port))
{
    // To connect clients
    boost::thread acceptor( boost::bind( &AsyncIOClientsManager::run_server, this));
}

void AsyncIOClientsManager::run_server()
{
    AsyncIOClientProxy* client = new AsyncIOClientProxy(_io_service);
    _acceptor.async_accept(client->socket(),
            boost::bind(&AsyncIOClientsManager::handle_accept,this,boost::asio::placeholders::error,client));
    _io_service.run();
}

void AsyncIOClientsManager::handle_accept(const boost::system::error_code& ec,AsyncIOClientProxy* client)
{
    if (! ec)
        register_client(client);
    else
    {
        syslog(LOG_NOTICE,"Error accepting client connection.");
        delete client;
    }
    client = new AsyncIOClientProxy(_io_service);
    _acceptor.async_accept(client->socket(),
            boost::bind(&AsyncIOClientsManager::handle_accept,this,boost::asio::placeholders::error,client));
}

AsyncIOClientsManager::AsyncIOClientProxy::AsyncIOClientProxy(boost::asio::io_service& io_service) :
    ClientProxy(),
    _socket(io_service),
    _state(kIdle),
    _proxy_mutex(),
    _code_buf(),
    _current_id(0)
{
}

void AsyncIOClientsManager::AsyncIOClientProxy::handle_response(ResponseCode code, JobUnitID id)
{
//     boost::mutex::scoped_lock glock(_proxy_mutex);
    if (code == JobUnitCompleted) // get result
    {
//         _socket.receive(boost::asio::buffer(size_buf,RESPONSE_HEADER_LENGTH));

        char size_buf[RESPONSE_HEADER_LENGTH];
        boost::asio::read(_socket,boost::asio::buffer(size_buf,RESPONSE_HEADER_LENGTH));

        JobUnitSize size;

        BIStream bis2(std::string(size_buf,RESPONSE_HEADER_LENGTH));

        bis2 >> size;
        if (size > 0)
        {
            char msg[size];
//             _socket.receive(boost::asio::buffer(msg,size));
            boost::asio::read(_socket,boost::asio::buffer(msg,size));
            ClientsManager::get_instance()->inform_completion(id,std::string(msg,size));
        }
    }
}

tcp::socket& AsyncIOClientsManager::AsyncIOClientProxy::socket()
{
    return _socket;
}

bool  AsyncIOClientsManager::assign_job_unit  (const JobUnit& job_unit)
{
    ClientProxy* client(get_available_client());
    if (client != NULL)
    {
        client->process(job_unit); //but on a different thread
        return true;
    }
    else
        return false;
}

void  AsyncIOClientsManager::do_tasks()
{
//     boost::system::error_code ec;
//     size_t st;

//     st = _io_service.run(ec);

//     if (ec)
//         syslog(LOG_NOTICE,"run error %u",st);

//     _io_service.reset();
}

void  AsyncIOClientsManager::initialize()
{
}

void AsyncIOClientsManager::AsyncIOClientProxy::handle_receive(const boost::system::error_code& ec)
{
    if (!ec)
    {
        syslog(LOG_NOTICE,"Client %u received response",get_id());
        boost::mutex::scoped_lock glock(_proxy_mutex);
        BIStream bis(std::string(_code_buf,RESPONSE_HEADER_LENGTH));

        ResponseCode code;
        bis >> code;
        syslog(LOG_NOTICE,"Client %u received response with code %u about Job Unit %u.",get_id(),code,_current_id);

        handle_response(code,_current_id);
        _state = kIdle;
    }
    else
    {
        syslog(LOG_NOTICE,"Error receiving results in client %u.",get_id());
        destroy();
    }
}

void AsyncIOClientsManager::AsyncIOClientProxy::handle_send(const boost::system::error_code& ec)
{
    if (!ec)
    {
        syslog(LOG_NOTICE,"Sending Job Unit %u to Client %u",_current_id,get_id());
        boost::asio::async_read(_socket,boost::asio::buffer(_code_buf,RESPONSE_HEADER_LENGTH),
                                boost::bind(&AsyncIOClientsManager::AsyncIOClientProxy::handle_receive,this,
                                boost::asio::placeholders::error));
    }
    else
    {
        syslog(LOG_NOTICE,"Error sending JobUnit %u to Client %u",_current_id,get_id());
        destroy();
    }
}

void AsyncIOClientsManager::AsyncIOClientProxy::process(const JobUnit& job_unit)
{
    try{
        boost::mutex::scoped_lock glock(_proxy_mutex);
        _current_id = job_unit.get_id();
        _state      = kBusy;
        boost::asio::async_write(_socket, boost::asio::buffer(job_unit.serialize()),
                                boost::bind(&AsyncIOClientsManager::AsyncIOClientProxy::handle_send,this,
                                boost::asio::placeholders::error));
    }
    catch(std::exception& e)
    {
        syslog(LOG_NOTICE,"Error(process/write): %s.",e.what());
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
        return new AsyncIOClientsManager(31337);
    }
}
