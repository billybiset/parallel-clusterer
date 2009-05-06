#include <iostream>
#include <stdexcept>
#include "cache_structs_db.h"
#include "structure.h"

using namespace std;
using namespace parallel_clusterer;

inline bool CacheStructsDB::Cache::is_hot(size_t index, EditableCoordsStructure& struc)
{
	if (enabled)
	{
		std::map<size_t, EditableCoordsStructure>::iterator it = structures.find(index);
		
		const bool ret = (it != structures.end() );
		
		if (ret)
		{
		    struc = static_cast<Structure&>(it->second);
		}

		return ret;
	}
	else
		return false;
}

void CacheStructsDB::CacheStructsHandler::move_first()
{
	_currentIdx = 0;
	_underlyingHandler->read_nth(0, _currentStruct);
	_underlyingHandler->move_first();
}

bool CacheStructsDB::CacheStructsHandler::read_next(EditableCoordsStructure& struc)
{
	const bool ret = _underlyingHandler->read_next(struc);

	_currentIdx++;
	
	if (ret)
		_currentStruct = struc;

	return ret;
}


void CacheStructsDB::CacheStructsHandler::read_nth(size_t index, EditableCoordsStructure& struc)
{
	if (_currentIdx == index)
	{
		struc = _currentStruct;
	}
	else
	{
		_currentIdx = index;
		
		if (!_cache->is_hot(index, struc))
		{
			_underlyingHandler->read_nth(index, struc);
			_currentStruct = struc;
		}
	}
}

void CacheStructsDB::CacheStructsHandler::write_next(const Structure& struc)
{
	_underlyingHandler->write_next(struc);
}

void CacheStructsDB::CacheStructsHandler::write_nth(size_t index, const Structure& struc)
{
	_underlyingHandler->write_nth(index, struc);
}

void CacheStructsDB::CacheStructsHandler::set_hot(size_t index)
{
	if (_cache->enabled)
	{
		if ( index != _currentIdx )
		{
			_currentIdx = index;
			_underlyingHandler->read_nth(index, _currentStruct);
		}
    		_cache->structures.insert(std::pair<size_t, EditableCoordsStructure>(_currentIdx, _currentStruct));
	}
}


void CacheStructsDB::open(const std::string& filename, AccessMode mode)
{
	throw std::runtime_error("CacheStructsDB::open call is invalid");
}

void CacheStructsDB::close()
{
	throw std::runtime_error("CacheStructsDB::close call is invalid");
}

void CacheStructsDB::enable_caching()
{
	_cache->enabled = true;
}

void CacheStructsDB::disable_caching()
{
    _cache->structures.clear();
    _cache->enabled = false;
}

StructuresDB* CacheStructsDB::clone() const
{
    return new CacheStructsDB(_underlyingDB);
}

