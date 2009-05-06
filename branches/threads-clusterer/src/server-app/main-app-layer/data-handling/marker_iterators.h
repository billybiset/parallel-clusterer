#ifndef MARKER_ITERATOR_H
#define MARKER_ITERATOR_H

//#include "defs.h"
#include "structure.h"

//class Marker;
#include "markers.h"

namespace parallel_clusterer
{

    class StructuresDB;
    class StructuresDB_handler;

    class MarkerIterator
    {
        static const size_t INVALID_INDEX = (size_t)-1;

    protected:
        StructuresDB_handler* const _database;
        Marker&                     _marker;
        bool                        _end;
        const size_t                _size;
        size_t                      _index;
        bool                        _first;

        mutable size_t                  _cacheIdx;
        mutable EditableCoordsStructure _cache;

        virtual bool IsMember(Marker::Color color) const = 0;
        void SetFirst();

    public:
        MarkerIterator(Marker& marker, StructuresDB& db);
        virtual ~MarkerIterator();

        size_t index() const;
        Marker::Color color() const;
        bool first() const;
        bool end() const;       /* cannot apply * operator */
        void operator ++();     /* increment operator */

        const Structure& operator* () const;
        void setHot();
    };

    class ColoredMarkerIterator : public MarkerIterator
    {
        const Marker::Color     _color;

        virtual bool IsMember(Marker::Color color) const;

    public:
        ColoredMarkerIterator(Marker& marker, StructuresDB& db, Marker::Color color);
    };

    class UnmarkedIterator : public MarkerIterator
    {
        virtual bool IsMember(Marker::Color color) const;

    public:
        UnmarkedIterator(Marker& marker, StructuresDB& db);
    };

    class MarkedUntouchedIterator : public MarkerIterator
    {
        virtual bool IsMember(Marker::Color color) const;

    public:
    //#include <iostream>
        MarkedUntouchedIterator(Marker& marker, StructuresDB& db);
    };

    class MarkedIterator : public MarkerIterator
    {
        virtual bool IsMember(Marker::Color color) const;

    public:
        MarkedIterator(Marker& marker, StructuresDB& db);
    };
}
#endif
