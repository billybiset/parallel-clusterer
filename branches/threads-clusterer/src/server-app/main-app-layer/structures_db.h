#ifndef STRUCTURES_DB_H
#define STRUCTURES_DB_H

#include <list>

#include "gmx_structs_db.h"

#include "cache_structs_db.h"

namespace parallel_clusterer
{
    class Structures_DB
    {
        public:
            Structures_DB();

//             std::list<Structure> get_unmarked

            inline size_t get_unmarked_size() const { return _unmarked_list.size(); }
            inline size_t get_marked_size() const { return _marked_list.size(); }

            inline std::list<Structure *> get_unmarked_list() const { return _unmarked_list; }
            inline std::list<Structure *> get_marked_list() const { return _marked_list; }
        private:
            StructuresDB*  _gmxDB;
            CacheStructsDB _cache_structs_db;

            std::list<Structure *> _unmarked_list;
            std::list<Structure *> _marked_list;
    };
}

#endif