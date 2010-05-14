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

#include "protein_database.h"
#include "prot-filer/format_filer.h"
#include "protein.h"

using namespace clusterer;

ProteinID ProteinDatabase::_last_protein_id = 0;

ProteinDatabase::ProteinDatabase(FormatFiler* reader) throw (const char*) :
    _proteins(),
    _reader(reader)
{
}

size_t ProteinDatabase::get_atom_number() const
{
    return _reader->get_atom_number();
}

const float* ProteinDatabase::get_box()
{
    return _reader->get_box();
}

float ProteinDatabase::get_precision() const
{
    return _reader->get_precision();
}

Protein& ProteinDatabase::operator[](ProteinID id)
{
    assert(id >= 0 && ( ( ! finished_reading() ) || (id <=  _proteins.size()) ) );

    if ( id >= _proteins.size() )
    {
        assert(id == _proteins.size() );

        _proteins.push_back( Protein(_last_protein_id, _reader->get_atom_number() ));

        _reader->read( _proteins[_last_protein_id] );

        ++_last_protein_id;
    }

    return _proteins[id];
}

bool ProteinDatabase::finished_reading() const
{
    return _reader->finished_reading();
}

size_t ProteinDatabase::size() const
{
    return _proteins.size();
}
