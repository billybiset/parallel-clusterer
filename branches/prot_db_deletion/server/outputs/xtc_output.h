/**
 * \file  xtc_output.h
 * \brief Definition of XtcOutput class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class XtcOutput.
 *                 To generate output to GROMACS' .xtc files.
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

#ifndef XTC_OUTPUT_H
#define XTC_OUTPUT_H

#include <vector>

#include "protein.h"

extern "C"
{
    #include "xdrfile.h"
}

namespace clusterer
{
    class XtcOutput
    {
        public:
            XtcOutput(const char* file_name, const float* box, float prec) throw(const char*);

            void add(const Protein& protein);
            void add(const std::vector<Coord3d>& backbone);

            void finish();

        private:
            void take_step();

            const float _TIME_START;
            const float _TIME_STEP;

            XDRFILE* _xdr_file;
            int      _step;
            float    _time;
            matrix   _matrix;
            float    _precision;
    };
}

#endif
