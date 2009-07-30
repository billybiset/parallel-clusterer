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

#ifndef ASYNC_IO_SERVER_H
#define ASYNC_IO_SERVER_H

#include <boost/asio.hpp>

namespace parallel_clusterer
{
    class AsyncIOServer
    {
        public:
            AsyncIOServer();
            ~AsyncIOServer() {}

            void run();
            void stop();
            void kill();

            bool send(SerializableObject msg);

        private:
            
    };
}

#endif