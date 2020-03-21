//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "IcdFeatures.h"
#include "PeakDetector.h"
#include "MorphPreprocessor.h"
#include "Iem/IemAggregate.h"
#include "CadxParm.h"



using namespace CADX_SEG;
 


IcdFeatures::IcdFeatures() {
	initialize();
}


void IcdFeatures::initialize() {
	minInterPeakDistance = 10;
	nMhacAggregations = 2;
	nMhacRadius = 8;
	nMhacBins = 8;
}


void IcdFeatures::calculate(IemTImage<short>& _img, IemTImage<short>& _imgDif,
 IemTImage<unsigned char>& _imgMask) {

	img = _img;
	imgDif = _imgDif;
	imgMask = _imgMask;
	
	char baseTag[1024], tag[1024];
	strcpy(baseTag, userAgent.getTag());

     sprintf(tag, "%s_Dif", baseTag);
	userAgent.setTag(tag);
	mhacFeature.setRadius(nMhacRadius);
	mhacFeature.setNBins(nMhacBins);
	mhacFeature.setAggregation(nMhacAggregations);
//	mhacFeature.setMinGradientMag(10);
//	mhacFeature.setMaxGradientMag(100);
//	mhacFeature.setMinGradientMag(40);
//	mhacFeature.setMaxGradientMag(200);
//	mhacFeature.setMinGradientMag(200);
//	mhacFeature.setMaxGradientMag(2000);
//	mhacFeature.setMinGradientMag(50);
//	mhacFeature.setMaxGradientMag(500);
	mhacFeature.setUserAgent(userAgent);
	mhacFeature.calculate(imgDif, imgMask);

	sprintf(tag, "%s_Den", baseTag);
	userAgent.setTag(tag);
	mhacFeature2.setRadius(nMhacRadius);
	mhacFeature2.setNBins(nMhacBins);
	mhacFeature2.setAggregation(nMhacAggregations);
//	mhacFeature2.setMinGradientMag(50);
//	mhacFeature2.setMaxGradientMag(300);
//	mhacFeature2.setMinGradientMag(100);
//	mhacFeature2.setMaxGradientMag(600);
//	mhacFeature2.setMinGradientMag(100);
//	mhacFeature2.setMaxGradientMag(1000);
	mhacFeature2.setUserAgent(userAgent);
	mhacFeature2.calculate(img, imgMask);

	IemTImage<unsigned char> imgMaskLowRes = iemAggregateDown(imgMask, nMhacAggregations, 0);

	sprintf(tag, "%s", baseTag);
	userAgent.setTag(tag);

	PeakDetector peakDetector;
	peakDetector.setUserAgent(userAgent);
	peakDetector.setMinInterPeakDistance(minInterPeakDistance);
	peakList = peakDetector.detect(mhacFeature.getScoreMap(), imgMaskLowRes);
//	if(userAgent.getDebug()) userAgent.writeDebugImage(peakDetector.getAnnotatedImage(), "IcdFeatures_Peaks");
    userAgent.writeDebugImage(peakDetector.getAnnotatedImage(), "IcdFeatures_Peaks");
	double rescale = pow(2.0, nMhacAggregations);

	for(list<Point>::iterator iter = peakList.begin(); iter != peakList.end(); iter++) {

     	long x = (*iter).x; 
     	long y = (*iter).y;
     	
		userAgent.getLogFile() << "\nPeak: col= " << x << ", row= " << y 
		 << ", img= " << mhacFeature.getImage()[0][y][x]
		 << ", grad= " << mhacFeature.getGradFeatureMap()[0][y][x]
		 << ", cv= " << mhacFeature.getCvFeatureMap()[0][y][x]
		 << ", pixels= " << mhacFeature.getGradPixelsMap()[0][y][x]
		 << ", score= " << mhacFeature.getScoreMap()[0][y][x];

 		(*iter).x *= rescale; (*iter).y *= rescale;
	}
}


void IcdFeatures::addToRoi(Roi& roi) {
	
	double rescale = pow(2.0, nMhacAggregations);
	
	long x = roi.getAbsCandidateCol() / rescale;
	long y = roi.getAbsCandidateRow() / rescale;

	roi.addFeature("Icd.Dif.Cv", mhacFeature.getCvFeatureMap()[0][y][x]);
	roi.addFeature("Icd.Dif.Grad", mhacFeature.getGradFeatureMap()[0][y][x]);
	roi.addFeature("Icd.Den.Cv", mhacFeature2.getCvFeatureMap()[0][y][x]);
	roi.addFeature("Icd.Den.Grad", mhacFeature2.getGradFeatureMap()[0][y][x]);
}

void IcdFeatures::addToRoiribsup(Roi& roi) {
	
	double rescale = pow(2.0, nMhacAggregations);
	
	long x = roi.getAbsCandidateCol() / rescale;
	long y = roi.getAbsCandidateRow() / rescale;

	roi.addFeature("Icdrib.Dif.Cv", mhacFeature.getCvFeatureMap()[0][y][x]);
	roi.addFeature("Icdrib.Dif.Grad", mhacFeature.getGradFeatureMap()[0][y][x]);
	roi.addFeature("Icdrib.Den.Cv", mhacFeature2.getCvFeatureMap()[0][y][x]);
	roi.addFeature("Icdrib.Den.Grad", mhacFeature2.getGradFeatureMap()[0][y][x]);
}

   
void IcdFeatures::writeFeatureInfo(ostream& s, long col, long row) {

	double rescale = pow(2.0, nMhacAggregations);
	
 	col /= rescale; 
	row /= rescale;

	s << "\n\n Dif image";
	mhacFeature.write(s, col, row);
	
	s << "\n\n Density image";
	mhacFeature2.write(s, col, row);
}
 











