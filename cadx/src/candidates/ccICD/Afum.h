#ifndef __AFUM_H_
#define __AFUM_H_

#include <set>
#include "Iem.h"
#include "Rect.h"

// AfumFilter : class used internally by Afum class
class AfumFilter
{
    public:
        AfumFilter(int _row, int _col, int _radius) :
                row(_row), col(_col), radius(_radius)
        {
        }
        
        int row, col, radius;

          // for sorting, by radius
        bool operator<(const AfumFilter &other) const
        {
            return(radius < other.radius || radius == other.radius && (row < other.row 
                   || row == other.row && col < other.col));
        }
};

// Afum : Perform Average Fraction Under the Minimum (AFUM) filtering
//
// This class implements M. Heath's AFUM filter. Please see Afum.cpp for 
// further information on Afum.
//
class Afum
{
    public:
          // constructs an Afum filter using radius _radius
        Afum(int _radius)
        {
            min_in_disk = 0;
            num_below_min = 0;
            num_at_radius = 0;

            change_radius(_radius);
        }

          // changes the Afum radius to _radius
        void change_radius(int _radius)
        {
            radius = _radius;
            setup_filter(_radius);
        }

          // apply the afum filter to an entire image, and return the resulting image
        IemPlane apply_filter(const IemTPlane<short> & image, int delta_r, const IemTPlane<byte> &mask);

          // apply the afum filter to a set of points within an image
        std::vector<double> apply_filter(const IemTPlane<short> & image, int delta_r, const IemTPlane<byte> &mask,
                                    const std::vector<Point> &points, double factor = 1);

        ~Afum()
        {
            cleanup();
        }

    protected:
          // process one pixel of an image through AFUM
        void process_pixel(const IemTPlane<short> & image, const IemTPlane<byte> &mask, int r, int c,
                               const short *img_cp, const byte *mask_cp, byte *afum_cp, int delta_r);

          // setup filter data structures
        void setup_filter(int radius);
        
          // do cleanup
        void cleanup();

        int radius;
        unsigned short int *min_in_disk;
        int *num_below_min, *num_at_radius;
		std::vector<AfumFilter> afum_filter;
};


#endif
