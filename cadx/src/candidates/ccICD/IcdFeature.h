//
// icdFeature : Base class for ICD features
//
// The idea here is that all feature-computation classes will be derived from
// this class, and that this class itself will do very little work. 
//
// There are two ways for an application to call this function:
//
//   - "point mode", in which the application provides a specific pixel in the
//     input image at which the feature should be computed. The feature is returned
//     as a double.
//
//   - "multipoint mode", in which the application provides a std::vector of pixels in
//     the input image, the feature is computed at each of the pixels, and a std::vector
//     of features is returned.
//
//   - "image mode", in which the feature is computed at every point in an image,
//     and a float image is returned, having the feature value at every pixel.
//
// A derived class is only required to implement "point mode", because the other
// modes are "synthesized" by the base class. Of course, derived classes are free
// to implement the other modes if they wish, because they may be able to do
// so much more efficiently.
//
// crandall, 7/2003
//

#include <Iem.h>
#include <IemHistogram.h>
#include <IemLinearArray.h>
#include <IemLinearArrayFxns.h>
#include "Rect.h"
#include "IcdImgProc.h"
#include "Afum.h"
#include "IcdXrayImage.h"
#include <iostream>
#include "LungMask.h"


#ifdef WIN32
// This hideously ugly hack is required because Microsoft Visual C++ 6.0
// does not properly implement the ANSI C++ standard regarding variables
// declared within 'for' statements.
#define for if(0) ; else for
#endif


class IcdFeature
{
    public:
        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed=false)
        {
            throw std::string("compute() not implemented for this feature");
        }

          // Do any processing on the whole image, before computing feature itself on
          // the point(s).
          //
        virtual void precompute(IcdXrayImage &img, const LungMask &mask)
        {
        }

        virtual std::vector<double> compute(IcdXrayImage &img, const LungMask &mask, 
                                       const std::vector<Point> &pts)
        {
            std::vector<double> results;

            precompute(img, mask);
            std::vector<Point>::const_iterator pt_iter;
            for(pt_iter = pts.begin(); pt_iter != pts.end(); pt_iter++)
                results.push_back(compute(img, mask, *pt_iter, true));
            
            return results;
        }

        virtual IemTPlane<float> compute(IcdXrayImage &img, const LungMask &mask)
        {
            IemTPlane<float> results;

            precompute(img, mask);
            float *out_cp = results[0];
            for(int i=0; i<img.rows(); i++)
                for(int j=0; j<img.cols(); j++, out_cp++)
                    *out_cp = compute(img, mask, Point(i,j), true);
            
            return results;
        }
};

//
// Now come all the derived classes, to implement the various features.
//
//

// compute variance of local neighborhood
class icdfLocalVar : public IcdFeature
{
    public:
        icdfLocalVar(int _window) : window(_window) {}
        virtual ~icdfLocalVar() {}

        virtual void precompute(IcdXrayImage &img, const LungMask &mask)
        {
            IcdImgProc<short> iip;
            iip.compute_mean_var_under_mask(img.get_xray_image(), mask, lung_mean, lung_variance);
        }

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed=false)
        {
            int window_half = window/2;
            double var = 0;

            if(!precomputed)
                precompute(img, mask);

            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemPlane neigh = iemCut(img.get_xray_image(), pt.first - window_half, pt.second - window_half, 
                                        window, window);
                var = iemVariance(neigh) / lung_variance;
            }

            return var;
        }

    protected:
        int window;
        double lung_mean, lung_variance;
};


// compute variance of local neighborhood
// This is the same as icdfLocalVar, but it normalizes the same way
// B. Senn's java code did (slight difference in way variance is
// computed versus IEM's way)
class icdfBSLocalVar : public IcdFeature
{
    public:
        icdfBSLocalVar(int _window) : window(_window) {}
        virtual ~icdfBSLocalVar() {}

        virtual void precompute(IcdXrayImage &img, const LungMask &mask)
        {
            IcdImgProc<short> iip;
            iip.compute_mean_var_under_mask(img.get_xray_image(), mask, lung_mean, lung_variance);
        }

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed=false)
        {

            if(!precomputed)
                precompute(img, mask);

            return bs_local_var(img.get_xray_image(), mask, pt.first, pt.second, window) / lung_variance;
        }


        double bs_local_var(const IemTPlane<short> &img, const LungMask &mask, int row, int col, int winSize)
        {
            double localVariance;  // our desired result
            
            int radius = winSize/2;
            int numPixels=0;  // area over which variance is computed
              // first get mean of w    indow, and num pixels
            double mean;
            double sum=0.0; 
            for (int r=(row-radius); r<=(row+radius); r++) {
                for (int c=(col-radius); c<=(col+radius); c++) {
                    if (mask[r][c]) {
                        numPixels++;
                        sum += img[r][c];
                    }
                }
            }
            
            mean = sum/numPixels;
              // now get variance around the mean
            double sumDevSq = 0.0;
            double dev;
            for (int r=(row-radius); r<=(row+radius); r++) {
                for (int c=(col-radius); c<=(col+radius); c++) {
                    if (mask[r][c]) {
                        dev = img[r][c] - mean;
                        sumDevSq += dev * dev;
                    }
                }
            }
            localVariance = sumDevSq / (numPixels-1);
            
            return localVariance;
        }


    protected:
        int window;
        double lung_mean, lung_variance;
};



// compute variance of local gradients
class icdfGMVar : public IcdFeature
{
    public:
        icdfGMVar(int _window) : window(_window) {}
        virtual ~icdfGMVar() {}

        virtual void precompute(IcdXrayImage &img, const LungMask &mask)
        {
            IcdImgProc<short> iip;
            iip.compute_mean_var_under_mask(img.get_xray_image(), mask, lung_mean, lung_variance);
        }
        
        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            if(!precomputed)
                precompute(img, mask);

            int window_half = window/2;
            double var = 0;

            IemTPlane<short> grad_mag = img.get_gradient_magnitude();
            
            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemPlane neigh = iemCut(grad_mag, pt.first - window_half, pt.second - window_half, 
                                        window, window);
                var = iemVariance(neigh) / lung_variance;
            }

            return var;
        }

    protected:
        int window;
        double lung_mean, lung_variance;
        IemTPlane<short> grad_mag;
};

// compute normalized y-position within lung region
class icdfNormYPos : public IcdFeature
{
    public:
        void virtual precompute(IcdXrayImage &img, const LungMask &mask)
        {
            top_lung_pos = -1, bottom_lung_pos = -1;
            const byte *cp = mask[0];
            int i;
            for(i=0; i<mask.rows(); i++)
            {
                bool blank=true;
                
                for(int j=0; j<mask.cols() && blank; j++, cp++)
                    if(*cp > 0)
                        blank = false;
                
                if(!blank)
                {
                    top_lung_pos = i;
                    break;
                }
            }
            
            for(i=mask.rows()-1; i>=0; i--)
            {
                const byte *cp = mask[i];
                bool blank = true;
                
                for(int j=0; j<mask.cols() && blank; j++, cp++)
                    if(*cp > 0)
                        blank = false;
                
                if(!blank)
                {
                    bottom_lung_pos = i;
                    break;
                }
            }
        }

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            if(!precomputed)
                precompute(img, mask);

            return((pt.first - top_lung_pos) / double(bottom_lung_pos - top_lung_pos));
        }

    protected:
        int top_lung_pos, bottom_lung_pos;
};


// compute YDiff feature
// This is difference between the means of the neighborhoods above and below the point. The 
// idea is that candidates on rib boundaries would have a higher value for this than 
// actual nodules
class icdfYDiff : public IcdFeature
{
    public:
        icdfYDiff(int _window) : window(_window) {}
        virtual ~icdfYDiff() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            double y_diff = 0;

            IemTPlane<short> xray_image = img.get_xray_image();

            if(pt.first >= window && pt.second >= window &&
               pt.first < img.rows() - window && pt.second < img.cols() - window)
            {
                IemPlane above_neigh = iemCut(xray_image, pt.first - window, pt.second - 1, window, 2);
                IemPlane below_neigh = iemCut(xray_image, pt.first + 1, pt.second - 1, window, 2);
                y_diff = fabs(iemMean(above_neigh) - iemMean(below_neigh));
            }
            return y_diff;
        }

    protected:
        int window;
};


// compute mean of local gradients
class icdfWinGrad : public IcdFeature
{
    public:
        icdfWinGrad(int _window) : window(_window) {}
        virtual ~icdfWinGrad() {}

        virtual void precompute(IcdXrayImage &img, const LungMask &mask)
        {
            IcdImgProc<short> iip;
            iip.compute_mean_var_under_mask(img.get_xray_image(), mask, lung_mean, lung_variance);
        }

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int window_half = window/2;
            double var = 0;
            
            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemPlane neigh = iemCut(img.get_gradient_magnitude(), pt.first - window_half, 
                                        pt.second - window_half, window, window);
                var = iemMean(neigh);
            }

            return var;
        }

    protected:
        int window;
        IemTPlane<short> grad_mag;
        double lung_mean, lung_variance;
};

// compute HorizontalLocation ("hloc")
//
class icdfHorizontalLocation : public IcdFeature
{
    public:
        icdfHorizontalLocation() {}
        virtual ~icdfHorizontalLocation() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int c_min, c_max;
            int peak_row = pt.first;
            int peak_col = pt.second;
            for(c_min=peak_col; c_min >= 0 && mask[peak_row][c_min] > 128; c_min--);
            for(c_max=peak_col; c_max < img.cols() && mask[peak_row][c_max]; c_max++);
            
            double dist;
            if (c_max-peak_col < peak_col-c_min) 
                dist = (c_max - peak_col)/(double)(c_max - c_min);
            else 
                dist = (peak_col - c_min)/(double)(c_max - c_min);
            
            return dist;
        }
};


// compute VerticalLocation ("vloc")
//
class icdfVerticalLocation : public IcdFeature
{
    public:
        icdfVerticalLocation() {}
        virtual ~icdfVerticalLocation() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int r_min, r_max;
            int peak_row = pt.first;
            int peak_col = pt.second;
            for(r_min=peak_row; r_min >= 0 && mask[r_min][peak_col] > 128; r_min--);
            for(r_max=peak_row; r_max < img.rows() && mask[r_max][peak_col]; r_max++);

            double dist;
            if (r_max - peak_row < peak_row - r_min) 
                dist = (r_max - peak_row)/(double)(r_max - r_min);
            else 
                dist = (peak_row - r_min)/(double)(r_max - r_min);
            
            return dist;
        }
};

// compute RelativeDensity ("rdens")
//
class icdfRelativeDensity : public IcdFeature
{
    public:
        icdfRelativeDensity() {}
        virtual ~icdfRelativeDensity() {}

        void virtual precompute(IcdXrayImage &img, const LungMask &mask)
        {
            IemHistogram hist(4096, 0, 4095);

            const byte *mask_cp = mask[0];
            IemTPlane<short> xray_image = img.get_xray_image();
            const short *img_cp = xray_image[0];
            
            count=0;

            for(int i=0; i<img.rows(); i++)
                for(int j=0; j<img.cols(); j++, img_cp++, mask_cp++)
                    if(*mask_cp)
                    {
                        hist[(int)(*img_cp)]++;
                        count++;
                    }
            hist.cvtToCumulative();
            
            histogram = hist;
        }

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            if(!precomputed)
                precompute(img, mask);
            
            return histogram[(int) img.get_xray_image().get(pt.first, pt.second)] / double(count);
        }

    protected:
        int count;
        IemHistogram histogram;
};


// compute Profile Symmetry Mean
//
class icdfProfileSymMean : public IcdFeature
{
    public:
        icdfProfileSymMean(int _window) : window(_window) {}
        virtual ~icdfProfileSymMean() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int window_half = window / 2;

            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemLinearArray v_errors(window_half - 1);
                IemLinearArray h_errors(window_half - 1);
                IemTPlane<short> xray_image = img.get_xray_image();

                for(int i=1; i<window_half; i++)
                {
                    v_errors[i-1] = fabs(xray_image[pt.first - i][pt.second] - 
                                         xray_image[pt.first + i][pt.second]);
                    h_errors[i-1] = fabs(xray_image[pt.first][pt.second-1] - 
                                         xray_image[pt.first][pt.second + i]);
                }
                
                IemPlane neigh = iemCut(xray_image, pt.first - window_half, pt.second - window_half, 
                                        window, window);
                double norm_factor = iemMaximum(neigh) - iemMinimum(neigh) + 0.01;
              
                return (iemMean(v_errors) + iemMean(h_errors)) / norm_factor;
            }

            return 0;
        }

    protected:
        int window;
};



// compute Profile Symmetry Variance
//
class icdfProfileSymVar : public IcdFeature
{
    public:
        icdfProfileSymVar(int _window) : window(_window) {}
        virtual ~icdfProfileSymVar() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int window_half = window / 2;

            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemLinearArray v_errors(window_half - 1);
                IemLinearArray h_errors(window_half - 1);
                IemTPlane<short> xray_image = img.get_xray_image();

                for(int i=1; i<window_half; i++)
                {
                    v_errors[i-1] = fabs(xray_image[pt.first - i][pt.second] - 
                                         xray_image[pt.first + i][pt.second]);
                    h_errors[i-1] = fabs(xray_image[pt.first][pt.second-1] - 
                                         xray_image[pt.first][pt.second + i]);
                }
                
                return iemVariance(v_errors) + iemVariance(h_errors);
            }

            return 0;
        }

    protected:
        int window;
};




// compute Profile Symmetry Mean
//
class icdfGradProfileSymMean : public IcdFeature
{
    public:
        icdfGradProfileSymMean(int _window) : window(_window) {}
        virtual ~icdfGradProfileSymMean() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int window_half = window / 2;

            IemTPlane<short> x_grad = img.get_x_sobel();
            IemTPlane<short> y_grad = img.get_y_sobel();

            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemLinearArray v_errors(window_half - 1);
                IemLinearArray h_errors(window_half - 1);
                for(int i=1; i<window_half; i++)
                {
                    v_errors[i-1] = fabs(y_grad[pt.first - i][pt.second] - 
                                         y_grad[pt.first + i][pt.second]);
                    h_errors[i-1] = fabs(x_grad[pt.first][pt.second-1] - 
                                         x_grad[pt.first][pt.second + i]);
                }
                
                IemPlane neigh = iemCut(img.get_xray_image(), pt.first - window_half, pt.second - window_half, 
                                        window, window);
                double norm_factor = iemMaximum(neigh) - iemMinimum(neigh) + 0.01;
//                double norm_factor = 1;
                
                return (iemMean(v_errors) + iemMean(h_errors)) / norm_factor;
            }

            return 0;
        }

    protected:
        int window;
        IemTPlane<short> x_grad, y_grad;
};



// compute Profile Symmetry Variance
//
class icdfGradProfileSymVar : public IcdFeature
{
    public:
        icdfGradProfileSymVar(int _window) : window(_window) {}
        virtual ~icdfGradProfileSymVar() {}

        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            int window_half = window / 2;

            IemTPlane<short> x_grad = img.get_x_sobel();
            IemTPlane<short> y_grad = img.get_y_sobel();

            if(pt.first >= window_half && pt.second >= window_half &&
               pt.first < img.rows() - window_half && pt.second < img.cols() - window_half)
            {
                IemLinearArray v_errors(window_half - 1);
                IemLinearArray h_errors(window_half - 1);
                for(int i=1; i<window_half; i++)
                {
                    v_errors[i-1] = fabs(y_grad[pt.first - i][pt.second] - 
                                         y_grad[pt.first + i][pt.second]);
                    h_errors[i-1] = fabs(x_grad[pt.first][pt.second-1] - 
                                         x_grad[pt.first][pt.second + i]);
                }
                
                return iemVariance(v_errors) + iemVariance(h_errors);
            }

            return 0;
        }

    protected:
        int window;
       IemTPlane<short> x_grad, y_grad;
};


// AFUM feature
//
class icdfAfum : public IcdFeature
{
    public:
        icdfAfum(int radius, int _delta_r) : delta_r(_delta_r) 
        {
            afum = new Afum(radius);
        }


        virtual double compute(IcdXrayImage &img, const LungMask &mask, const Point &pt,
                               bool precomputed = false)
        {
            throw std::string("computing AFUM feature for a single point is not currently implemented.");
        }

        virtual void precompute(IcdXrayImage &img, const LungMask &mask)
        {
            Afum afum(30);
            double actual_spacing = img.get_pixel_spacing() * 1000.0;

            double desired_spacing = 300.0;

            std::cout << "into median subsample" << std::endl;
            IcdImgProc<short> iip;
            IemTPlane<short> big_img_rev = 32767-img.get_xray_image();
            medsub_big_img = iip.median_subsample(big_img_rev, actual_spacing, desired_spacing);
            int afum_subsamp = (int)ceil((double)(desired_spacing / actual_spacing));
            
            std::cout << "afum_subsamp " << afum_subsamp << std::endl;
            afum_sized_mask = iemSubsample(mask, afum_subsamp, afum_subsamp);
            std::cout << "out of med sub" << std::endl;
            scale_factor = afum_subsamp;
        }

        virtual std::vector<double> compute(IcdXrayImage &img, const LungMask &mask, 
                                       const std::vector<Point> &pts)
        {
            precompute(img, mask);

            std::cout << "running AFUM..." << std::endl;
            return afum->apply_filter(medsub_big_img, delta_r, afum_sized_mask, pts, scale_factor);
        }

        virtual IemTPlane<float> compute(IcdXrayImage &img, const LungMask &mask)
        {
            precompute(img, mask);

            return afum->apply_filter(medsub_big_img, delta_r, afum_sized_mask);
        }

        virtual ~icdfAfum()
        {
            delete afum;
        }

    protected:
        int delta_r;
        Afum *afum;
        IemPlane medsub_big_img, afum_sized_mask;
        int scale_factor;
};


