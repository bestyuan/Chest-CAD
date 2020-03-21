//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "EdgeDetector.h" 
#include "Util.h" 
#include "Iem/IemKernelPlane.h"
#include "Histogram.h"

using namespace CADX_SEG;


EdgeDetector::EdgeDetector() {	
	
	upperHistPen = 0.4;	
	
}
	
		
IemTImage<short> EdgeDetector::calcSobelGradient(IemTImage<short>& _img) {

	IemTImage<short> img = _img.copy();
	
	
	double horData[] = { 1,  2,  1,
                          0,  0,  0,
                         -1, -2, -1};

	IemKernelPlane horKernel(3, 3, horData); 
	
	
	double verData[] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1};
                           
	IemKernelPlane verKernel(3, 3, verData);	 
	
	
	IemTImage<short> imgVerEdge = verKernel.apply(img);         
	
	IemTImage<short> imgHorEdge = horKernel.apply(img);   
	
					
	IemTImage<short> imgEdge = IemTImage<short>(3, img.rows(), img.cols()); 
	imgEdge = 0;

	
	double pi = Util::PI;
	double pih = Util::PI / 2.0;	 
	double q = 180.0 / Util::PI;
	
	long rows = img.rows();
	long cols = img.cols();
	
	for(long r = 0; r < rows; r++) {
		for(long c = 0; c < cols; c++) {
			
			double y = imgVerEdge[0][r][c];
			double x = imgHorEdge[0][r][c];
			
			double z = sqrt(x * x + y * y);
				
			imgEdge[0][r][c] = (short)z;
			
			double theta =  atan2(x, y);
				
			if(theta >= 0.0) imgEdge[1][r][c] = (short)(q * theta);
			else  imgEdge[1][r][c] = (short)(360.0 + q * theta);
	}}

	return imgEdge;	
}     



	
	

