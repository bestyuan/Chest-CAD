//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MATCHEDFILTER_H
#define MATCHEDFILTER_H

 
#include "Iem/Iem.h"  



class MatchedFilter
{
	IemTImage<short> imgTemplate;
	double templateN;
	double templateMean; 
	double templateSigma;
	short aggregation;
	
	// The aggregated image.
	IemTImage<short> imgAggregated;
	
	// The padded image.
	IemTImage<short> imgPadded;

	public:    
	MatchedFilter(IemTImage<short>& imgTemplate, short aggregation);
	
	void initialize();

	void calcTemplateStats();
	
	IemTImage<short> getAggregatedImage() {return imgAggregated;}
	
	IemTImage<short> getPaddedImage() {return imgPadded;}
		
	IemTImage<short> filter(IemTImage<short>& img);         
	
	double filter(IemTImage<short>& img, long c, long r);
};




#endif
