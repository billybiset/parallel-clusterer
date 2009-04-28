#include <iostream>

#include "structures_db.h"

#include "structuresDB.h"

#include "gmx_structs_db.h"

#include "cache_structs_db.h"

#include "structure.h"

using namespace parallel_clusterer;

Structures_DB::Structures_DB() :
    _gmxDB(new GmxStructsDB),
    _cache_structs_db(_gmxDB),
    _unmarked_list(),
    _marked_list()

/*    _unmarked_list(_cache_structs_db.size() - 1),
    _marked_list(1)*/
{
    _gmxDB->open(string("data/1-2.xtc"), kReadOnly);

    EditableCoordsStructure s;
    StructuresDB_handler* hnd = _gmxDB->get_handler();
    hnd->read_nth(0, s);

    Structure * ptr = new Structure(s);
    _marked_list.push_front(ptr);

    int i;
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