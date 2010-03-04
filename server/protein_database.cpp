/**
 * \file  protein_database.cpp
 * \brief Implementation file providing class ProteinDatabase.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009,2010 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 *  Contents:       Implementation file for FDC providing class ProteinDatabase.
 *
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

#include <syslog.h>
#include <memory>

#include "protein_database.h"
#include "protein.h"

using namespace clusterer;

ProteinID ProteinDatabase::_last_protein_id = 0;

extern "C"
{
    #include "xdrfile_xtc.h"
}

ProteinDatabase::ProteinDatabase(char const *file_name) throw (const char*) :
    _finished_reading(false),
    _proteins(),
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

size_t ProteinDatabase::get_atom_number() const
{
    return _atoms_in_a_protein;
}

std::vector<float>& ProteinDatabase::get_box()
{
    return _box;
}

float ProteinDatabase::get_precision() const
{
    return _precision;
}

IteratorRange ProteinDatabase::get_iterator_pair(size_t begin, size_t end)
{
    return IteratorRange(_proteins.begin() + begin, _proteins.begin() + end);
}

Protein& ProteinDatabase::operator[](ProteinID id)
{
    assert(id >= 0 && id < _proteins.size());
    return _proteins[id];
}

bool ProteinDatabase::finished_reading() const
{
    return _finished_reading;
}

size_t ProteinDatabase::size() const
{
    return _proteins.size();
}

std::pair<size_t, size_t> ProteinDatabase::generate_elements(size_t from, size_t size)
{
    size_t end;

    if (_finished_reading)
    {
        if (from + size > _proteins.size() - 1)
            end =  _proteins.size() - 1;
        else
            end = from + size;
    }
    else
    {
        std::auto_ptr<rvec> _atoms_vector(new rvec[ _atoms_in_a_protein ] );

        int    result;
        int    step;
        float  time;
        matrix box;
        float  prec;

        size_t protein_number;

        for (protein_number = 0; protein_number < size && !_finished_reading; ++protein_number)
        {
            // read one protein
            result = read_xtc(_xd,_atoms_in_a_protein,&step,&time,box,_atoms_vector.get(),&prec);

            if (step == 1) //first iteration
            {
                _precision = prec;

                //DIM is defined in xdrfile.h (should be 3)
                for (size_t i(0); i < DIM; ++i)
                    for (size_t j(0); j < DIM; ++j)
                        _box.push_back(box[i][j]);
            }

            if (exdrENDOFFILE != result)
            {
                if (exdrOK != result)
                    syslog(LOG_NOTICE,"ERROR: read_xtc %d",result);
                else
                {
                    _proteins.push_back(Protein(_last_protein_id, _atoms_in_a_protein));
                    assert(_proteins.size() == _last_protein_id + 1);

                    for (int atom_number(0); atom_number < _atoms_in_a_protein; ++atom_number)
                        _proteins[_last_protein_id][atom_number] = Coord3d(_atoms_vector.get()[atom_number][0],
                                                                           _atoms_vector.get()[atom_number][1],
                                                                           _atoms_vector.get()[atom_number][2]);
                    ++_last_protein_id;
                }
            }
            else
                _finished_reading = true;
        }
        if (_finished_reading)
            end = from + (protein_number - 1);
        else
            end = from + protein_number;
    }

    return std::pair<size_t,size_t>(from,end);
}
