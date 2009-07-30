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

#include "string_message.h"

using namespace parallel_clusterer;

const std::string StringMessage::serialize() const
{
    return _message;
}

StringMessage::StringMessage(const std::string& str) :
    _message(str)
{
}

StringMessage::StringMessage(const char* str) :
    _message(str)
{
}