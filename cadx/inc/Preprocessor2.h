//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H


#include "Iem/Iem.h"
#include "Hit.h"
#include "Point.h"
#include "UserAgent.h"


namespace CADX_SEG {


class Preprocessor2 {

	protected:  
	
	IemTImage<short> img;
	IemTImage<unsigned char> imgMask;
	IemTImage<short> imgProcessed;
	
	IemTImage<unsigned char> imgMaskRightClf;
	IemTImage<unsigned char> imgMaskLeftClf;
	
	IemTImage<short> imgRightClfFit;
	IemTImage<short> imgLeftClfFit;
	
	IemTImage<short> imgTrendCorrected;
	
	IemTImage<short> imgStatScaled;

	// Center of clear lung fields.
	Point center;
	
	bool invert;
	short minCV;
	short maxCV;
	
	short fitDegree;
	short fitSubsample;
	
	short subsample;
	
	double aimMean;
	double aimSigma;
	short aimMinCV;
	short aimMaxCV;
	
	double gain;
	


	UserAgent userAgent;


	public:

	Preprocessor2();
	
	Preprocessor2(short minCV, short maxCV, bool invert);

	IemTImage<short> apply(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	IemTImage<short> getTrendCorrectedImage() {return imgTrendCorrected;}
	
	IemTImage<short> getStatScaledImage() {return imgStatScaled;}
	
	Point getClfCenter() {return center;}
	
	void setGain(double v) {gain = v;}
	
	void setAimMean(double v) {aimMean = v;}
 	void setAimSigma(double v) {aimSigma = v;}
 	void setAimMinCV(double v) {aimMinCV = v;}
 	void setAimMaxCV(double v) {aimMaxCV = v;}
		
	protected:

	void initialize();

	void apply();
	
	void maskClf();
	
	void fitClf();

     void subtract();
};   


} // Namespace CADX_SEG


#endif
