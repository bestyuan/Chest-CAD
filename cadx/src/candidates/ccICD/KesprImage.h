#include "Iem.h"

// KesprImage : encapsulates KESPR images, so that they are compatible
//              with IEM image objects
//
// The base class of KesprImage is an Iem short image plane; hence
// objects of type KesprImage can be used wherever Iem short image
// planes could. The full IEM library is therefore available for
// processing objects of type KesprImage.
//
// crandall, 4/2003

#ifndef __KESPRIMAGE_H_
#define __KESPRIMAGE_H_

class KesprImage : public IemTPlane<short>
{
    public:
          // read a KESPR image from a file into this object
        void kesprRead(const char *filename); 

          // retrieve pixel spacing information from KESPR file
        double get_pixel_spacing() const { return pix_spacing; }

    protected:
        double pix_spacing;
};

#endif
