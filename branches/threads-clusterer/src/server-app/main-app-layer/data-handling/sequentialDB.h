#ifndef SEQUENTIAL_DB_H

#include "structuresDB.h"

namespace parallel_clusterer
{

    class SequentialDB_handler : public StructuresDB_handler
    {
    protected:
        virtual void move_first()
        {
            _next_to_read = 0;
        }
        
        virtual bool read_next(EditableCoordsStructure& struc) // to be implemented!!!
        {
            _next_to_read++;
            return true;
        }
        
        virtual void read_nth(size_t target, EditableCoordsStructure& struc);	/* index=0 is first */
        
        virtual void write_next(const Structure& struc)
        {
            _next_to_read++;
        }

        
        // NOT YET IMPLEMENTED: virtual void write_nth(size_t index, const Structure& struc);	/* index=0 is first */

        virtual ~SequentialDB_handler() {}
        SequentialDB_handler() : _next_to_read(0){}
    private:
        size_t _next_to_read;
    };
}
#endif
