/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           async_io_distribution.h
    Contents:       Header file providing class AsyncIODistribution, which will
                    be used by the main client application and implemented with
                    Boost.Asio.

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

#ifndef ASYNC_IO_DISTRIBUTION_H
#define ASYNC_IO_DISTRIBUTION_H

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include "distribution_client.h"

namespace parallel_clusterer
{
    class AsyncIODistribution : public DistributionClient
    {
        public:
            AsyncIODistribution();

            virtual void run();
        private:
            virtual void inform_result(bool result);

            void wait_for_job_unit();

            /*attr*/
            tcp::socket* _socket;
    };
}

#endif