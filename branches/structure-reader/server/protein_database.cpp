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
#include "structure_reader.h"
#include "protein.h"

using namespace clusterer;

ProteinID ProteinDatabase::_last_protein_id = 0;

ProteinDatabase::ProteinDatabase(StructureReader* reader) throw (const char*) :
    _proteins(),
    _reader(reader)
{
}

size_t ProteinDatabase::get_atom_number() const
{
    return _reader->get_atom_number();
}

float* ProteinDatabase::get_box()
{
    return _reader->get_box();
}

float ProteinDatabase::get_precision() const
{
    return _reader->get_precision();
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
    return _reader->finished_reading();
}

size_t ProteinDatabase::size() const
{
    return _proteins.size();
}

std::pair<size_t, size_t> ProteinDatabase::generate_elements(size_t from, size_t size)
{
    size_t end;

    if (_reader->finished_reading())
        end = std::min(from + size, _proteins.size() - 1);
    else
    {
        size_t protein_number;

        for (protein_number = 0; protein_number < size && !_reader->finished_reading(); ++protein_number)
        {
           // read one protein
            _proteins.push_back( Protein(_last_protein_id, _reader->get_atom_number() ));

            _reader->read_structure( _proteins[_last_protein_id] );

            ++_last_protein_id;
        }

        end = from + protein_number;

        if ( _reader->finished_reading() )
            --end;
    }

    return std::pair<size_t,size_t>(from,end);
}
