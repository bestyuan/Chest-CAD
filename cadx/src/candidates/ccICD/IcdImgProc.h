#ifndef __ICDIMGPROC_H_
#define __ICDIMGPROC_H_

#include <Iem.h>

// IcdImgProc : image processing routines for initial candidate detection
//
// crandall, 7/2003
// individual routines from various sources; see comments with each
//

template<class T>
class IcdImgProc
{
    public:
          // median subsample routine, from Mike Heath
        IemPlane median_subsample(const IemTPlane<T> &image, float actual_pixelspacing_microns,
                                  float desired_pixelspacing_microns);

          // methods to apply sobel edge detection
        IemPlane xSobel(const IemTPlane<T> &img);
        IemPlane ySobel(const IemTPlane<T> &img);

          // computes the mean and variance of pixels underneath a mask
        void compute_mean_var_under_mask(const IemTPlane<T> &img, const IemTPlane<byte> &in_mask,
                                         double &mean, double &variance);

          // downsamples in half, from Bob Senn
        IemImage downSample2x(const IemTImage<T> &img);

          // applies a mask to an image (zeros pixels outside the mask)
        IemPlane apply_mask(const IemTPlane<T> &img, const IemTPlane<byte> &mask);

          // apply a square mean filter to an image, efficiently
        IemImage apply_mean_filter(const IemTImage<T> &image, 
                                   int filter_size);

          // compute the gradient magnitude, given gradients in the x and y directions.
        IemPlane gradient_magnitude(const IemTPlane<T> &x, const IemTPlane<T> &y);
};

#endif
