//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H


#include "Iem/Iem.h"
#include "Roi.h" 
#include "Hit.h"
#include "UserAgent.h"


namespace CADX_SEG {


class Preprocessor {

	protected:  
	
	IemTImage<short> img;
	IemTImage<short> imgProcessed;
	
	bool invert;
	short minCV;
	short maxCV;
	
	short fitDegree;
	short fitSubsample;
	
	double aimMean;
	double aimSigma;
	short aimMinCV;
	short aimMaxCV;
	


	UserAgent userAgent;


	public:

	Preprocessor();
	
	Preprocessor(short minCV, short maxCV, bool invert);

	void apply(Roi& roi);

	IemTImage<short> apply(IemTImage<short>& img);
	

	void setUserAgent(UserAgent& agent) {userAgent = agent;}

		
	protected:

	void initialize();

	void apply();
	


};   


} // Namespace CADX_SEG


#endif
