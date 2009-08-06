/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           client_proxy.h
    Contents:       Header file for Parallel Cluster providing abstract class
                    ClientProxy. This is a dummy proxy to represent a Client,
                    later on, it will be implemented by private Proxy classes
                    inside a concrete ClientsManager.
                    

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

#ifndef CLIENT_PROXY_H
#define CLIENT_PROXY_H

#include "common.h"
#include "job_unit.h"

namespace parallel_clusterer
{
    class ClientProxy
    {
        public:
            virtual void process(const JobUnit& job_unit) = 0;

            virtual bool busy() const = 0;

            inline ClientID get_id() const {return _id;}
        protected:
            ClientProxy();
        private:
            static ClientID _last_id;
            ClientID        _id;
    };
}

#endif