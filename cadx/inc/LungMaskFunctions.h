#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "Iem/Iem.h"
#include "Iem/IemLinearArray.h"
#include "Iem/IemLinearArrayFxns.h"
#include "Iem/IemLinearArrayOps.h"




void linear_fit_P(int st_fit, int tot_point, IemLinearArray &line_dif, 
                  double &slope, double &offset, double &average, double &chi2);
void boost_line(IemLinearArray &Left_Lung_Line, IemLinearArray &Left_Lung_Copy, 
				double boost_factor, int limit=0);
void validate_line(IemPlane &plane, IemLinearArray &spine,IemLinearArray &lung);
IemPlane KESPR2IEM(const char *filename);
void Fill_Hist(IemPlane &img, IemLinearArray &hist1, IemLinearArray &hist2, int th1, int th2);
IemLinearArray Hist_Remove_Invalid(IemLinearArray val, int &shift);
int Locate_Maximum(IemLinearArray &hist, double f_edge, int &locs);
int Max_Peak(IemLinearArray &data, int win, int st, double *pars);
int p2fit(int np, IemLinearArray& array, int start, double *par);
int Bottom_Search(IemLinearArray &hist, int win, int start, int step, double *pars);
IemLinearArray Regional_Hist(IemPlane img, int x1, int x2, int th1, int th2);
double Diff_Mean_Striped(IemLinearArray hist,double Sfactor);
IemPlane find_ridegs(IemPlane &img);
IemLinearArray LAverage(IemLinearArray a, int win);
void peak_vally_locations( IemLinearArray &a, int locs0, int *locs, double to_edge);
IemLinearArray Line_Rescale(IemLinearArray &a, int scale , int new_size, int shift1, int shift2);
IemLinearArray get_lung_value(IemPlane &img,IemLinearArray &Spine_Line,IemLinearArray &Left_Lung_Line);

//------------------------------------------------------------------------------
// hiGaussianBlur copied from iuGaussianBlur
//------------------------------------------------------------------------------
//: Returns typed IEM plane smoothed with gaussian filter
template <class PixelType>
inline IemTPlane<PixelType> hiGaussianBlur(const IemTPlane<PixelType> &src, float sigma = 1.0, float sigfactor = 3.0)
{
  int src_rows = src.rows();
  int src_cols = src.cols();

  // Smooth the image using separable Gaussian filters
  // The sigma parameter defines the size of the smoothing kernel.
  // The kernel size is 1xn where n = 2*ceil(sigfactor*sigma)+1, 
  // where sigfactor defaults to 3 because 6*sigma includes 99.7% 
  // of the energy of the Gaussian function, and 1 makes it odd.
  float half_glen = sigfactor*sigma;
  // round up to integer
  if (half_glen - (float)((int)half_glen))
    half_glen += 1.0;
  int gcenter = (int)half_glen;
  int gaussian_len = 1 + 2 * gcenter;
  double sigscale = -0.5/(sigma*sigma);
  double *gaussian = new double[gaussian_len];
  double sigsum = 0;
  int ii;
  for (ii = -gcenter; ii <= gcenter; ii++)
  {
    double gtmp = exp(ii * ii * sigscale);
    gaussian[gcenter + ii] = gtmp;
    sigsum += gtmp;
  }
  // renormalize it to sum to 1.0
  for (ii = -gcenter; ii <= gcenter; ii++)
    gaussian[gcenter + ii] /= sigsum;

  if ((src_cols < gaussian_len) || (src_rows < gaussian_len))
  {
//    cerr << "hiGaussianBlur: blur kernel exceeds image dimensions...skipping\n";
 //   return src;
	IemTPlane<PixelType> tSrc;
	tSrc = src;
	return tSrc;
  }

  // It is faster to convolve the 1D filter over a row than over a column 
  // therefore, in preparation for the second pass we will transpose the 
  // image while convolving the first pass. In the second pass we will 
  // transpose it back.
  IemTPlane<float> smooth_tmp(src_cols, src_rows);  // transposed
  // convolve horizontally and transpose
  int jj;
  for (jj = 0; jj < src_rows; jj++)
  {
    const PixelType *src_jj = src[jj];
    // left edge 
    for (ii = 0; ii < gcenter; ii++)
    {
      double sum = 0;
      double norm = 0;
      for (int kk = -ii; kk <= gcenter; kk++)
      {
        norm += gaussian[gcenter + kk];
        sum += gaussian[gcenter + kk] * src_jj[ii+kk];
      }
      smooth_tmp[ii][jj] = (float)(sum/norm);
    }
    // center (normal)
    for (; ii < src_cols-gcenter; ii++)
    {
      double sum = 0;
      for (int kk = -gcenter; kk <= gcenter; kk++)
        sum += gaussian[gcenter + kk] * src_jj[ii+kk];
      smooth_tmp[ii][jj] = (float)sum;
    }
    // right edge 
    for (; ii < src_cols; ii++)
    {
      double sum = 0;
      double norm = 0;
      for (int kk = -gcenter; kk < src_cols-ii; kk++)
      {
        norm += gaussian[gcenter + kk];
        sum += gaussian[gcenter + kk] * src_jj[ii+kk];
      }
      smooth_tmp[ii][jj] = (float)(sum/norm);
    }
  }

  // convolve horizontally again and re-transpose 
  IemTPlane<PixelType> smooth(src_rows, src_cols);  // re-transposed
  for (jj = 0; jj < smooth_tmp.rows(); jj++)
  {
    float *smooth_tmp_jj = smooth_tmp[jj];
    // left edge 
    for (ii = 0; ii < gcenter; ii++)
    {
      double sum = 0;
      double norm = 0;
      for (int kk = -ii; kk <= gcenter; kk++)
      {
        norm += gaussian[gcenter + kk];
        sum += gaussian[gcenter + kk] * smooth_tmp_jj[ii+kk];
      }
      smooth[ii][jj] = (PixelType)(sum/norm);
    }
    // center (normal)
    for (; ii < smooth_tmp.cols()-gcenter; ii++)
    {
      double sum = 0;
      for (int kk = -gcenter; kk <= gcenter; kk++)
        sum += gaussian[gcenter + kk] * smooth_tmp_jj[ii+kk];
      smooth[ii][jj] = (PixelType)sum;
    }
    // right edge 
    for (; ii < smooth_tmp.cols(); ii++)
    {
      double sum = 0;
      double norm = 0;
      for (int kk = -gcenter; kk < smooth_tmp.cols()-ii; kk++)
      {
        norm += gaussian[gcenter + kk];
        sum += gaussian[gcenter + kk] * smooth_tmp_jj[ii+kk];
      }
      smooth[ii][jj] = (PixelType)(sum/norm);
    }
  }
  delete[] gaussian;
  return smooth;
}

//: Returns typed IEM image smoothed with gaussian filter
template <class PixelType>
inline IemTImage<PixelType> hiGaussianBlur(
    const IemTImage<PixelType> &src, 
    float sigma = 1.0, 
    float sigfactor = 3.0)
{
    const int chans = src.chans();
    IemTImage<PixelType> out(chans, src.rows(), src.cols());

    for (int i=0; i<chans; i++) {
        out[i] = hiGaussianBlur(src[i], sigma, sigfactor);
    }

    return out;
}

//------------------------------------------------------------------------------
// hiGaussianBlur
//------------------------------------------------------------------------------
//: Returns un-typed IEM plane smoothed with gaussian filter
IemPlane hiGaussianBlur(const IemPlane &src, float sigma = 1.0, float sigfactor = 3.0);


//: Returns un-typed IEM image smoothed with gaussian filter
IemImage hiGaussianBlur(const IemImage &src, float sigma = 1.0, float sigfactor = 3.0);





#endif
