//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MHACFEATURE_H
#define MHACFEATURE_H



#include "UserAgent.h"
#include "Iem/Iem.h" 


namespace CADX_SEG {  



class MhacFeature {

	private:   	
	IemTImage<short> img;
	IemTImage<unsigned char> imgMask;
	IemTImage<short> imgGradient;

	IemTImage<short> imgGradFeature;
	IemTImage<short> imgCvFeature;
	IemTImage<short> imgGradPixels;
	
	IemTImage<short> imgScore;
	
	bool verbose;
	
	long nfeatures;
	long nBins;
	double dTheta;

	long innerRadius;
	double* innerCodeValue;
	long* nInnerPixels;
	double* gradDotProduct;
	long* nGradPixels;

	long outterRadius;
	double* outterCodeValue;
	long* nOutterPixels;
	long outterRadius2;

	short aggregation;

	short minGradientMag;
	short maxGradientMag;

	UserAgent userAgent;


	public:

	MhacFeature() {initialize();}

	MhacFeature(double radius, long nBins, short aggregation);

	~MhacFeature();

	void calculate(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);

	IemTImage<short> getGradientImage() {return imgGradient;}
	
	void setRadius(double r) {
		setInnerRadius(r);
	}

	void setNBins(long n) {nBins = n; allocate();}
	void setAggregation(long n) {aggregation = n;}
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	void setMinGradientMag(short x) {minGradientMag = x;}
	void setMaxGradientMag(short x) {maxGradientMag = x;}

	long pointStatistics(long originCol, long originRow);

	void regionStatistics();
	
	void write(ostream& s, long originCol, long originRow);

	void setInnerRadius(double r); 

	IemTImage<short> getImage() {return img;}
	IemTImage<short> getGradFeatureMap() {return imgGradFeature;}
	IemTImage<short> getGradPixelsMap() {return imgGradPixels;}
	IemTImage<short> getCvFeatureMap() {return imgCvFeature;}
	IemTImage<short> getScoreMap() {return imgScore;}
	
	private:

	void initialize();
	
	void allocate();
	
	void free();

	double getGradFeature();
	
	double getCvFeature();

};  


} // End namepace


#endif 

