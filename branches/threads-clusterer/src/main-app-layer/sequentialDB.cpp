#include <stdexcept>
#include "sequentialDB.h"

void SequentialDB_handler::read_nth(size_t target, EditableCoordsStructure& struc)	/* index=0 is first */
{
	size_t i=0;
	bool readOK;
	size_t delta;
	
	if (_next_to_read <= target)
	{
		delta = target - _next_to_read + 1;
	}
	else
	{
		move_first();
		delta = target + 1;
	}
	//_last = target; NOT NECESSARY: read_next will invoke our's read_next delta times
	
	do
	{
		readOK = read_next(struc);
		i++;
	}
	while(i < delta && readOK);
	
	if (!readOK)
		throw std::runtime_error("structure not found");
}

