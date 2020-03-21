//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "BlurFilter.h" 
#include "Util.h" 
#include "Iem/IemKernelPlane.h"   


using namespace CADX_SEG;  



BlurFilter::BlurFilter(long _width, long _height, double sigma) {

	initialize();

	width = _width;
	height = _height;
	
	long size = width * height;
	
	data = new double[size];

	makeGaussianKernel(sigma);
}


BlurFilter::~BlurFilter() {

	if(data != NULL) delete[] data;
}


void BlurFilter::initialize() {

	width = 0;
	height = 0;
	data = NULL;		
}	
	
	
IemTImage<short>  BlurFilter::filter(IemTImage<short>& img) {
                                                          
	IemKernelPlane kernel(width, height, data);	 
	
	return kernel.apply(img);         
}    


void BlurFilter::makeGaussianKernel(double sigma) {
	
	long x0 = width / 2; 
	long y0 = height / 2; 
	
	double sigma2 = Util::square(sigma);
	double sum = 0;
			
	for(long x = 0; x < width; x++) {
		for(long y = 0; y < height; y++) {
		
			double d2 = 0.5 * (Util::square(x - x0) + Util::square(y - y0)) / sigma2;
		
			long i = x + y * width;

			data[i] = exp(-d2);
			sum += data[i];
	}}	
	
	long size = width * height;
	
	for(long i = 0; i < size; i++) data[i] /= sum;
}


