/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           client.h
    Contents:       Header file for Parallel Cluster providing class Client.

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

#ifndef CLIENT_H
#define CLIENT_H

#include <boost/thread.hpp>

#include "common.h"
#include "distribution_client.h"

namespace parallel_clusterer
{
    template <typename T>
    class Client
    {
        public:
//             virtual bool supports_method(const MethodDescriptor& method_name) const = 0;
//             virtual bool process(const JobUnit* job_unit)                           = 0;
            virtual void run()                                                      = 0;

            enum Status {kBusy, kIdle};

            inline Status get_status() const
                { return _status;}

//             inline bool is_idle_and_supports(const MethodDescriptor& method_name) const 
//                 { return _status == kIdle && supports_method(method_name); }

            virtual ~Client(){}

        protected:
            inline Client();

            void   set_status(Status new_status);

            inline void register_method(const MethodDescriptor& method_name, void (T:: *method)());

        private:
            Status       _status;
            boost::mutex _status_mutex;
            boost::mutex _busy_lock;
    };

    template<typename T> inline Client<T>::Client() :
        _status(kIdle),
        _status_mutex()
    {
    }

    template<typename T> inline void Client<T>::register_method(const MethodDescriptor&, void (T:: *method)())
    {
    }

}
#endif