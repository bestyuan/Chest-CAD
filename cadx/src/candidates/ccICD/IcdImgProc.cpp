#include "IcdImgProc.h"
#include "IcdUtils.h"

// IcdImgProc : image processing routines for initial candidate detection
//
// crandall, 7/2003
// individual routines from various sources; see comments with each
//

#ifdef WIN32
// This hideously ugly hack is required because Microsoft Visual C++ 6.0
// does not properly implement the ANSI C++ standard regarding variables
// declared within 'for' statements.
#define for if(0) ; else for
#endif

// performs a median subsample on an image.
//    
// Borrowed/stolen from M. Heath's AFUM code.
// Ported to IEM and STL by crandall, 6/2003.
//
// note (by crandall): This code has not been optimized at all. In particular,
// Iem's convenient (but notoriously slow) "[]" operators are being used for
// pixel gets and puts. 
//
template<class T>
IemPlane IcdImgProc<T>::median_subsample(const IemTPlane<T> &image, float actual_pixelspacing_microns,
                                      float desired_pixelspacing_microns)
{
	int aggsize=0;
    int smallrows, smallcols;

	aggsize = (int)ceil((double)(desired_pixelspacing_microns / actual_pixelspacing_microns));

	if(aggsize < 1) 
        aggsize = 1;

    int rows = image.rows(), cols = image.cols();

    smallrows = rows / aggsize;
	if((smallrows * aggsize) < rows) 
        smallrows++;

	smallcols = cols / aggsize;
    if((smallcols * aggsize) < cols) 
        smallcols++;

    IemTPlane<T> smallimage(smallrows, smallcols);

    T *small_cp = smallimage[0];
	for(int r=0; r<smallrows; r++)
		for(int c=0; c<smallcols; c++, small_cp++)
        {
            std::set<T> med_buf;

			for(int ra=0, rr=aggsize*r; ra<aggsize; ra++,rr++)
				for(int ca=0, cc=aggsize*c; ca<aggsize; ca++,cc++)
                {
					if(rr<rows && cc<cols)
                        med_buf.insert(image[rr][cc]);
                }

              // if we saw less than two samples, just choose the first one
			if(med_buf.size() <= 2)
                *small_cp = *(med_buf.begin());
			else // else choose the midpoint (note that set is already sorted)
            {
                std::set<T>::iterator iter = med_buf.begin();
                for(unsigned int i=0; i<med_buf.size()/2; i++)
                    iter++;

                *small_cp = *iter;
            }
                
		}

    return smallimage;
}


// computes image gradient in x-direction, according to sobel operators
//
// from B. Senn's java code
// ported to IEM by crandall, 4/2003
//
template<class T>
IemPlane IcdImgProc<T>::xSobel(const IemTPlane<T> &img)
{
    double h_kernel[] = {-1.0, 0.0, 1.0};
    double v_kernel[] = { 1.0, 2.0, 1.0};

    IemTPlane<float> result = iemConvert(img, IemFloat);
    IemKernelPlane sobel_h(1, 3, h_kernel), sobel_v(3, 1, v_kernel);
    
      // note: multiplications by norm() here are required to un-normalize
      // the convolutions, in order to mimic the behavior B. Senn's java code
    result %= sobel_h;
    result *= sobel_h.norm();
    result %= sobel_v;
    result *= sobel_v.norm();

    return(iemConvert(result, img.type()));
}


// computes image gradient in y-direction, according to sobel operators
//
// from B. Senn's java code
// ported to IEM by crandall, 4/2003
//
template<class T>
IemPlane IcdImgProc<T>::ySobel(const IemTPlane<T> &img)
{
    double h_kernel[] = { 1.0, 2.0, 1.0};
    double v_kernel[] = {-1.0, 0.0, 1.0};

    IemTPlane<float> result = iemConvert(img, IemFloat);
    IemKernelPlane sobel_h(1, 3, h_kernel), sobel_v(3, 1, v_kernel);
    
      // note: multiplications by norm() here are required to un-normalize
      // the convolutions, in order to mimic the behavior B. Senn's java code
    result %= sobel_h;
    result *= sobel_h.norm();
    result %= sobel_v;
    result *= sobel_v.norm();

    return(iemConvert(result, img.type()));
}


// computes the mean and variance of the pixels lying underneath a mask
//
// img      : input image
// in_mask  : image mask, where 0 is "off" and non-zero is "on"
// mean     : (output) mean of pixels in img whose corresponding pixels in in_mask are on
// variance : (output) variance of pixels in img whose corresponding pixels in im_mask are on
//
// crandall, 5/2003
//
template<class T>
void IcdImgProc<T>::compute_mean_var_under_mask(const IemTPlane<T> &img, const IemTPlane<byte> &in_mask,
                                                double &mean, double &variance)
{
    const byte *mask_cp = in_mask[0];
    const T *img_cp = img[0];
    int count=0;

    if(img.rows() != in_mask.rows() && img.cols() != in_mask.cols())
        throw std::string("compute_mean_var_under_mask: image and mask are different sizes");

    mean = variance = 0;

    for(int i=0; i<img.rows(); i++)
        for(int j=0; j<img.cols(); j++, img_cp++, mask_cp++)
            if(*mask_cp)
            {
                count++;
                mean+=*img_cp;
            }

    mean /= count;

    mask_cp = in_mask[0];
    img_cp = img[0];
    for(int i=0; i<img.rows(); i++)
        for(int j=0; j<img.cols(); j++, img_cp++, mask_cp++)
            if(*mask_cp)
                variance += (*img_cp - mean) * (*img_cp - mean);
    
    variance /= (count - 1);
}




// down-sample an image to one-quarter the original size (1/2 in each dimension)
//
// borrowed/stolen from B. Senn's java code
// ported to IEM by crandall, 4/2003
//
template<class T>
IemImage IcdImgProc<T>::downSample2x(const IemTImage<T> &img)
{
    double kernel[] = {-0.1016,0.2578,0.6875,0.2578,-0.1016};
    int offset = 2;
    int numRows = img.rows();
    int numCols = img.cols(); 
    int numBands = img.chans(); 
    int newRows = numRows/2;
    int newCols = numCols/2;

    IcdUtils icdu;

      // Note that we are going to downsample on the fly in a separable fashion.
      // Therefore the intermediate buffer (hresult)needs only numCols/2 cols 
      // but we will still need every row for the subsequent vertical filtering
      // which produces a buffer downsampled in both directions.
    IemTImage<T> hresult(img.chans(), numRows, newCols);

      double sum;
      // center field, horizontal first, ranging over the orig pixels

      for (int b=0; b<numBands; b++) {
         for (int r=0; r<numRows; r++) {      
            // ignore ends and skip every other col which would get tossed 
             const T *outer_cp = &(img[b][r][2]);
             T *out_cp = &(hresult[b][r][1]);
            for (int c=2; c<numCols-2; c+=2, outer_cp+=2, out_cp++) {
               sum = 0.0;
               const T *inner_cp = outer_cp - offset;
               double *kernel_cp = kernel;

               for (int o=-offset; o<=offset; o++, inner_cp++, kernel_cp++) 
                   sum += *inner_cp * (*kernel_cp);

               // note output column is ajusted to reflect downsampling
               *out_cp = icdu.rint(sum);
            }

            // do left end pixel (c=0)
            sum = img[b][r][0] * kernel[offset+0];
            for (int o=1; o<=offset; o++) 
               sum += 2.0*img[b][r][o] * kernel[offset+o];
            hresult[b][r][0] = icdu.rint(sum);
            
            // do right end pixel (c=numCols-1 or newCols-1)
            sum = img[b][r][numCols-1] * kernel[offset+0];
            for (int o=1; o<=offset; o++) 
               sum += 2.0*img[b][r][numCols-1-o] * kernel[offset-o];
            hresult[b][r][newCols-1] = icdu.rint(sum);
         }
      }

      // repeat for vertical (still center field only)
      IemTImage<T> result(img.chans(), newRows, newCols);
      // center field, horizontal first, ranging over the orig pixels
      for (int b=0; b<numBands; b++) 
      {
            // we need to do every col but now src (hresult) has newCols
          for (int c=0; c<newCols; c++) 
          { 
              T *out_cp = &(result[b][1][c]);
              T *outer_cp = &(hresult[b][2][c]);
                // ignore ends and skip every other row which would get tossed 
              for (int r=2; r<numRows-2; r+=2, out_cp += newCols, outer_cp += newCols * 2) 
              {      
                  T *inner_cp = outer_cp - offset * newCols;
                  sum = 0.0;
                  double *kernel_cp = kernel;
                  for (int o=-offset; o<=offset; o++, inner_cp+=newCols, kernel_cp++) 
                      sum += *inner_cp * (*kernel_cp);
                  
                    // note output row is ajusted to reflect downsampling
                  *out_cp = icdu.rint(sum);
              }
              
                // do top end pixel (r=0)
              sum = hresult[b][0][c] * kernel[offset+0];
              for (int o=1; o<=offset; o++) 
                  sum += 2.0*hresult[b][o][c]*kernel[offset+o];
              
              result[b][0][c] = icdu.rint(sum);
              
                // do bottom end pixel (r=numRows-1 or newRows-1)
              sum = hresult[b][numRows-1][c] * kernel[offset+0];
              for (int o=1; o<=offset; o++) {
                  sum += 2.0 * hresult[b][numRows-1-o][c] * kernel[offset-o];
              }
              
              result[b][newRows-1][c] = icdu.rint(sum);
          }
      }
      return result;
}


// applies a mask to an image
//
// img  : input image
// mask : input image mask, where a zero pixel is "off" and a non-zero pixel is "on"
//
// returns an image in which pixels marked as off in mask are 0, and pixels marked as 
//  on in the mask have the value of the corresponding pixel in img.
// 
template<class T>
IemPlane IcdImgProc<T>::apply_mask(const IemTPlane<T> &img, const IemTPlane<byte> &mask)
{
    IemTPlane<T> result = img.copy();

    const byte *mask_cp = mask[0];
    T *res_cp = result[0];
    for(int i=0; i<img.rows(); i++)
        for(int j=0; j<img.cols(); j++, mask_cp++, res_cp++)
            if(*mask_cp == 0)
                *res_cp = 0;

    return(result);
}



// apply_mean_filter : apply a square mean filter to each plane of an image (efficiently)
//
// inputs: image       : input image
//         filter_size : size of desired filter kernel
//
// returns: result of mean filtering
//
// crandall, 8/2002
//
// This function has been optimized in the following ways:
//   - apply separable linear filters instead of naive implementation
//   - use "running totals" to reduce number of pixels visited during 
//     each convolution
//   - use fast pixel addressing instead of Iem [][] operators
//
// Note that it is a little more efficient to perform apply_mean_filter()
// on an image than on each of the individual planes, because the intermediate
// plane result_h must then only be allocated once.
//
// Note: "Why is this not a templated method?", you may ask. For that matter,
// why aren't any of the methods in this class templated? That's because Microsoft
// Visual C++ 6.0 is far from ANSI C++ compliant, and it does not support
// templated methods defined outside the class declaration.
//
template<class T>
IemImage IcdImgProc<T>::apply_mean_filter(const IemTImage<T> &image, 
                                       int filter_size)
{
      // kernel larger than image may crash code (not yet tested)
    assert(image.rows() > filter_size && image.cols() > filter_size);

      // kernels with even dimensions may crash code (not yet tested)
    assert(filter_size % 2);

    int half_filter = filter_size/2;
        
    int rows = image.rows(), cols = image.cols();

    IemTImage<T> result(image.chans(), rows, cols);

    double sum=0;

      // do 1xfilter_size convolution
    IemTPlane<T> result_h(rows, cols);

    for(int z=0; z<image.chans(); z++) 
    {
        T *res_ptr = (result_h[0]);
        const IemTPlane<T> &in_plane = image[z];
        
        int i;
        for(i=0; i<rows; i++)
        {
            const T *row_ptr = in_plane[i];
            
              // first take care of first half_filter pixels in each row
            int j;
            for(j=0; j<half_filter; j++, res_ptr++)
            {
                  // beg_col=0 in this case
                const T *in_ptr = row_ptr;
                int end_col = j+half_filter;
                
                sum=0;
                for(int l=0; l<=end_col; l++, in_ptr++)
                    sum += (*in_ptr);           
                
                *res_ptr=T(sum/filter_size);
            }
            
            int upper_limit = cols-half_filter-1;
            
              // compute sum of first filter_size entries in row
            const T *in_ptr = row_ptr;          
            double last_sum=0;
            for(int l=0; l<filter_size; l++, in_ptr++)
                last_sum += (*in_ptr);          
            
            *res_ptr=T(last_sum/filter_size);
            res_ptr++;
            
              // now just subtract the first and add the next to last_sum,
              // as the filter moves across the image row
            const T *remove = row_ptr;
            const T *add = row_ptr + filter_size;
            for(j=half_filter+1; j<upper_limit; j++, res_ptr++, remove++, add++)
            {           
                last_sum += (*add)-(*remove);                           
                *res_ptr=T(last_sum/filter_size);
            }
            
              // and clean up by doing the last half_filter pixels of the row
            for(j=upper_limit; j<cols; j++, res_ptr++)
            {
                int beg_col = j-half_filter;
                int end_col = cols-1;
                const T *in_ptr = row_ptr + beg_col;
                
                sum=0;
                for(int l=beg_col; l<=end_col; l++, in_ptr++)
                    sum += (*in_ptr);           
                
                *res_ptr=T(sum/filter_size);
            }
        }
        
          // do filter_sizex1 convolution
        T *col_res_ptr = (result[z][0]);
        T *col_in_ptr = (result_h[0]);
        
        
        for(int j=0; j<cols; j++, col_res_ptr++, col_in_ptr++)
        {
            res_ptr = col_res_ptr;
            

            for(i=0; i<half_filter; i++, res_ptr+=cols)
            {
                int end_row = i+half_filter;
                double sum=0;
                
                const T *in_ptr = col_in_ptr;
                
                for(int k=0; k<=end_row; k++, in_ptr+=cols)
                    sum += (*in_ptr);           
                
                *res_ptr=T(sum/filter_size);
            }
            
            int upper_limit = rows-half_filter-1;
            
            
              // figure out sum of first filter_size rows of the column
            double last_sum=0;
            const T *in_ptr = col_in_ptr;
            
            for(int k=0; k<filter_size; k++, in_ptr+=cols)
                last_sum += (*in_ptr);          
            
            *res_ptr=T(last_sum/filter_size);
            res_ptr+=cols;
            
            const T *remove = col_in_ptr;
            const T *add = col_in_ptr + filter_size*cols;
            
            for(i=half_filter+1; i<upper_limit; i++, res_ptr += cols, 
                    add += cols, remove += cols)
            {
                last_sum += (*add) - (*remove);
                
                *res_ptr=T(last_sum/filter_size);
            }
            
            for(i=upper_limit; i<rows; i++, res_ptr += cols)
            {
                int beg_row = i-half_filter;
                int end_row = rows-1;
                
                double sum=0;
                
                const T *in_ptr = col_in_ptr+beg_row*cols;
                
                for(int k=beg_row; k<=end_row; k++, in_ptr+=cols)
                    sum += (*in_ptr);           
                
                *res_ptr=T(sum/filter_size);
            }
        }
    }
    
    return(IemImage(result));
}


// Compute a gradient magnitude image, given images containing x and y gradient magnitudes.
//
// Note: this method could use the pretty IEM functions to do everything in one line.
// (all it does is sqrt(x*x + y*y)).
// But it could overflow since shorts are used internally for the computations!
//
template<class T>
IemPlane IcdImgProc<T>::gradient_magnitude(const IemTPlane<T> &x, const IemTPlane<T> &y)
{
    assert(x.rows() == y.rows() && x.cols() == y.cols());

    const T *x_cp = x[0], *y_cp = y[0];

    IemTPlane<T> result(x.rows(), x.cols());
    T *res_cp = result[0];

    for(int i=0; i<x.rows(); i++)
        for(int j=0; j<x.cols(); j++, x_cp++, y_cp++, res_cp++)
            *res_cp = T( sqrt( int(*x_cp) * int(*x_cp) + int(*y_cp) * int(*y_cp) ) );

    return result;
}


template class IcdImgProc<short>;
template class IcdImgProc<float>;
