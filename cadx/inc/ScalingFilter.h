//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SCALINGFILTER_H
#define SCALINGFILTER_H

 
#include "Iem/Iem.h"  
#include "UserAgent.h"


namespace CADX_SEG {

class ScalingFilter
{
	long windowHalfWidth;	// Window size is (2*windowHalfWidth + 1)
	long subsample;
	double meanAim;
	double sigmaAim;
	short minCodeValue;
	short maxCodeValue;
	
	UserAgent userAgent;

	public:
	ScalingFilter();
	ScalingFilter(double meanAim, double sigmaAim, short minCodeValue, short maxCodeValue);
	
	void setMeanAim(double x) {meanAim = x;}
	void setSigmaAim(double x) {sigmaAim = x;}
	void setMinCodeValue(short x) {minCodeValue = x;}
	void setMaxCodeValue(short x) {maxCodeValue = x;}
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	void initialize();
	
	IemTImage<short> globalScaling(IemTImage<short>& img); 
	
	IemTImage<short> globalScaling(IemTImage<short>& img, long band, IemTImage<unsigned char>& imgMask);

	IemTImage<short> windowedScaling(IemTImage<short>& img, long windowHalfWidth);
	
	double scale(IemTImage<short>& img, long c, long r);
	

};


} // End namespace

#endif
