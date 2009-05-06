#ifndef GMX_STRUCTURES_DB
#define	GMX_STRUCTURES_DB

#include <string>
using std::string;

#include "sequentialDB.h"

/* Begin GMX definitions */
#define	DIM		3
typedef float           real;
typedef real            rvec[DIM];
/* End GMX definitions */

namespace parallel_clusterer
{
    class GmxStructsDB : public StructuresDB
    {
        class GmxHandler : public SequentialDB_handler
        {
            // Interface
            virtual void move_first();
            virtual bool read_next(EditableCoordsStructure& struc) ;
            virtual void write_next(const Structure& struc);
            virtual void write_nth(size_t index, const Structure& struc);

            void fill_structure(EditableCoordsStructure& s) const;
            int writextc(rvec* atoms, size_t natoms);

            int					handle;
            bool				first_read;
            size_t				atoms_count;
            rvec*				vec;
            real				last_prec;
            int					last_step;
            const AccessMode	access_mode;
            size_t* const		_natoms_to_fill;
            
        public:
            GmxHandler(const string& filename, AccessMode mode, size_t* natoms_to_fill);
            virtual ~GmxHandler();
        };
        
        // Interface
        virtual void open(const std::string& filename, AccessMode mode)
        {
            if (opened) close();
            file_name = filename;
            access_mode = mode;
        }
        
        virtual void close() {}
            
        virtual StructuresDB* clone() const
        {
            return new GmxStructsDB(file_name, access_mode, *_cached_data);
        }
        
        virtual StructuresDB_handler* get_handler() const
        {
            size_t* natoms_to_fill = NULL;
            if (_cached_data->natoms == 0)
                natoms_to_fill = &_cached_data->natoms;
            return new GmxHandler(file_name, access_mode, natoms_to_fill);
        }

        ~GmxStructsDB() { close(); }
        //GmxStructsDB(real prec) : handle(0), first_read(true), last_prec(prec), last_step(0), opened(false) {}
        GmxStructsDB(const string& filename, AccessMode mode, const CachedData& cdata) : StructuresDB(cdata), file_name(filename), access_mode(mode) {}

        string		file_name;	
        AccessMode	access_mode;
        bool		opened;
        
    public:
        //GmxStructsDB() : handle(0), first_read(true), last_prec(1.0), last_step(0), opened(false) {}
        GmxStructsDB() : opened(false) {}
    };
}
#endif
