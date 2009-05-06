/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           marked_structuresDB.h
    Contents:       Header file for a Database for marked structures. Will hold 
                    a set of marked structures, and another of unmarked ones.
                    Used by the Representatives Job.

    System:         Parallel Clusterer
    Language:       C++
  
    Author:         Guillermo Biset
    E-Mail:         billybiset AT gmail.com
  
    Parallel Clusterer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Parallel Clusterer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Parallel Clusterer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARKED_STRUCTURES_DB_H
#define MARKED_STRUCTURES_DB_H

#include <list>

#include "gmx_structs_db.h"
#include "cache_structs_db.h"

namespace parallel_clusterer
{
    class MarkedStructuresDB
    {
        public:
            MarkedStructuresDB();

            inline size_t get_unmarked_size() const { return _unmarked_list.size(); }
            inline size_t get_marked_size()   const { return _marked_list.size(); }

            inline std::list<Structure *> get_unmarked_list() const { return _unmarked_list; }
            inline std::list<Structure *> get_marked_list()   const { return _marked_list; }
        private:
            StructuresDB*  _gmxDB;
            CacheStructsDB _cache_structs_db;

            std::list<Structure *> _unmarked_list;
            std::list<Structure *> _marked_list;
    };
}

#endif