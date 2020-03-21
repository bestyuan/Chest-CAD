#include "LungMask.h"

//
// LungMask : encapsulates a lung mask output from lung segmentation
//
// Note that this class is derived from an IemTPlane, so IEM methods may be
// applied to a Lung Mask.
//

//
// overlay_lung_boundaries
//
// Draw boundaries of lung mask on image outT2.
//
void LungMask::overlay_lung_boundaries(IemTImage<short> outT2)
{
    byte *mask_cp = (*this)[0];

    for(int i=0; i<outT2.rows(); i++)
        for(int j=0; j<outT2.cols(); j++, mask_cp++)
        {
            if(j < outT2.cols()-1 && *mask_cp != *(mask_cp+1) ||
               i < outT2.rows()-1 && *mask_cp != *(mask_cp + outT2.cols()))
                outT2[0][i][j] = outT2[1][i][j] = outT2[2][i][j] = 2048;
        }
}


//
// revise_lung_mask
//
// Do lung mask revision (expansion), based on local mean/variance
// criteria.
//
// This is a very simple process, really just a "proof-of-concept"
// more than anything that should be used in product.
//
LungMask LungMask::revise_lung_mask(const IemTPlane<short> &img)
{
      // first determine mean of pixels underneath current mask
    const byte *mask_cp = (*this)[0];
    const short *img_cp = img[0];
    double mean=0;
    int count=0;

    for(int i=0; i<img.rows(); i++)
        for(int j=0; j<img.cols(); j++, img_cp++, mask_cp++)
            if(*mask_cp)
            {
                count++;
                mean+=*img_cp;
            }

    mean /= count;
    LungMask new_mask = copy();

      // iteratively grow
    for(int n=0; n<30; n++)
    {
        LungMask thisiter_in_mask = new_mask.copy();
        byte *in_mask_cp = thisiter_in_mask[0];
        byte *out_mask_cp = new_mask[0];
        img_cp = img[0];
        for(int i=0; i<new_mask.rows()-1; i++)
            for(int j=0; j<new_mask.cols(); j++, in_mask_cp++, out_mask_cp++, img_cp++)
            {
                if(!(i >= 10 && j >= 10 && i < new_mask.rows() - 10 && j < new_mask.cols() - 10))
                    continue;

                if(*in_mask_cp == 0 && *(in_mask_cp+1) > 0)
                {
                    IemPlane neigh = iemCut(img, i-5, j-5, 11, 11);
                    if(iemMean(neigh) > mean / 1.15)
                        *out_mask_cp = 255;
                }
                else if(*in_mask_cp > 0 && *(in_mask_cp+1) == 0)
                {
                    IemPlane neigh = iemCut(img, i-5, j-5, 11, 11);
                    if(iemMean(neigh) > mean / 1.15)
                        *(out_mask_cp+1) = 255;
                }
                else if(*in_mask_cp > 0 && *(in_mask_cp + new_mask.cols()) == 0)
                {
                    IemPlane neigh = iemCut(img, i-5, j-5, 11, 11);
                    if(iemMean(neigh) > mean / 1.15)
                        *(out_mask_cp + new_mask.cols()) = 255;
                }
            }
    }

    return new_mask;
}
