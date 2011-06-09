/**
 * \file  clusterer_client.cpp
 * \brief Main file for the client side of the Clusterer.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Main file for the client side of the Clusterer.
 *
 * System:         FDC
 * Language:       C++
 *
 * Author:         Guillermo Biset
 * E-Mail:         billybiset AT gmail.com
 *
 * FDC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FDC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FuD.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fud/fud_client.h"
#include "clusterer_processor.h"
#include "getopt_pp.h"

using namespace fud;
using namespace GetOpt;

/* main program*/
int main(int argc, char** argv)
{
    size_t      port;
    std::string address;

    GetOpt_pp ops(argc, argv);
    ops >> Option('a', "address", address, "127.0.0.1") >> Option('p', "port", port, static_cast<size_t>(31337));

    new ClustererProcessor();

    DistributionClient* const distribution_client = create_distribution_client(address,port);
    distribution_client->run();

    return 0;
}
