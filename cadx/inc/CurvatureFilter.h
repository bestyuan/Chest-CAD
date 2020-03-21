//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CURVATUREFILTER_H
#define CURVATUREFILTER_H

 
#include "Iem/Iem.h"  



class CurvatureFilter
{
	long windowHalfWidth;	// Window size is (2*windowHalfWidth + 1)
	long subsample;
	long a;			// A useful constant based on windowHalfWidth

	public:    
	CurvatureFilter(long _windowHalfWidth);
	
	void initialize();
		
	IemTImage<short> filter(IemTImage<short>& img);         
	
	double calcCurvature(IemTImage<short>& img, long c, long r);
	
	void calcEigenvalues(double ixx, double ixy, double iyx, double iyy, 
	 double& lamda1, double& lamda2);
};




#endif