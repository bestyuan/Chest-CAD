#include <Iem.h>
#include "IcdImgProc.h"
#include "CandidateList.h"

#ifndef __MHAC_H_
#define __MHAC_H_

// 
// Mhac : encapsulates Bob Senn's MHAC algorithm
//
// This is a C++/IEM port of Bob Senn's MHAC algorithm, described in 
// B. Senn, Initial Candidate Detection for CADX: Preliminary Results,
// TM 332154R, 2/1/2003. 
//
// Original notes, from his source code:
//
// /**
//  * This class provides an initial candidate detection algorithms for
//  * CADX.  The implementation is similar to the Fractional Dimension
//  * Filter of Mao et al of the University of South Florida. (see 
//  * F. Mao, W. Qian, and L. Clarke, "Fractional Dimension Filtering for 
//  * Multiscale Lung Nodule Detection", SPIE Vol 3034, p449-456, 1997.)
//  * except it is not "fractional dimesnion" in nature.
//  * This is essentially the same as Kobatake's convergence index filter
//  * (COIN) of Univ. of Tokyo.  This version was derived from FracDimFilter.java
//  * with modifications so support the non-interactive testing of Test0
//  * scheduled for 11/6/02.
//  * The algorithm is essentially a modified version of the Hough Transform
//  * based circular collimation detection routine that was developed for
//  * Kodak/HI back in 1996.  It accumulates the gradient direction 
//  * (or its direction cosine) over all the pixels of a cirular 
//  * neighborhood.
//  */

class Mhac
{
    public:
          // constructor, assumes radius of 0 (not really useful)
        Mhac() : radius(0) {}

          // constructor, with a given radius
        Mhac(int _radius) : radius(_radius) {}

          // run Mhac.
        CandidateList generate_initial_candidates(IcdXrayImage &img, const IemTPlane<byte> &lung_mask, 
                                                  int scale_factor);
        void set_radius(int _radius) 
        {
            radius = _radius;
        }

    protected:
        IemPlane normalize_gradient(const IemTPlane<short> &grad, const IemTPlane<short> &magnitude);
        IemPlane fd_filter(IcdXrayImage &img, int radius, const IemTPlane<byte> &mask);
        CandidateList detect_peaks(const IemTPlane<float> &img, const IemTPlane<byte> &mask, int scale_factor);

        IemTPlane<short> grad_mag, x_grad, y_grad;
        int radius;

};
#endif
