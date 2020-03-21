//
//  (c) Copyright Eastman Kodak Company
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Preprocessor2.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemInterpolate.h"
#include "ScalingFilter.h"
#include "PolyFit.h"
#include "ImageUtil.h"
#include "RegionLabel.h"
#include "CadxParm.h"
#include "Iem/minimumFxns.h"
#include "LungMaskMaker.h"

 

using namespace CADX_SEG;

 
Preprocessor2::Preprocessor2() {
	initialize();
}


Preprocessor2::Preprocessor2(short _minCV, short _maxCV, bool _invert) {
	initialize();

	minCV = _minCV;
	maxCV = _maxCV;
	invert = _invert;
}


void Preprocessor2::initialize() {
//	aimMean = 2000;
//	aimSigma = 500;
	aimMean = 2500;
	aimSigma = 1000;
	aimMinCV = 0;
	aimMaxCV = 4095;
	
	subsample = 4;
	
	invert = true;
	minCV = 0;
	maxCV = 4095;

	fitDegree = 2;
	fitSubsample = 4;
	gain = 1.0;
}  


IemTImage<short> Preprocessor2::apply(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask) {

	img = _img;
	imgMask = _imgMask;

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
	imgStatScaled = scalingFilter.globalScaling(imgProcessed);
	imgProcessed = imgStatScaled.copy();
//	userAgent.writeDebugImage(imgStatScaled, "Preprocessor_StatScaled");

	// Make a mask for right and left CLF.
	maskClf();
	
	fitClf();
	
	subtract();

	return imgProcessed; 
}


void Preprocessor2::maskClf() { 


	IemTImage<unsigned char> imgMaskSub = iemSubsample(imgMask, subsample, subsample);

	// In the labeled image the background of the lung mask should be labeled
	// 1 and the two clear lung fields are labeled 2 and 3.
	CADX_SEG::RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgMaskSub, 0, 255, 10000);
	IemTImage<unsigned char> imgLabeled = regionLabel.getLabeledImage();
//	userAgent.writeDebugImage(imgLabeled, "Preprocessor_Labeled");
	
	// Make a mask for region labeled 2 and mask for region labeled 3.
	double meanX1, meanY1, meanX2, meanY2;
	IemTImage<unsigned char> imgMask1 = ImageUtil::codevalueToMask(imgLabeled, 2, 255, meanX1, meanY1);
	IemTImage<unsigned char> imgMask2 = ImageUtil::codevalueToMask(imgLabeled, 3, 255, meanX2, meanY2);

//	userAgent.writeDebugImage(imgMask1, "Preprocessor_Mask1");
//	userAgent.writeDebugImage(imgMask2, "Preprocessor_Mask2");

	if(meanX1 < meanX2) {
		imgMaskRightClf =  ImageUtil::mapResize(imgMask1, imgMask.cols(), imgMask.rows());
		imgMaskLeftClf = ImageUtil::mapResize(imgMask2, imgMask.cols(), imgMask.rows());
	}
	else {
		imgMaskRightClf = ImageUtil::mapResize(imgMask2, imgMask.cols(), imgMask.rows());
		imgMaskLeftClf = ImageUtil::mapResize(imgMask1, imgMask.cols(), imgMask.rows());
	}
	
	center.x = subsample * (meanX1 + meanX2) / 2.0;
	center.y = subsample * (meanY1 + meanY2) / 2.0;
	
	userAgent.getLogFile() << "\nPreprocessor: CLF center x= " << center.x << ", y= " << center.y;
	
//	userAgent.writeDebugImage(imgMaskRightClf, "Preprocessor_RightCLF");
//	userAgent.writeDebugImage(imgMaskLeftClf, "Preprocessor_LeftCLF");
}

 
void Preprocessor2::fitClf() {
	// Fit the image to a polynomial. 
		LungMaskMaker lungMaskfit;
	PolyFit polyFit(fitDegree);

	polyFit.calcFit(imgProcessed, imgMaskRightClf, fitSubsample);
	imgRightClfFit = polyFit.getMaskedFitImage();
	imgRightClfFit = iemInterpolate(imgRightClfFit, img.rows(), img.cols());

	IemTImage<unsigned char> imgfit = lungMaskfit.convertTo8Bit(imgRightClfFit);
	userAgent.writeDebugImage(imgfit, "Preprocessor_RightClfFit");

	IemTImage<unsigned char> imgMask;
	 
	polyFit.calcFit(imgProcessed, imgMaskLeftClf, fitSubsample);
	imgLeftClfFit = polyFit.getMaskedFitImage();
	imgLeftClfFit = iemInterpolate(imgLeftClfFit, img.rows(), img.cols());

	IemTImage<unsigned char> imgfitleft = lungMaskfit.convertTo8Bit(imgLeftClfFit);
	userAgent.writeDebugImage(imgfitleft, "Preprocessor_LeftClfFit");
}


void Preprocessor2::subtract() {
	short dif, min = SHRT_MAX;
	LungMaskMaker lungMask;

	imgTrendCorrected = IemTImage<short>(1, img.rows(), img.cols());

	long cols = img.cols();
	long rows = img.rows(); 

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(imgMaskLeftClf[0][y][x] == 255) {
				dif = gain * (imgProcessed[0][y][x] - imgLeftClfFit[0][y][x]);
			     if(dif < min) min = dif;
				imgTrendCorrected[0][y][x] = dif;
			}
			if(imgMaskRightClf[0][y][x] == 255) {
				dif = gain * (imgProcessed[0][y][x] - imgRightClfFit[0][y][x]);
				if(dif < min) min = dif;
				imgTrendCorrected[0][y][x] = dif;
			}
	}}

	// Outside the CLF set image to min value.
	for(x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(imgMaskLeftClf[0][y][x] != 255 && imgMaskRightClf[0][y][x] != 255) {
			      imgTrendCorrected[0][y][x] = min;
			}
	}}
 
	IemTImage<unsigned char> imgCorrect = lungMask.convertTo8Bit(imgTrendCorrected);
	userAgent.writeDebugImage(imgCorrect, "imgTrendCorrected");
	
}

 



