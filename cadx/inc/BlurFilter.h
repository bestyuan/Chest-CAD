//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef BLURFILTER_H
#define BLURFILTER_H    


#include "Iem/Iem.h" 
  

namespace CADX_SEG {


class BlurFilter {   
	
	private:
	long width;
	long height;
	double* data;			
	
	public:	 
	
	BlurFilter(long _width, long _height, double sigma);	
	
	~BlurFilter();

	void initialize();
	
	IemTImage<short> filter(IemTImage<short>& img);
	
	void makeGaussianKernel(double sigma);
	
	
};





} // End Namespace 







#endif



