#ifndef __RECT_H_
#define __RECT_H_

#include <Iem.h>
#include <set>
#ifdef WIN32
#include <iostream>
#else
#include <iostream>
#endif

// Rect : a simple Rectangle class
//
// crandall, 10/2001
//

// a Point is a pair of ints (coordinates)
// Our convention is that the first int is the row, and the second is the column.
typedef std::pair<int, int> Point;

class Rect
{
    public:
          // create empty rectangle
        Rect() {} 

          // create rectangle using top and left coordinates, and height and width
        Rect(int _t, int _l, int _h, int _w)
        {
            top = _t, left = _l;
            height = _h, width = _w;
        }

          // test whether point is within (or on boundary of) our rectangle.
          // return true if so, otherwise false.
        bool hit_test(Point point) const
        {
            return(point.first >= top && point.first <= top + height + 1 &&
                   point.second >= left && point.second <= left + width + 1);
        }

        int top, left, height, width;

          // function to output rectangle coordinates to a stream
        friend std::ostream &operator<<(std::ostream &os, const class Rect &rect);
};


#endif
