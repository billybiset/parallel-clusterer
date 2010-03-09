/**
 * \file  xtc_reader.cpp
 * \brief Implementation of XtcReader class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FDC project.
 *
 * Contents:       Implementation file for FDC providing class XtcReader.
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

#include <string>
#include <memory>

#include <syslog.h>

#include "protein.h"
#include "xtc_reader.h"

extern "C"
{
    #include "xdrfile_xtc.h"
}


using namespace clusterer;

XtcReader::XtcReader(const char* file_name) throw (const char*) :
    _finished_reading(false),
    _atoms_in_a_protein(),
    _box(),
    _precision(1000.0)
{
    // You can't always get what you want...
    std::string str(file_name);
    char *fname = new char[str.size() + 1];
    strcpy(fname,str.c_str());
    // ...(RS)

    int result;

    result = read_xtc_natoms(fname,&_atoms_in_a_protein);

    if (exdrOK != result)
        throw( "Error initializing database. Wrong filename?");
    else
        syslog(LOG_NOTICE,"Read %d atoms from file.",_atoms_in_a_protein);

    _xd = xdrfile_open(fname,"r");

    if (NULL == _xd)
        throw("Error opening file." );
}

void XtcReader::read_structure(Protein& protein)
{
    std::auto_ptr<rvec> _atoms_vector(new rvec[ _atoms_in_a_protein ] );

    int    result;
    int    step;
    float  time;
    float  prec;

    // read one protein
    result = read_xtc(_xd,_atoms_in_a_protein,&step,&time,_box,_atoms_vector.get(),&prec);

    if (step == 1) //first iteration
        _precision = prec;

    if (exdrENDOFFILE != result)
    {
        if (exdrOK != result)
            syslog(LOG_NOTICE,"ERROR: read_xtc %d",result);
        else
        {
            for (int atom_number(0); atom_number < _atoms_in_a_protein; ++atom_number)
                protein[atom_number] = Coord3d(_atoms_vector.get()[atom_number][0],
                                               _atoms_vector.get()[atom_number][1],
                                               _atoms_vector.get()[atom_number][2]);
        }
    }
    else
        _finished_reading = true;
}

float* XtcReader::get_box()
{
    return &(_box[0][0]);
}

size_t XtcReader::get_atom_number()  const
{
    return _atoms_in_a_protein;
}

float  XtcReader::get_precision()    const
{
    return _precision;
}


bool   XtcReader::finished_reading() const
{
    return _finished_reading;
}
