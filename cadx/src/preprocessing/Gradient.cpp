//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Gradient.h"
#include "Util.h" 
#include "Iem/IemKernelPlane.h"
#include "Histogram.h"

using namespace CADX_SEG;
 
 
Gradient::Gradient() {
	initialize();
}
	
		
IemTImage<short> Gradient::calcSobelGradient(IemTImage<short>& _img) {

	initialize();

	IemTImage<short> img = _img.copy();
	
	
	double verData[] = { 1,  2,  1,
                          0,  0,  0,
                         -1, -2, -1};

	double horData[] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1};
                           
	IemKernelPlane verKernel(3, 3, verData);
	IemKernelPlane horKernel(3, 3, horData);

	IemTImage<short> imgVerGradient = verKernel.apply(img);         
	IemTImage<short> imgHorGradient = horKernel.apply(img);

	IemTImage<short> imgGradient = IemTImage<short>(3, img.rows(), img.cols());
	imgGradient = 0;

	
	double pi = Util::PI;
	double pih = Util::PI / 2.0;	 
	double q = 180.0 / Util::PI;
	
	long rows = img.rows(); 
	long cols = img.cols();
	
	for(long r = 0; r < rows; r++) {
		for(long c = 0; c < cols; c++) {
			
			double y = imgVerGradient[0][r][c];
			double x = imgHorGradient[0][r][c];
			
			double z = sqrt(x * x + y * y);
			
			nPixels++;
			meanMag += z;
				
			imgGradient[0][r][c] = (short)z;
			 
			double theta =  atan2(y, x);
				
			if(theta >= 0.0) imgGradient[1][r][c] = (short)(q * theta);
			else  imgGradient[1][r][c] = (short)(360.0 + q * theta);
	}}
	
	meanMag /= (double)nPixels;

	return imgGradient;	
}     


IemTImage<short> Gradient::calcSobelGradient(IemTImage<short>& _img, IemTImage<unsigned char>& imgMask) {

	initialize();

	IemTImage<short> img = _img.copy();
	
	
	double verData[] = { 1,  2,  1,
                          0,  0,  0,
                         -1, -2, -1};

	double horData[] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1};
                           
	IemKernelPlane verKernel(3, 3, verData);
	IemKernelPlane horKernel(3, 3, horData);

	IemTImage<short> imgVerGradient = verKernel.apply(img);         
	IemTImage<short> imgHorGradient = horKernel.apply(img);

	IemTImage<short> imgGradient = IemTImage<short>(3, img.rows(), img.cols());
	imgGradient = 0;

	
	double pi = Util::PI;
	double pih = Util::PI / 2.0;	 
	double q = 180.0 / Util::PI;
	
	long rows = img.rows(); 
	long cols = img.cols();

	for(long r = 0; r < rows; r++) {
		for(long c = 0; c < cols; c++) {
		                               	
			if(imgMask[0][r][c] != 0) {
				double y = imgVerGradient[0][r][c];
				double x = imgHorGradient[0][r][c];
			
				double z = sqrt(x * x + y * y);

				nPixels++;
				meanMag += z;
				
				imgGradient[0][r][c] = (short)z;
			 
				double theta =  atan2(y, x);
				
				if(theta >= 0.0) imgGradient[1][r][c] = (short)(q * theta);
				else  imgGradient[1][r][c] = (short)(360.0 + q * theta);
			}
	}}
	
	meanMag /= (double)nPixels;    

	return imgGradient;	
} 


void Gradient::initialize() {
	nPixels = 0;
	meanMag = 0.0;
}


	
	

