#ifndef STRUCTURES_DB
#define	STRUCTURES_DB

#include <cstddef>
#include <string>

/* ALTA SOSPECHA: el parametro de las read deberia ser Structure y no Editable */

namespace parallel_clusterer
{

    class Structure;
    class EditableCoordsStructure;

    enum AccessMode
    {
        kReadOnly,
        kWriteOnly,
        kReadWrite
    };

    struct StructuresDB_handler;

    struct StructuresDB
    {
        virtual void open(const std::string& filename, AccessMode mode) = 0;
        virtual void close() = 0;
        
        virtual StructuresDB* clone() const = 0;
        virtual StructuresDB_handler* get_handler() const = 0;
        virtual size_t size() const; // try to avoid :D
        virtual size_t natoms() const;
        
        virtual ~StructuresDB()
        {
            delete _cached_data;
        }

    protected:
        struct CachedData
        {
            size_t	size;
            size_t	natoms;
            
            CachedData() : size(0), natoms(0) {}
            CachedData(const CachedData& other) : size(other.size), natoms(other.natoms) {}
        } * const _cached_data;
        
        StructuresDB() : _cached_data(new CachedData) {}
        StructuresDB(const CachedData& cdata) : _cached_data(new CachedData(cdata)) {}
    };

    struct StructuresDB_handler
    {
        virtual void move_first() = 0;
        virtual bool read_next(EditableCoordsStructure& struc) = 0;
        virtual void read_nth(size_t index, EditableCoordsStructure& struc) = 0;	/* index=0 is first */
        virtual void write_next(const Structure& struc) = 0;
        virtual void write_nth(size_t index, const Structure& struc) = 0;	/* index=0 is first */

        virtual void set_hot(size_t index) {}

        virtual ~StructuresDB_handler() {}
    };
}
#endif
