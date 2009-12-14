/**
 * \file  protein_database.h
 * \brief Definition of ProteinDatabase class.
 *
 * FDC: FuDePAN Distributed Clusterer
 * <http://fud.googlecode.com/>
 * Copyright (C) 2009 Guillermo Biset, FuDePAN
 *
 * This file is part of the FuD project.
 *
 * Contents:       Header file for FDC providing class ProteinDatabase.
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

#ifndef PROTEIN_DATABASE_H
#define PROTEIN_DATABASE_H

extern "C"
{
    #include "xdrfile.h"
}

#include "protein.h"
#include "cluster.h"

namespace clusterer
{
    typedef std::vector<Protein>::iterator ProteinIterator;
    typedef std::pair<ProteinIterator,ProteinIterator> IteratorRange;

    class ProteinDatabase
    {
        public:
            ProteinDatabase(const char* file_name) throw (const char*);

            size_t get_atom_number() const;

            std::vector<float>& get_box();

            float get_precision() const;

            bool   finished_reading() const;

            size_t size() const;

            std::pair<size_t, size_t> generate_elements(size_t from, size_t size);

            IteratorRange get_iterator_pair(size_t begin, size_t end);

            const Protein& operator[](ProteinID id);

            const std::vector<Protein>&   get_proteins()   const;
        private:
            bool                     _finished_reading;
            std::vector<Protein>     _proteins;
            XDRFILE*                 _xd;
            int                      _atoms_in_a_protein;
            std::vector<float>       _box;
            float                    _precision;

            static ProteinID         _last_protein_id;
    };
}

#endif