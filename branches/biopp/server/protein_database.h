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


#include "protein.h"
#include "cluster.h"
#include "prot-filer/cached_reader.h"

namespace clusterer 
{

using prot_filer::Coord3DReaderFactory;
using prot_filer::Coord3DSeqReaderFactory;
using prot_filer::Coord3DReader;
using prot_filer::Coord3DSeqReader;
using prot_filer::SlidingWindowCache;

class CachedReader
{
    public:
    virtual const Coord3DReader& get_reader() = 0;
    virtual prot_filer::Protein* read(ProteinID id) = 0;
    virtual void close() = 0;
};

template<template<class> class Cache, class Reader>
class CachedReaderImple : public CachedReader
{
    public:
    typedef prot_filer::CachedReader<Cache, Reader, prot_filer::Protein> FilerCachedReader;

    CachedReaderImple(Reader* _reader) :
        reader(FilerCachedReader(_reader))
    {};
    const Coord3DReader& get_reader()
    {
        return reader.get_reader();
    }
    prot_filer::Protein* read(ProteinID id) 
    {
        return reader.read(id);
    }
    void close()
    {
        reader.close();
    }
    FilerCachedReader reader;
};

class ProteinDatabase 
{
    public:
        ProteinDatabase(const string& input_format, const string& input_file, string& cache_mode, size_t max_elems) :
            cached_reader(createCachedReader(input_format, input_file, cache_mode, max_elems)),
            _size(-1)
        {};
        ~ProteinDatabase()
        {
            delete cached_reader;
        }
        size_t get_atom_number()
        {
            return cached_reader->get_reader().get_atom_number();
        }
        Protein* read(const ProteinID& id)
        {
            prot_filer::Protein* p = cached_reader->read(id);
            if (p == NULL)
            {
                _size = id;
                return NULL;
            }
            else
            {
                biopp::StructureWithRotation r(*p);
                return new Protein(r, id);
            }
        }
        Protein& operator[](const ProteinID& id)
        {
            Protein* p = read(id);
            if (p == NULL)
                throw("Invalid protein index");
            return *p;
        }
        size_t size() const 
        {
            if (_size == -1)
            {
                throw("cannot get size until last element reached");
            }
            else
            {
                return _size;
            }
        }
        void close()
        {
            cached_reader->close();
        }
    private:
        CachedReader* cached_reader;
        int _size;
        static CachedReader* createCachedReader(const string& input_format, const string& input_file, 
                string& cache_mode, int max_elements)
        {
            if (input_format == "xtc" && cache_mode != "full_cache")
            {
                cerr << "Cache Policy changed to full_cache. No other policies are available for xtc format.";
                cache_mode = "full_cache";
            }
            if (cache_mode == "full_cache")
            {
                Coord3DReader* reader = Coord3DReaderFactory::get_instance()->create(input_format);
                reader->open( input_file );
                return new CachedReaderImple<prot_filer::FullCache, Coord3DReader>(reader);
            }
            else if (cache_mode == "protein_sliding_window")
            {
                Coord3DSeqReader* reader = Coord3DSeqReaderFactory::get_instance()->create(input_format);
                reader->open( input_file );
                return new CachedReaderImple<SlidingWindowCache<1>::CacheType, Coord3DSeqReader>(reader);
            }
            else
            {
                throw("invalid cache mode");
            }
        }
};

}
#endif
