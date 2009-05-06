#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "markers.h"
#include "marker_iterators.h"


using namespace std;
using namespace parallel_clusterer;

const Marker::Color Marker::Touched;
const Marker::Color Marker::Untouched;

Marker::Marker(size_t size) :
	_marks(size, Untouched)
{
}

size_t Marker::size() const
{
	return _marks.size();
}

size_t Marker::marked_count(Color color) const
{
	return std::count(_marks.begin(), _marks.end(), color);
}

size_t Marker::unmarked_count() const
{
	return marked_count(Untouched);
}

/* Deprecated
size_t Marker::get_index_unmarked_ord(size_t ordinal) const
{
	size_t	index		    = 0;
	size_t	unmarkedCount	= 0;
	//bool	strFound	    = false;

	size_t unmarked = unmarked_count();
	if ( unmarked > 0 )
	{
		ordinal %= unmarked;
		while ( unmarkedCount < ordinal )
		{
			if ( !_marks[index] )
			{
				unmarkedCount++;
			}
			index++;
		}
		
	}
	else
	{
		throw std::out_of_range("Marker::get_index_unmarked_ord");
	}

	return index;
}
*/

Marker::Color Marker::max_color() const
{
	// need this to be more efficient?
	return *(std::max_element(_marks.begin(), _marks.end()));
}

bool Marker::isThereAny(Color color) const
{
	bool any = false;

	size_t index = 0;
	const size_t size = _marks.size();
	while ( (index < size) && !any )
	{
		any = _marks[index] == color;
	}

	return any;
}

void Marker::mark(size_t ordinal, Color color)
{
	_marks[ordinal] = color;
}

void Marker::mark(const MarkerIterator& it, Color color)
{
	_marks[it.index()] = color;
}

void Marker::reset(Color color)
{
	std::replace(_marks.begin(), _marks.end(), color, Untouched);
}

Marker& Marker::operator +=(const Marker& other)
{
	const size_t size = _marks.size();
	if (size != other._marks.size() )
		throw "Ups!!!!";
		
	for (size_t i = 0; i < size; ++i)
	{
		if ( _marks[i] == Untouched )
		{
			_marks[i] = other._marks[i];
		}
	}

	return *this;
}

