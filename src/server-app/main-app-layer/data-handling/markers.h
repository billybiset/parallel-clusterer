#ifndef MARKERS_H
#define MARKERS_H

#include <vector>

// debug DFG ggg
#include <iostream>
using namespace std;

namespace parallel_clusterer
{

    class MarkerIterator;

    class Marker
    {
    public:
        typedef size_t Color;
        static const Color Untouched = 0;
        static const Color Touched = 1;

    private:
        std::vector<Color>	_marks;

    public:
        Marker(size_t size);
        
        // DFG debug ggg
        void show() const
        {
            for (size_t i = 0; i < _marks.size(); i++)
                cout << i << ": " << _marks[i] << endl;
        }
        
        size_t size() const;
        size_t marked_count(Color color) const;
        size_t unmarked_count() const;
        Color max_color() const;
        Color min_color() const		{ return Touched+1;	}	// GGG: should be a cached variable from construction of marks
        bool isThereAny(Color color) const;

        void mark(size_t ordinal, Color color);
        void mark(const MarkerIterator& it, Color color);
            Color get_mark(size_t ordinal) const { return _marks[ordinal]; }
        void reset(Color color);
        
        Marker& operator +=(const Marker& other);	/* Does not include overlappings */

        friend class MarkerIterator;
    };
}
#endif
