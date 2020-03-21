//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef OVERLAPFEATURE_H
#define OVERLAPFEATURE_H



#include "Feature.h"   
#include "Roi.h" 
#include "UserAgent.h"
#include "Boundary.h"
#include "Iem/Iem.h" 


namespace CADX_SEG {  



class OverlapFeature : public Feature {

	private:
	IemTImage<short> img;    
	IemTImage<unsigned char> imgMask;
	
	IemTImage<unsigned char> imgEdgeMap;
	
	IemTImage<unsigned char> imgOverlapMap;
	
	Boundary boundary;
	
	double overlap;
	
	long lookAround;


	short regionLabel;
	
	UserAgent userAgent;


	public:     
	
	OverlapFeature();
	
	OverlapFeature(char* labelRoot);

	OverlapFeature(char* labelRoot, Roi& roi);
	
	OverlapFeature(char* labelRoot, IemTImage<short>& img, Boundary& boundary, IemTImage<unsigned char>& imgMask);


	void calculate();


	double getScore();
	



	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	private:          


	void initialize();


	bool isOverlapping(long col, long row);




};


} // End namepace


#endif 

