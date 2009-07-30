/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           client_processor.h
    Contents:       Header file for Parallel Cluster providing class ClientProcessor.

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

#ifndef CLIENT_PROCESSOR_H
#define CLIENT_PROCESSOR_H

#include <string>
#include <boost/thread.hpp>

#include "common.h"
#include "job_unit.h"

namespace parallel_clusterer
{
    class ClientProcessor
    {
        public:
            virtual bool process(const std::string& message) = 0;

            virtual ~ClientProcessor(){}

            const std::string& get_return_message() const;

        protected:
            ClientProcessor();

            inline void register_method(const MethodDescriptor& method_name, void* method());

            std::string _return_message;
    };
}
#endif