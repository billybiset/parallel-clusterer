#include "markers.h"
#include "marker_iterators.h"
#include "structuresDB.h"

using namespace parallel_clusterer;

MarkerIterator::MarkerIterator(Marker& marker, StructuresDB& db) :
	_database(db.get_handler()),
	_marker(marker),
	_end(false),
	_size(marker.size()),
	_index(0),
	_cacheIdx(INVALID_INDEX)
{
		if (_database == 0)
			throw "Ups!";
}

MarkerIterator::~MarkerIterator()
{
	delete _database;
}

void MarkerIterator::SetFirst()
{
	_index = 0;
	if ( !IsMember(this->color()) )
	{
		++*this;
	}
	_first = true;
}

size_t MarkerIterator::index() const
{
	return _index;
}

Marker::Color MarkerIterator::color() const
{
	return _marker._marks[_index];
}

bool MarkerIterator::first() const
{
	return _first;
}

bool MarkerIterator::end() const
{
	return _end;
}

void MarkerIterator::operator ++()
{
	const size_t max_index = _size - 1;

	bool next_found = false;
	while ( !next_found && (_index < max_index) )
	{
		++_index;
		next_found = IsMember(_marker._marks[_index]);
	}

	_end = !next_found;
	_first = false;
}

const Structure& MarkerIterator::operator *() const
{
	if ( _index != _cacheIdx )
	{
		_database->read_nth(_index, _cache);
		_cacheIdx = _index;
	}
	return _cache;
}

void MarkerIterator::setHot()
{
	_database->set_hot(_index);
}


//////////////////////////////////////////

ColoredMarkerIterator::ColoredMarkerIterator(Marker& marker, StructuresDB& db, Marker::Color color) :
	MarkerIterator(marker, db),
	_color(color)
{
	SetFirst();
}

bool ColoredMarkerIterator::IsMember(Marker::Color color) const
{
	return color == _color;
}

////////////////////////////////////////////

UnmarkedIterator::UnmarkedIterator(Marker& marker, StructuresDB& db) :
	MarkerIterator(marker, db)
{
	SetFirst();	
}

bool UnmarkedIterator::IsMember(Marker::Color color) const
{
	return color == Marker::Untouched;
}

////////////////////////////////////////////

MarkedIterator::MarkedIterator(Marker& marker, StructuresDB& db) :
	MarkerIterator(marker, db)
{
	SetFirst();
}

bool MarkedIterator::IsMember(Marker::Color color) const
{
	return color != Marker::Untouched;
}

////////////////////////////////////////////

MarkedUntouchedIterator::MarkedUntouchedIterator(Marker& marker, StructuresDB& db) :
	MarkerIterator(marker, db)
{
	SetFirst();
}

bool MarkedUntouchedIterator::IsMember(Marker::Color color) const
{
	return (color != Marker::Untouched) && (color != Marker::Touched);
}
