//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "CurvatureFilter.h"   
#include "util.h"  
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"



CurvatureFilter::CurvatureFilter(long _windowHalfWidth) {

	initialize();

	windowHalfWidth = _windowHalfWidth;
}


void CurvatureFilter::initialize() {

	windowHalfWidth = 5;
	subsample = 1;
	a = (long)(windowHalfWidth * (windowHalfWidth + 1) / 3.0);
}



     
IemTImage<short> CurvatureFilter::filter(IemTImage<short>& img) {     
		
	long tpad, lpad, rpad, bpad;
	
	tpad = lpad = rpad = bpad = windowHalfWidth;
		
	IemTImage<short> imgPadded = iemPadOut(img, tpad, lpad, rpad, bpad);    
	
	IemTImage<short> imgFiltered(1, img.rows() / subsample, img.cols() / subsample); 
	
	
	for(long c = 0; c < img.cols(); c += subsample) {
		for(long r = 0; r < img.rows(); r += subsample) {  
						
			imgFiltered[0][r/subsample][c/subsample] = (short)calcCurvature(imgPadded, c, r);
	}}  
	
	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}  


double CurvatureFilter::calcCurvature(IemTImage<short>& img, long c, long r) {
	
	long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, s; 
	double curvature;
	
	long ccEnd = c + 2 * windowHalfWidth;
	long rrEnd = r + 2 * windowHalfWidth;
	
	long c0 = c + windowHalfWidth;
	long r0 = r + windowHalfWidth;
			
	for(long cc = c; cc <= ccEnd; cc++) {  	
		for(long rr = r; rr <= rrEnd; rr++) {
				
			short v = img[0][rr][cc]; 
			
			long x = cc - c0;
			long y = rr - r0;
			
			s = x * x - a; 
			sum1 += s * v;
			sum2 += s * s;
			
			s = x * y; 
			sum3 += s * v;
			sum4 += s * s;

			s = y * y - a; 
			sum5 += s * v;
			sum6 += s * s;	
		}  
	}
	
	double k4 = (double)sum1 / (double)sum2;
	double k5 = (double)sum3 / (double)sum4;
	double k6 = (double)sum5 / (double)sum6;
			
	double ixx = 2.0 * k4;
	double ixy = k5;
	double iyy = 2.0 * k6;
	
	double lamda1, lamda2;
	
	calcEigenvalues(ixx, ixy, ixy, iyy, lamda1, lamda2);
			
	curvature = (lamda1 + lamda2); // * Util::abs(lamda1) * Util::abs(lamda2); 

	
	//curvature = (lamda1 + lamda2) / pow(Util::abs(lamda1 - lamda2), 1.0);

	

	
//	double curvature = Util::abs(lamda1) * Util::abs(lamda2);
	
//	if(lamda1 > 0 && lamda2 > 0) curvature = 0.01 * Util::abs(lamda1 * lamda2) / Util::abs(lamda1 - lamda2);
//	else curvature = 0;
	
	

//	if(curvature < 0) curvature = 0;
	if(curvature > SHRT_MAX) curvature = SHRT_MAX;
	
	
	
	return curvature;
}


void CurvatureFilter::calcEigenvalues(double ixx, double ixy, double iyx, double iyy, 
double& lamda1, double& lamda2) {

	double b = -(ixx + iyy);
	double c = (ixx * iyy - ixy * iyx);
	
	double d = sqrt(b * b - 4.0 * c);

	lamda1 = (-b + d);
	lamda2 = (-b - d);
}