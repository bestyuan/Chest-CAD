#include "Mhac.h"
#include <iostream>
using namespace std;

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

#ifdef WIN32
// This hideously ugly hack is required because Microsoft Visual C++ 6.0
// does not properly implement the ANSI C++ standard regarding variables
// declared within 'for' statements.
#define for if(0) ; else for
#endif

extern bool debug_g, nobug_fix_g;



IemPlane Mhac::normalize_gradient(const IemTPlane<short> &grad, const IemTPlane<short> &magnitude)
{
    IemTPlane<float> result(grad.rows(), grad.cols());

    int GRAD_LOWER_THRESHOLD = 5;
    int GRAD_UPPER_THRESHOLD = 200;

    const short *grad_cp = grad[0], *mag_cp = magnitude[0];
    float *res_cp = result[0]; 
    for(int i=0; i<grad.rows(); i++)
        for(int j=0; j<grad.cols(); j++, res_cp++, grad_cp++, mag_cp++)
            if(*mag_cp < GRAD_LOWER_THRESHOLD || *mag_cp > GRAD_UPPER_THRESHOLD)
                *res_cp = 0;
            else
                *res_cp = *grad_cp / double(*mag_cp);

    return(result);
}


IemPlane Mhac::fd_filter(IcdXrayImage &img, int radius, const IemTPlane<byte> &mask)
{
    cout << "applying sobel operators...\n";

    IcdImgProc<short> iip;

    x_grad = img.get_x_sobel();
    y_grad = img.get_y_sobel();
    grad_mag = img.get_gradient_magnitude();

    grad_mag = iip.apply_mask(grad_mag, mask);

    cout << "normalizing edge magnitudes...\n";

    if(debug_g)
    {
        iemWrite(grad_mag, "grad_mag.tif");
        iemWrite(x_grad, "x_grad.tif");
        iemWrite(y_grad, "y_grad.tif");
    }

    IemTPlane<float> x_norm_grad = normalize_gradient(x_grad, grad_mag);
    IemTPlane<float> y_norm_grad = normalize_gradient(y_grad, grad_mag);

    if(debug_g)
    {
        iemWrite(x_norm_grad, "x_norm_grad.tif");
        iemWrite(y_norm_grad, "y_norm_grad.tif");
    }

    IemTPlane<float> result(img.rows(), img.cols());
    result = 0;

    double radSqr = radius*radius;

    int rows = img.rows(), cols = img.cols();

    int *dxes = new int[radius * 2 + 1];

    for(int j=0, i=-radius; i<=radius; i++, j++)
        dxes[j] = int(sqrt(radSqr - i*i));

    double *cos_theta_arr = new double[radius * 2 + 1];
    for(int i=-radius, i2=0; i <= radius; i++, i2++)
        cos_theta_arr[i2] = i / double(radius);

    cout << "computing COIN feature...\n";
    for(int r=radius; r < rows - radius; r++)
    {
        float *result_cp = result[r] + radius;

        for(int c=radius; c < cols - radius; c++, result_cp++)
        {
            int *dx_cp = dxes;
            float *in_x_cp_row = x_norm_grad[r-radius], *in_y_cp_row =y_norm_grad[r-radius];

            for(int i=-radius; i<=radius; i++, dx_cp++, in_x_cp_row += cols, in_y_cp_row += cols)
            {
                int dx = *dx_cp;
                
                float *in_x_cp = in_x_cp_row + c - dx;
                float *in_y_cp = in_y_cp_row + c - dx;

                double sin_theta = i / double(radius);
                double *cos_theta_cp = cos_theta_arr + radius - dx;

                for(int j = -dx; j<=dx; j++, in_x_cp++, in_y_cp++, cos_theta_cp++)
                {
                    double cos_theta = *cos_theta_cp;

                    *result_cp += (cos_theta) * (*in_x_cp) + sin_theta * (*in_y_cp);
                }
            }
        }
    }

	delete[] dxes;
	delete[] cos_theta_arr;

      // crandall -- We're now normalizing by the area of the circular filter.
      // Note, of course, that really we should be normalizing by pi*radius^2, but
      // radius^2 is sufficient because we don't care about the absolute magnitude
      // of COIN (just the relative value).
    result /= radius * radius;

    return result;
}



CandidateList Mhac::generate_initial_candidates(IcdXrayImage &img, const IemTPlane<byte> &lung_mask, 
                                                int scale_factor)
{
    assert(lung_mask.rows() == img.rows() && lung_mask.cols() == img.cols());

      // This is the brain-dead way we fixed the "false alarms along lung boundaries" bug.
      // Just calculate the COIN filter on ALL pixels in the image, instead of just those
      // under the lung mask. This is accomplised by passing a solid white image as the
      // lung mask. Note that for computational efficiency reasons, this should
      // really be substituted with a more graceful fix, like computing COIN for
      // a few pixels outside the lung mask (e.g. dilating the lung mask and applying
      // COIN using it).

    IemTPlane<byte> lung_mask2 = lung_mask.copy();
    if(!nobug_fix_g)
        lung_mask2 = 255;

    IemTPlane<float> fdfImg = fd_filter(img, radius, lung_mask2);

    cout << "doing mean filter...\n";
    IcdImgProc<float> iip;
    fdfImg = iip.apply_mean_filter(IemTImage<float>(fdfImg), radius)[0];

      // This next line of code is one of the most perplexing parts of all of the ICD code.
      // It turns out that if you normalize COIN based on the area underneath the filter 
      // (i.e. by radius^2), and then try to compare COIN values computed across different
      // scales, you find that COIN values computed at smaller radii are disproportionately low.
      // It seems that we really want to normalize by radius, not by radius^2.
      // But why? I'm not sure.

    fdfImg *= radius;

    IemTPlane<byte> lung_mask3 = lung_mask.copy();
    
    if(debug_g)
        iemWrite(fdfImg, "fdfimg.tif");

    cout << "doing peak detection...\n";
    return detect_peaks(fdfImg, lung_mask3, scale_factor);
}



CandidateList Mhac::detect_peaks(const IemTPlane<float> &img, const IemTPlane<byte> &mask, int scale_factor)
{

    CandidateList result;

      // ignore boundary of 10 (was 2). The coin features are not accurate
      // near the image boundaries.
    for(int r=10; r < img.rows() - 10; r++)
    {
        const byte *mask_cp = mask[r]+10;
        const float *in_cp = img[r]+10;

        for(int c=10; c < img.cols() - 10; c++, in_cp++, mask_cp++)
        {
            if(*mask_cp > 127)
            {
                bool local_peak = true;

                for(int dr=-1; dr <= 1; dr++)
                {
                    const float *neigh_cp = img[r-dr] + c - 1;
                    for(int dc=-1; dc <= 1; dc++, neigh_cp++)
                    {
                        if(*neigh_cp > *in_cp)
                            local_peak = false;
                    }
                }

                if(local_peak)
                    result.push_back(Candidate(r, c, *in_cp, scale_factor));
            }
        }
    }

    return result;
}
