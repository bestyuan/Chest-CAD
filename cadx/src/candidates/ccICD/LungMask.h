#include <Iem.h>

#ifndef __LUNGMASK_H_
#define __LUNGMASK_H_

//
// LungMask : encapsulates a lung mask output from lung segmentation
//
// Note that this class is derived from an IemTPlane, so IEM methods may be
// applied to a Lung Mask.
//

class LungMask : public IemTPlane<byte>
{
    public:
          // assignment
        LungMask &operator=(const IemPlane &other)
        {
            *this = LungMask(other);
            return *this;
        }

          // "copy constructor", but for an IemPlane
        LungMask(const IemPlane &other) : IemTPlane<byte>(other)
        {
        }

          // generic constructor
        LungMask() : IemTPlane<byte>() {}

          // overlay lung mask boundaries on an image
        void overlay_lung_boundaries(IemTImage<short> outT2);

          // do lung mask "revision" process, to expand lung mask
        LungMask revise_lung_mask(const IemTPlane<short> &img);

    protected:
        

};


#endif
