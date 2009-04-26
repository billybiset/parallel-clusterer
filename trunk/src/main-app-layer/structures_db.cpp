#include "structures_db.h"

#include "gmx_structs_db.h"

using namespace parallel_clusterer;

Structures_DB::Structures_DB() :
    _gmxDB(new GmxStructsDB)
{
    _gmxDB->open("data/1-2.xtc", kReadOnly);
}