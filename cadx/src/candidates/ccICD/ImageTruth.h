#ifndef __IMAGETRUTH_H_
#define __IMAGETRUTH_H_

#include "TrueNodule.h"
#include <string>
#include <fstream>

// ImageTruth : encapsulates nodule ground truth information for a single 
//              x-ray image (i.e. contecnts of a .hit file)
//
// crandall, 4/2003

class ImageTruth
{
    public:
          // constructor
        ImageTruth() {}

          // load a .hit file into current object
        void LoadHitFile(std::string _filename);

          // test whether a given point in the x-ray image falls inside
          // a ground truth nodule
        const TrueNodule *hit_test(Point point) const; 

          // count number of nodules in ground truth having a given status
          // (possible, probable, etc.)
        int count_by_status(Status stat) const;

          // overlay nodule boundaries onto an image
        void OverlayTruth(IemTImage<short> &img, int subsample_factor=1) const;

    protected:
          // hit filename and kespr image filename, respectively
		std::string filename, img_filename;

          // set of nodules in this image
		std::vector<TrueNodule> nodules;
};

#endif
