//
// IcdXrayImage : Encapsulates an x-ray image and various image processing
//                algorithms.
//
// This class was developed to prevent different parts of the ICD code from
// having to re-preform the same image processing tasks (edge detection, etc.).
// This was a problem because we don't want to require that certain features
// (IcdFeature derived classes) be computed before others, since features
// may very likely be added and removed throughout the course of research.
// 
// This class provides a clean mechanism for accomplishing this. An IcdXrayImage
// is created with a given IEM image cooresponding to an x-ray. The class
// handles requests for various commonly-used image processing routines,
// and caches the results so that subsequent calls to the routines take
// trivial computation time.
//

#ifndef __ICDXRAYIMAGE_H_
#define __ICDXRAYIMAGE_H_

#include <Iem.h>
#include "IcdImgProc.h"

class IcdXrayImage
{
    public:
        IcdXrayImage(const IemTPlane<short> &_xray_image) 
        {
            xray_image = _xray_image.copy();
            x_sobel_valid = y_sobel_valid = grad_mag_valid = false;
        }
        IcdXrayImage(const std::string &filename)
        {
            kesprRead(filename);
            x_sobel_valid = y_sobel_valid = grad_mag_valid = false;
        }
        
        IemTPlane<short> get_x_sobel()
        {
            if(!x_sobel_valid)
            {
                x_sobel = iip.xSobel(xray_image);
                x_sobel_valid = true;
            }

            return x_sobel;
        }

        IemTPlane<short> get_y_sobel()
        {
            if(!y_sobel_valid)
            {
                y_sobel = iip.ySobel(xray_image);
                y_sobel_valid = true;
            }
            
            return y_sobel;
        }

        IemTPlane<short> get_gradient_magnitude()
        {
            if(!grad_mag_valid)
            {
                IemTPlane<short> x, y;
                x = get_x_sobel();
                y = get_y_sobel();
                
                grad_mag = iip.gradient_magnitude(x, y);
                grad_mag_valid = true;
            }

            return grad_mag;
        }

          // return the actual x-ray image
        IemTPlane<short> get_xray_image()
        {
            return xray_image;
        }

          // query number of rows, columns in x-ray image
        int rows()
        {
            return xray_image.rows();
        }

        int cols()
        {
            return xray_image.cols();
        }

          // retrieve pixel spacing information from KESPR file
        double get_pixel_spacing() const { return pix_spacing; }

          // read a KESPR image from a file into this object
        void kesprRead(const std::string &filename); 

    protected:
        IemTPlane<short> xray_image;
        IemTPlane<short> x_sobel, y_sobel, grad_mag;
        IcdImgProc<short> iip;
        bool x_sobel_valid, y_sobel_valid, grad_mag_valid;
        double pix_spacing;
};

#endif
