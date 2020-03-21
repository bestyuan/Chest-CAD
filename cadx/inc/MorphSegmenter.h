//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MORPHSEGMENTER_H
#define MORPHSEGMENTER_H


#include "Iem/Iem.h"
#include "Roi.h" 
#include "Hit.h"
#include "UserAgent.h"


namespace CADX_SEG {


class MorphSegmenter {

	protected:  
	// Image to be segmented.
	IemTImage<short> img;
	
	// Mask with non lung field values set to 0.
	IemTImage<unsigned char> imgLungMask;

	IemTImage<short> imgNoduleRibDif;
	
	IemTImage<unsigned char> imgRegionMap;
	
	UserAgent userAgent;


	public:

	MorphSegmenter();
	
	void segment(Roi& roi);
	
	IemTImage<short> excludeNonLung(IemTImage<short>& img, IemTImage<unsigned char>& mask);
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}
		
	private:
	void initialize();
	
	IemTImage<short> ribFilter(IemTImage<short>& img);
	
	void calculateFeatures(Roi& roi);
	
	void useDefaultFeatures(Roi& roi);
	
	
};   


} // Namespace CADX_SEG


#endif
