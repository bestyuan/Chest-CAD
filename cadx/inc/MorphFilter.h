//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MORPHFILTER_H
#define MORPHFILTER_H

 
#include "Iem/Iem.h" 
#include "UserAgent.h"


namespace CADX_SEG {



class MorphFilter {

	public:
	static const short ERODE;
	static const short DILATE;
	static const short OPEN;
	static const short CLOSE;

	protected:
	IemTImage<short> imgTemplate;
	IemTImage<short> imgAggregated;
	IemTImage<short> imgPadded;
	
	IemTImage<unsigned char> imgMaskAggregated;
	IemTImage<unsigned char> imgMaskPadded;

	short aggregation;
	
	UserAgent userAgent;


	public:    
	MorphFilter(IemTImage<short>& imgTemplate, short aggregation = 0, double templateHeightScale = 1.0);
	
	IemTImage<short> filter(IemTImage<short>& img, short type);
	IemTImage<short> filter(IemTImage<short>& img, short type, IemTImage<unsigned char>& imgMask);

	IemTImage<short> getAggregatedImage() {return imgAggregated;}
	
	IemTImage<short> getPaddedImage() {return imgPadded;}
	
	IemTImage<short> getTemplate() {return imgTemplate;}
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}


	protected:
	IemTImage<short> basicFilter(IemTImage<short>& img, short type);      
	IemTImage<short> basicFilter(IemTImage<short>& img, short type, IemTImage<unsigned char>& imgMask);

	short dilatePixel(IemTImage<short>& img, long c, long r);
	short erodePixel(IemTImage<short>& img, long c, long r);
	
	IemTImage<short> MorphFilter::dilation(IemTImage<short>& img);
	
	IemTImage<short> MorphFilter::erosion(IemTImage<short>& img);

	IemTImage<short> MorphFilter::dilation(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);
	
	IemTImage<short> MorphFilter::erosion(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);
	
	void initialize();
};




} // End namespace




#endif
