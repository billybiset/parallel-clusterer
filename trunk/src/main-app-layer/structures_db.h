#ifndef STRUCTURES_DB_H
#define STRUCTURES_DB_H

#include "gmx_structs_db.h"

namespace parallel_clusterer
{
    class Structures_DB
    {
        public:
            Structures_DB();

        private:
            StructuresDB* _gmxDB;
    };
}

#endif