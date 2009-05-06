#include <stdexcept>

#include "structuresDB.h"
#include "structure.h"

using std::runtime_error;
using namespace parallel_clusterer;

size_t StructuresDB::size() const
{
	if (_cached_data->size == 0)
	{
		bool readOK;
		EditableCoordsStructure struc;
		StructuresDB_handler* const h = get_handler();
		
		if (h != NULL)
		{
			h->move_first();
			
			do
			{
				readOK = h->read_next(struc);
				_cached_data->size++;
			}
			while(readOK);
			
			delete h;
			
			_cached_data->size--;
		}
	}
		
	return _cached_data->size;
}

size_t StructuresDB::natoms() const
{
	if (_cached_data->natoms == 0)
	{
		EditableCoordsStructure s;
		StructuresDB_handler* hnd = get_handler();
		hnd->read_nth(0, s);
		_cached_data->natoms = s.get_natoms();
		delete hnd;
	}
	
	return _cached_data->natoms;
}

