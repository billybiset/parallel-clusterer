/*
  Parallel Clusterer: A distributed protein backbone clusterer.
    Copyright (C) 2009 Guillermo Biset, FuDePAN
    
    This file is part of the Parallel Clusterer Project.

    File:           marked_structuresDB.cpp
    Contents:       Implementaton file for a Database for marked structures. 
                    Will hold a set of marked structures, and another of 
                    unmarked ones. Used by the Representatives Job.

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

#include "marked_structuresDB.h"
#include "structuresDB.h"
#include "structure.h"

#include "gmx_structs_db.h"
#include "cache_structs_db.h"

using namespace parallel_clusterer;

MarkedStructuresDB::MarkedStructuresDB() :
    _gmxDB(new GmxStructsDB),
    _cache_structs_db(_gmxDB),
    _unmarked_list(),
    _marked_list()
{
    _gmxDB->open(string("data/1-2.xtc"), kReadOnly);

    EditableCoordsStructure s;
    StructuresDB_handler* hnd = _gmxDB->get_handler();
    hnd->read_nth(0, s);

    Structure * ptr = new Structure(s);
    _marked_list.push_front(ptr);

    unsigned int i;
    for (i = 1; i < _cache_structs_db.size(); i++)
    {
        hnd->read_nth(i, s);

        ptr = new Structure(s);
        _unmarked_list.push_front(ptr);
    }

//         _cached_data->natoms = s.get_natoms();
    delete hnd;

//     std::cout << "created lists. Marked size is " << _marked_list.size() << ". And unmarked size is " << _unmarked_list.size() << std::endl;

/*
assert:
    _unmarked_list(_cache_structs_db.size() - 1),
    _marked_list(1)
*/


//     structure_ptr = new Structure;

//     _gmxDB.get_handler()->read_nth(0,*structure_ptr);


/*

const Structure& MarkerIterator::operator *() const
{
    if ( _index != _cacheIdx )
    {
        _database->read_nth(_index, _cache);
        _cacheIdx = _index;
    }
    return _cache;
}

*/

//     std::cout << _cache_structs_db.size() << std::endl;
}