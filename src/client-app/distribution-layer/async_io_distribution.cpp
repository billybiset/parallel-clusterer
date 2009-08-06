/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           async_io_distribution.cpp
    Contents:       Implementation file providing class AsyncIODistribution, which
                    will be used by the main client application and implemented 
                    with Boost.Asio.

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

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/regex.hpp>

#include <iostream>

#include <cstdlib>

#include "binstream.h"
#include "common.h"
#include "async_io_distribution.h"
#include "processors_manager.h"

#include "distribution_client.h"

using boost::asio::ip::tcp;

using namespace parallel_clusterer;

AsyncIODistribution::AsyncIODistribution() :
    _socket(0),
    DistributionClient()
{
}

void AsyncIODistribution::wait_for_job_unit()
{
    try
    {
        JobUnitID   id(1);
        JobUnitSize size(1);

        while (id != 0 && size != 0)
        {
            std::cout << "About to read." << std::endl;
            char header[HEADER_LENGTH];
            size_t received;

            boost::system::error_code ec;
//             received = _socket->receive(boost::asio::buffer(header,HEADER_LENGTH),boost::asio::transfer_all(),ec);
            received = boost::asio::read(*_socket,boost::asio::buffer(header,HEADER_LENGTH),boost::asio::transfer_all(),ec);
            if (ec)
                std::cout << "Error receiving" << std::endl;
            else
                std::cout << "Received: " << received << std::endl;

            BIStream bis(std::string(header,HEADER_LENGTH));
            bis >> id >> size;
            std::cout << "Id: " << id << ". Size: " << size << std::endl;
            if (id != 0 && size != 0)
            {
                char body[size];
                _socket->receive(boost::asio::buffer(body,size));

                ProcessorsManager::get_instance()->deliver_message(std::string(body,size));
            }
            else
                std::cout << std::endl << "Finished reading." << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "An error ocurred :( -> " << e.what() << std::endl;
    }
}

void AsyncIODistribution::inform_result(bool result)
{
    try
    {
        if (result)
        {
            std::cout << "Positive " << std::endl;
            BOStream bos;
            bos << JobUnitCompleted << ProcessorsManager::get_instance()->get_return_message();
            boost::asio::write(*_socket, boost::asio::buffer(bos.str()));
        }
        else
        {
            std::cout << "Negarchative " << std::endl;
            boost::asio::write(*_socket, boost::asio::buffer(Messages::BAD.serialize()));
        }

        std::cout << "Informing" << std::endl;
//         wait_for_job_unit();
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void AsyncIODistribution::run()
{
    std::cout << "Starting ASIO client." << std::endl;
    try
    {
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"),31337);
        _socket = new tcp::socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        _socket->connect(endpoint,error);
        if (error)
            throw boost::system::system_error(error);

        std::cout << "Connected." << std::endl;
        //state -> connected
        wait_for_job_unit();
    }
    catch (const std::exception& e)
    {
        std::cerr << "An error ocurred: " << e.what() << std::endl;
    }
}

namespace parallel_clusterer
{
    DistributionClient* create_distribution_client()
    {
        return new AsyncIODistribution();
    }
}