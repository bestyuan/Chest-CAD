//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SEGFEATURES_H
#define SEGFEATURES_H


#include "Iem/Iem.h"
#include "Roi.h" 
#include "Hit.h"
#include "UserAgent.h"


namespace CADX_SEG {


class SegFeatures {

	protected:  
	// Image to be segmented.
	IemTImage<short> img;
	IemTImage<short> imgribsup;

	IemTImage<short> bilateralimg;
	
	// Mask with non lung field values set to 0.
	IemTImage<unsigned char> imgLungMask;

	IemTImage<short> imgNoduleRibDif;
	IemTImage<short> imgNoduleRibDifribsup;
	
	IemTImage<unsigned char> imgRegionMap;
	IemTImage<unsigned char> imgRegionMapribsup;
	
	UserAgent userAgent;


	public:

	SegFeatures();
	
	void calculate(Roi& roi);

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
		
	private:
	void initialize();

	void calculateFeatures(Roi& roi);
	
	void useDefaultFeatures(Roi& roi);
	
	
};   


} // Namespace CADX_SEG


#endif
