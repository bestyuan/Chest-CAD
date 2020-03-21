//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Preprocessor.h"
#include "Iem/IemImageIO.h"
#include "ScalingFilter.h"
#include "PolyFit.h"
#include "ImageUtil.h"
#include "CadxParm.h"

 

using namespace CADX_SEG;


Preprocessor::Preprocessor() {
	initialize();
}


Preprocessor::Preprocessor(short _minCV, short _maxCV, bool _invert) {
	initialize();

	minCV = _minCV;
	maxCV = _maxCV;
	invert = _invert;
}


void Preprocessor::initialize() {
	aimMean = 2000;
	aimSigma = 500;
	aimMinCV = 0;
	aimMaxCV = 4095;
	
	invert = true;
	minCV = 0;
	maxCV = 4095;

	fitDegree = 2;
	fitSubsample = 16;
}  


void Preprocessor::apply() {

//	userAgent.writeDebugImage(img, "Preprocessor_Input");

	if(invert) {
	     // Invert image so that high density corresponds to high code values.
		short maxCodeValue = CadxParm::getInstance().getMaxCodeValue();
		imgProcessed = ImageUtil::invert(img, minCV, maxCV);
//		userAgent.writeDebugImage(imgProcessed, "Preprocessor_Inverted");
	}
	else{imgProcessed = img;}

	// Scale the image to aim mean and std. dev.
	ScalingFilter scalingFilter(aimMean, aimSigma, aimMinCV, aimMaxCV);
	imgProcessed = scalingFilter.globalScaling(imgProcessed);
//	userAgent.writeDebugImage(imgProcessed, "Preprocessor_Scaled");

	// Fit the image to a polynomial.
	PolyFit polyFit(fitDegree);
	polyFit.calcFit(imgProcessed, fitSubsample);
	IemTImage<short> imgPolyFit = polyFit.getFitImage();
//	userAgent.writeDebugImage(imgPolyFit, "Preprocessor_PolyFit");
	
	// Subtract the fitted image.
	imgProcessed = imgProcessed - imgPolyFit;
//	userAgent.writeDebugImage(imgProcessed, "Preprocessor_TrendCorr");
}


void Preprocessor::apply(Roi& roi) {
	img = roi.getCroppedImage();
	apply();
	roi.setPreprocessedImage(imgProcessed);
}


IemTImage<short> Preprocessor::apply(IemTImage<short>& _img) {
	img = _img.copy();
	apply();
	return imgProcessed;
}

