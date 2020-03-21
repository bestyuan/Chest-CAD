#ifndef _____BOUNDING_BOX_H_____
#define _____BOUNDING_BOX_H_____

#include <limits.h>
#include <iostream>

using namespace std;
// left is the x offset of the first (farthest left) pixel
// top is the y offset of the first (highest) pixel
// right is the offset of the farthest right pixel
// bottom is the offset of the lowest pixel
class BoundingBox {    /* bounding box structure */
public:
    inline BoundingBox(): left(INT_MAX), top(INT_MAX), right(INT_MIN), bottom(INT_MIN){}
    inline BoundingBox(int l, int t, int r, int b): left(l), top(t), right(r), bottom(b){}
    inline BoundingBox(const BoundingBox &a): left(a.left), top(a.top), right(a.right), bottom(a.bottom){}
    inline int height() {return 1 + bottom - top;}
    inline int width() {return 1 + right - left;}
    inline int area() {return (1 + right - left) * (1 + bottom - top);}
    inline float aspect()  {return float (1 + right - left) / (1 + bottom - top);}
    inline bool isValid() {return (left <= right) && (top <= bottom);}
    inline void include(int yy, int xx)
    {
      if (xx < left) left = xx;
      if (yy < top) top = yy;
      if (xx > right) right = xx;
      if (yy > bottom) bottom = yy;
    }
    inline void include(const BoundingBox &other)
    {
      if (other.left < left) left = other.left;
      if (other.top < top) top = other.top;
      if (other.right > right) right = other.right;
      if (other.bottom > bottom) bottom = other.bottom;
    }
    // make the boundingbox invalid
    inline void clear() 
    {
      left = INT_MAX; 
      top = INT_MAX;
      right = INT_MIN;
      bottom = INT_MIN;
    }
    inline bool operator== (const BoundingBox &a)
    { return (left == a.left) && (top == a.top) && (right == a.right) && (bottom == a.bottom);}
    inline bool operator!= (const BoundingBox &a)
    { return (left != a.left) || (top != a.top) || (right != a.right) || (bottom != a.bottom);}

    inline friend ostream & operator << (ostream &os, const BoundingBox &bbox)
    {  return os << "[" << bbox.left << "," << bbox.top << "," << bbox.right << "," << bbox.bottom << "]"; }
    inline friend ostream & operator << (ostream &os, const BoundingBox *bbox)
    {  return os << "[" << bbox->left << "," << bbox->top << "," << bbox->right << "," << bbox->bottom << "]"; }

public:
    int    left;
    int    top;
    int    right;
    int    bottom;
};

#endif /* _____BOUNDING_BOX_H_____ */

