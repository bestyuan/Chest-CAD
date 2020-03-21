#include "TrueNodule.h"
#include <string>

// TrueNodule : encapsulates information about a single ground truth nodule
//
// crandall, 4/2003
//

using namespace std;

// output status name (string) to a stream given a Status value
//
std::ostream &operator<<(std::ostream &os, Status status)
{
    switch(status)
    {
        case falsealarm:
            os << "negative";
            break;
        case possible:
            os << "possible";
            break;
        case probable:
            os << "probable";
            break;
        case definite:
            os << "definite";
            break;
        case unknown:
            os << "unknown";
            break;
        default:
            os << "error";
            break;
    }

    return os;
}

// input status name (string) from a stream and put in Status value
//
std::istream &operator>>(std::istream &is, Status &status)
{
    string word;

    is >> word;

    if(word == "possible")
        status = possible;
    else if(word == "probable")
        status = probable;
    else if(word == "definite")
        status = definite;
    else if(word == "unknown")
        status = unknown;
    else if(word == "negative")
        status = falsealarm;

    return is;
}

// input information about a nodule, from a .hit file, to current object
std::istream &operator>>(std::istream &is, TrueNodule &tn)
{
    string word;

    while(1)
    {
          // get a word
        is >> word;

          // process fields
        if(word == "noduleIndex:")
            is >> tn.index;
        else if(word == "status:")
            is >> tn.status;
        else if(word == "centroid:")
            is >> tn.centroid.first >> tn.centroid.second;
        else if(word == "nPoints:")
        {
            int point_count;
            is >> point_count;

              // read in contour
            for(int i=0; i<point_count; i++)
            {
                Point p;

                is >> p.second >> p.first;
                tn.contour.insert(p);
            }
            
            break;
        }
    }

      // compute bounding boxes around each nodule
      // (this is a bit inefficient and clumsy, but it works).
    tn.bounding_box = Rect(1000000, 1000000, -1, -1);
    Contour::iterator con_iter;
    for(con_iter = tn.contour.begin(); con_iter != tn.contour.end(); ++con_iter)
    {
        if(con_iter->first < tn.bounding_box.top)
            tn.bounding_box.top = con_iter->first;
        if(con_iter->first > tn.bounding_box.top + tn.bounding_box.height)
            tn.bounding_box.height = con_iter->first - tn.bounding_box.top;
        if(con_iter->second < tn.bounding_box.left)
            tn.bounding_box.left = con_iter->second;
        if(con_iter->second > tn.bounding_box.left + tn.bounding_box.width)
            tn.bounding_box.width = con_iter->second - tn.bounding_box.left;
    }

    return is;
}


// overlays nodule boundary on img
//
// This function paints the boundary of the nodule onto the specified img.
// Currently, nodules are painted according to the following color code:
//
//   possible : red
//   probable : green
//   definite : blue
//
// img : short IemTImage upon which nodules will be painted
// subsample_factor : if not 1, contour coordinates are first divided by
//                    this factor before painting on img
//                    (useful if img has been subsampled)
//
void TrueNodule::OverlayTruth(IemTImage<short> &img, int subsample_factor) const
{
    Contour::const_iterator con_iter;
    for(con_iter = contour.begin(); con_iter != contour.end(); ++con_iter)
    {
        int row = con_iter->first / subsample_factor;
        int col = con_iter->second / subsample_factor;

        img[status-1][row][col] = 2048;
    }

}

// determine whether given point lies within nodule boundary
//
// point : point of interest
//
// returns true if point lies within boundary, otherwise false
//
// Note: This test is not exactly accurate, but is good enough for our purposes.
// A point is automatically rejected if it lies outside the bounding box of
// the nodule. The point is also rejected if it is to the left of the
// left-most boundary pixel on its row, or if it is right of the right-most
// boundary pixel on its row. Otherwise it is accepted. Note that this means
// that if the boundary crosses the current row more than twice, and the pixel
// could lie outside the boundary but still be counted as being inside.
// This whole issue is rather moot because nodules are typically convex anyway.
//
bool TrueNodule::hit_test(Point point) const
{
    if(bounding_box.hit_test(point))
    {
        Contour same_line;

        Contour::const_iterator con_iter;
        for(con_iter = contour.begin(); con_iter != contour.end(); ++con_iter)        
            if(con_iter->first == point.first)
                same_line.insert(*con_iter);

        bool low_test = false, high_test = false, border_hit = false;
        for(con_iter = same_line.begin(); con_iter != same_line.end(); ++con_iter)        
        {
            if(point.second >= con_iter->second)
                low_test = true;
            else if(point.second <= con_iter->second)
                high_test = true;

            if(con_iter->second == point.second)
                border_hit = true;
        }
        
          // count as a hit if the point falls *anywhere* within the region 
          // *including* the region boundary
          // crandall 6/4/03: I'm not really sure whether this is a fair 
          // criterion or not (ideally we'd like to somehow measure how closely
          // the candidate position is to the centroid of the true nodule),
          // but I'm doing it this way to be able to fairly compare to 
          // tests run using J. Revelli's code.
        return(low_test && high_test || border_hit);
    }

    return false;
}


bool operator==(const TrueNodule &a, const TrueNodule &b)
{
    return(a.centroid.first == b.centroid.first && a.centroid.second == b.centroid.second);
}
