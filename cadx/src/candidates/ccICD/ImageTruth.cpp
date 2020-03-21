#include "ImageTruth.h"

// ImageTruth : encapsulates nodule ground truth information for a single 
//              x-ray image (i.e. contecnts of a .hit file)
//
// crandall, 4/2003

using namespace std;

// count number of nodules in ground truth having a given status
// (possible, probable, etc.)
//
// stat : Status of interest
//
// returns number of nodules having Status stat
// 
int ImageTruth::count_by_status(Status stat) const
{
    int count=0;

    vector<TrueNodule>::const_iterator nod_iter;
    for(nod_iter = nodules.begin(); nod_iter != nodules.end(); nod_iter++)
        if(nod_iter->get_status() == stat)
            count++;

    return count;
}


// overlay nodule boundaries onto an image
//
void ImageTruth::OverlayTruth(IemTImage<short> &img, int subsample_factor) const
{
    vector<TrueNodule>::const_iterator nod_iter;
    for(nod_iter = nodules.begin(); nod_iter != nodules.end(); nod_iter++)
        nod_iter->OverlayTruth(img, subsample_factor);
}

// load ground truth information from a .hit file into current object
//
// filename : filename of .hit file
//
void ImageTruth::LoadHitFile(string _filename)
{
    filename = _filename;

	ifstream file;
    file.open(_filename.c_str());

    char temp[1024];

      // This is not a terribly robust reader, because we don't
      // even bother to make sure that the file is actually a 
      // .hit file (by verifying the contents of the first line).
      // Instead just look for the numNodules: field, pluck out
      // the nodule count, then read in the nodules one-by-one.
    while(file.getline(temp, 1024))
    {
          // ignore comments
        if(temp[0] == '#')
            continue;
        else if(!strncmp(temp, "numNodules:", 11)) 
        {
            int nodule_count = atoi(temp + 12);

            for(int i=0; i<nodule_count; i++)
            {
                TrueNodule nod;
                
                file >> nod;
                nodules.push_back(nod);
            }
            break;
        }
    }
}


// test whether a given point in the x-ray image falls inside a ground truth nodule
//
// point : point within x-ray image
// 
// returns: 0 if there's no hit; a pointer to the TrueNodule object if there is a hit
//
// note: there's no provision here for multiple hits (i.e. caused by overlapping gt nodules);
//       only one (the first found) will be returned.
//
const TrueNodule *ImageTruth::hit_test(Point point) const
{
    vector<TrueNodule>::const_iterator nod_iter;

    for(nod_iter = nodules.begin(); nod_iter != nodules.end(); ++nod_iter)
    {
        if(nod_iter->hit_test(point))
        {
            return &(*nod_iter);
        }
    }

      // not found
    return 0;
}
