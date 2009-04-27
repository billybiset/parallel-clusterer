#include <iostream>

#include "structures_db.h"

#include "structuresDB.h"

#include "gmx_structs_db.h"

#include "cache_structs_db.h"

using namespace parallel_clusterer;

Structures_DB::Structures_DB() //:
//     _gmxDB(new GmxStructsDB) //,
//     _cache_structs_db(_gmxDB)
{
    StructuresDB* _gmxDB = new GmxStructsDB;
    _gmxDB->open(string("data/1-2.xtc"), kReadOnly);

//     _cache_structs_db = CacheStructsDB(_gmxDB);
    CacheStructsDB _cache_structs_db(_gmxDB);

    std::cout << _cache_structs_db.size() << std::endl;

//     std::cout << _gmxDB << std::endl;
}