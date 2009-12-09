/**
 * \file  clusterer_processor.h
 * \brief Definition of ClustererProcessor class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class ClustererProcessor.
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

#ifndef CLUSTERER_PROCESSOR_H
#define CLUSTERER_PROCESSOR_H

#include "fud/fud_client.h"
#include "protein.h"

namespace fud
{
    class ClustererProcessor : ClientProcessor
    {
        public:
            ClustererProcessor();

            bool representatives_process(InputMessage& input, OutputMessage& output);
            bool clusters_process(InputMessage& input, OutputMessage& output);
            bool adding_process(InputMessage& input, OutputMessage& output);
            bool centers_process(InputMessage& input, OutputMessage& output);

            virtual bool process(InputMessage& input, OutputMessage& output);
    };
}

#endif
