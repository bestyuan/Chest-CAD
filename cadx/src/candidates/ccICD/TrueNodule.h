#ifndef __TRUENODULE_H_
#define __TRUENODULE_H_

#include <set>
#include <Iem.h>
#include "Rect.h"

// a contour is just a set of points...
typedef std::set<Point> Contour;

// possible Status values
typedef enum {falsealarm, possible, probable, definite, unknown=-1} Status;


// TrueNodule : encapsulates information about a single ground truth nodule
//
// crandall, 4/2003
//
class TrueNodule
{
    public:
          // constructor, constructs an empty object
        TrueNodule() {}

          // constructor, fills in contour, status, centroid information
        TrueNodule(const Contour &_contour, Status _status, Point _centroid) :
                contour(_contour), status(_status), centroid(_centroid) {}

          // tests whether the given point is inside our nodule boundary
        bool hit_test(Point point) const;

          // returns the status of our nodule (falsepositive, unknown, etc.)
        Status get_status() const
        {
            return status;
        }

          // overlays nodule boundary on img
        void OverlayTruth(IemTImage<short> &img, int subsample_factor=1) const;

          // input operator
		friend std::istream &operator>>(std::istream &is, TrueNodule &tn);

          // equality operator
        friend bool operator==(const TrueNodule &a, const TrueNodule &b);

    protected:
          // index (from .hit file) of nodule
        int index;

          // contour of nodule boundary
        Contour contour;

          // status of nodule
        Status status;

          // centroid point
        Point centroid;

          // minimum-enclosing rectangle (MER) of nodule boundary
        Rect bounding_box;
};

// output status name (string) given Status
std::ostream &operator<<(std::ostream &os, Status status);

// input status name (string) to Status value
std::istream &operator>>(std::istream &is, Status &status);

#endif
