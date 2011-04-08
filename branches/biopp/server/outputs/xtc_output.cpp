/**
 * \file  xtc_output.h
 * \brief Implementation of XtcOutput class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Implementation file for FDC providing class XtcOutput.
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

#include "xtc_output.h"

extern "C"
{
    #include "xdrfile_xtc.h"
}


using namespace clusterer;

XtcOutput::XtcOutput(const char* file_name, const float* box, float prec) throw(const char*) :
    _TIME_START(1.0f),
    _TIME_STEP(1.0f),
    _step(1),
    _time(_TIME_START),
    _precision(prec)
{
    _xdr_file = xdrfile_open(file_name,"w");

    if (NULL == _xdr_file)
        throw("Error opening file for writing." );

    //avoid size_t because mili::square deduces int
    for (int i(0); i < mili::square(DIM); ++i)
        _matrix[i / DIM][i % DIM] = box[i];
}

void XtcOutput::add(const Protein& protein)
{
    write_xtc(_xdr_file, (int) protein.atoms(),_step, _time ,_matrix,
              const_cast<rvec*>(reinterpret_cast<const rvec*>(&protein.front() )) , _precision);
    // const_cast is safe here, after analysis.
    // The base address of the first Coord3d in the proteins works as the rvec vector.

    take_step();
}

void XtcOutput::add(const std::vector<Coord3d>& backbone)
{
    write_xtc(_xdr_file, (int) backbone.size(),_step, _time ,_matrix,
              const_cast<rvec*>(reinterpret_cast<const rvec*>(&backbone.front() )), _precision);
    // const_cast is safe here, after analysis.
    //Idem, &backbone[0] works fine

    take_step();
}

void XtcOutput::finish()
{
    xdrfile_close(_xdr_file);
}

void XtcOutput::take_step()
{
    ++_step;
    _time += _TIME_STEP;
}
