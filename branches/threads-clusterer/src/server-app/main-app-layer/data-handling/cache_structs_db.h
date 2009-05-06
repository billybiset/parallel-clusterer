#ifndef CACHE_STRUCTS_DB
#define	CACHE_STRUCTS_DB

#include <map>

#include "structuresDB.h"
#include "structure.h"

namespace parallel_clusterer
{
    class EditableCoordsStructure;

    class CacheStructsDB : public StructuresDB
    {
    public:
        struct Cache
        {
            std::map<size_t, EditableCoordsStructure> structures;
            bool enabled;
            
            inline Cache();

            inline bool is_hot(size_t index, EditableCoordsStructure& struc);
        };
        
        class CacheStructsHandler : public StructuresDB_handler
        {
        public:
            // Interface
            virtual void move_first();
            virtual bool read_next(EditableCoordsStructure& struc) ;
            virtual void read_nth(size_t index, EditableCoordsStructure& struc);	/* index=0 is first */
            virtual void write_next(const Structure& struc);
            virtual void write_nth(size_t index, const Structure& struc);	/* index=0 is first. DOESN'T CONSIDER HOTS */

            virtual void set_hot(size_t index);
            
            inline CacheStructsHandler(StructuresDB_handler* underlying, Cache* cache);
            
            inline virtual ~CacheStructsHandler();
            
        private:

            StructuresDB_handler* const _underlyingHandler;
            Cache* const				_cache;
            size_t						_currentIdx;
            EditableCoordsStructure 	_currentStruct;
            
        };
        
        virtual void open(const std::string& filename, AccessMode mode);
        virtual void close();
        void enable_caching();
        void disable_caching(); // removes the cache as well.
        
        virtual StructuresDB* clone() const;

        inline virtual CacheStructsHandler* get_handler() const;
        
        inline CacheStructsDB(StructuresDB* underlying);
        
        inline ~CacheStructsDB();

        inline virtual size_t natoms();

    private:
        StructuresDB* const _underlyingDB;
        Cache* const		_cache;

    };






    //// ================================ INLINE DEFINITIONS =================================

    inline CacheStructsDB::Cache::Cache()
        : enabled(true) 
    {}

    inline CacheStructsDB::CacheStructsHandler::CacheStructsHandler(StructuresDB_handler* underlying, Cache* cache)
        : _underlyingHandler(underlying), _cache(cache)
    {
        move_first();
    }

    inline CacheStructsDB::CacheStructsHandler::~CacheStructsHandler()
    {
        delete _underlyingHandler;
    }

    inline CacheStructsDB::CacheStructsHandler* CacheStructsDB::get_handler() const
    {
        return new CacheStructsHandler(_underlyingDB->get_handler(), _cache);
    }

    inline CacheStructsDB::CacheStructsDB(StructuresDB* underlying)
        : _underlyingDB(underlying), _cache(new Cache)
    {}

    inline CacheStructsDB::~CacheStructsDB()
    {
        delete _cache;
    }

    inline size_t CacheStructsDB::natoms()
    {
        return _underlyingDB->natoms();
    }
}
#endif
