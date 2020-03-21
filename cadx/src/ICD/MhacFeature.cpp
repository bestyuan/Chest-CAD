//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "MhacFeature.h"
#include "Gradient.h"
#include "BlurFilter.h"
#include "Histogram.h"
#include "ImageUtil.h"
#include "Util.h"   
#include "CadxParm.h" 
#include "Iem/IemImageIO.h" 
#include "Iem/maximumFxns.h"
#include "Iem/IemMorphological.h"


using namespace CADX_SEG; 




MhacFeature::MhacFeature(double _radius, long _nBins, short _aggregation) {
 	initialize();
	nBins = _nBins;
	aggregation = _aggregation;
	allocate();
	setInnerRadius(_radius);
}


MhacFeature::~MhacFeature() {
	free();
}


void MhacFeature::free() {
	if(gradDotProduct != NULL) delete[] gradDotProduct;
	if(nGradPixels != NULL) delete[] nGradPixels;

	if(innerCodeValue != NULL) delete[] innerCodeValue;
	if(nInnerPixels != NULL) delete[] nInnerPixels;

	if(outterCodeValue != NULL) delete[] outterCodeValue;
	if(nOutterPixels != NULL) delete[] nOutterPixels;
}


void MhacFeature::setInnerRadius(double r) {
	innerRadius = r;
	outterRadius = 1.5 * innerRadius;
	outterRadius2 = outterRadius * outterRadius;
}

void MhacFeature::allocate() {
     dTheta = 360.1 / (double)nBins;
	innerCodeValue = new double[nBins];
	nInnerPixels = new long[nBins];
	gradDotProduct = new double[nBins];
	nGradPixels = new long[nBins];
	outterCodeValue = new double[nBins];
	nOutterPixels = new long[nBins];
}


void MhacFeature::calculate(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask) {

	img = iemAggregateDown(_img, aggregation, 0);
//	userAgent.writeDebugImage(img, "Mhac_Input");

	imgMask = iemAggregateDown(_imgMask, aggregation, 0);
//	userAgent.writeDebugImage(imgMask, "Mhac_Mask");

	imgGradFeature = IemTImage<short>(1, img.rows(), img.cols());
	imgGradFeature = 0;

	imgCvFeature = IemTImage<short>(1, img.rows(), img.cols());
	imgCvFeature = 0;
	
	imgScore = IemTImage<short>(1, img.rows(), img.cols());
	imgScore = 0;

	imgGradPixels = IemTImage<short>(1, img.rows(), img.cols());
	imgGradPixels = 0; 
	
	BlurFilter blurFilter(7, 7, 3);
	img = blurFilter.filter(img);
//	userAgent.writeDebugImage(img, "Mhac_Blurred");

	// Erode mask to remove large gradient at CLF boundary.
	imgMask = iemErode(imgMask, 15, 15);

	Gradient gradient;
	imgGradient = gradient.calcSobelGradient(img, imgMask);
	userAgent.writeDebugImage(imgGradient, "Mhac_Gradient");
	
	Histogram histogram(0, iemMaximum(imgGradient));
	histogram.build(imgGradient, imgMask, 0);
	histogram.write(userAgent.getLogFile());
	minGradientMag = histogram.getLowerPenetration(0.50);
	maxGradientMag = SHRT_MAX; // histogram.getUpperPenetration(0.01);
	userAgent.getLogFile() << "\nminGradientMag= " << minGradientMag << ", maxGradientMag= " << maxGradientMag;

	if(userAgent.getDebug()) {
		IemTImage<short> imgGradientThresh = ImageUtil::applyDoubleThreshold(imgGradient, 0, minGradientMag, maxGradientMag);
//		userAgent.writeDebugImage(imgGradientThresh, "Mhac_Gradient_Thresh");	                         	
	}

 	regionStatistics();

 	userAgent.writeDebugImage(imgGradFeature, "Mhac_GradFeature");
 	userAgent.writeDebugImage(imgCvFeature, "Mhac_CvFeature");
	userAgent.writeDebugImage(imgScore, "Mhac_Score");
	userAgent.writeDebugImage(imgGradPixels, "Mhac_GradPixels");
}

 
void MhacFeature::initialize() { 
	verbose = false;

	minGradientMag = 0;
	maxGradientMag = SHRT_MAX;
	aggregation = 2;

	nBins = 16;
	dTheta = 360.1 / (double)nBins;

	innerRadius = 8;
	innerCodeValue = NULL;
	nInnerPixels = NULL;

	gradDotProduct = NULL;
	nGradPixels = NULL;

	outterRadius = 1.5 * innerRadius;
	outterRadius2 = outterRadius * outterRadius;
	outterCodeValue = NULL;
	nOutterPixels = NULL;
}


void MhacFeature::regionStatistics() {

	long rows = img.rows() - outterRadius;
	long cols = img.cols() - outterRadius;

	for(long originRow = outterRadius; originRow < rows; originRow++) {
		for(long originCol = outterRadius; originCol < cols; originCol++) {

			if(imgMask[0][originRow][originCol] != 0) {
			          
				long totalGradPixels = pointStatistics(originCol, originRow);

				double cvFeature = getCvFeature();
				double gradFeature = getGradFeature();

				imgCvFeature[0][originRow][originCol] = cvFeature;
				imgGradFeature[0][originRow][originCol] = gradFeature;
				imgGradPixels[0][originRow][originCol] = totalGradPixels;
				
				double score = 0.0;
				
				if(img[0][originRow][originCol] <= 0) {score = 0.0;}
				else if(cvFeature <= 0) {score = 0.0;}
				else if(gradFeature <= 0) {score = 0.0;}
				else {score = gradFeature;}

				if(score > SHRT_MAX) score = SHRT_MAX;

				imgScore[0][originRow][originCol] = score;

			} // Mask != 0

	}} // Next origin 
  
}


long MhacFeature::pointStatistics(long originCol, long originRow) {

	long totalGradPixels = 0;

	if(verbose) {userAgent.getLogFile() << "\nOrigin col= " << originCol
	 << ", row= " << originRow << ", cv= " << img[0][originRow][originCol];}


	// Initialize buffers.
	for(int i = 0; i < nBins; i++) {
		innerCodeValue[i] = 0.0;
		nInnerPixels[i] = 0;
		gradDotProduct[i] = 0.0;
		nGradPixels[i] = 0;
		outterCodeValue[i] = 0.0;
		nOutterPixels[i] = 0;
	}

	for(long dr = -outterRadius; dr <= outterRadius; dr++) {
	long dcMax = sqrt(outterRadius2 - dr * dr); 
		for(long dc = -dcMax; dc <= dcMax; dc++) {

			long r = originRow + dr;
			long c = originCol + dc;
				          
			if(imgMask[0][r][c] == 0) continue;

			// Calculate angle between pixel and origin.
			double dx = (originCol - c);
			double dy = -(originRow - r);
			double theta0 =  Util::RAD_TO_DEG * atan2(dy, dx);
			if(theta0 < 0.0) theta0 += 360.0;
			long index = theta0 / dTheta;

			long radius = sqrt(dr*dr + dc*dc);

			// Within inner radius
			if(radius <= innerRadius) {
			     short cv = img[0][r][c];
				innerCodeValue[index] += cv;
				nInnerPixels[index]++;

				double mag = imgGradient[0][r][c];
				
				if(verbose) {userAgent.getLogFile() << "\nPixel at col= " << c
				 << ", row= " << r << ", index= " << index << ", cv= " << img[0][r][c]
				 << ", mag= " << mag;}

				if(mag >= minGradientMag && mag <= maxGradientMag) {

					double theta = imgGradient[1][r][c];
					double delta = Util::abs(theta - theta0);
					double cosine = cos(Util::DEG_TO_RAD * delta);

					if(verbose) {userAgent.getLogFile() << ", theta0= " << theta0 
					 << ", theta= " << theta << ", cosine= " << cosine;}

					gradDotProduct[index] += cosine;
				//	gradDotProduct[index] += mag * cosine;
					nGradPixels[index]++;
					totalGradPixels++;
				}
			}
			// Within outter radius
			else {
				outterCodeValue[index] += img[0][r][c];
				nOutterPixels[index]++;
			}

	}} // Circular region around origin

	return totalGradPixels;
}


double MhacFeature::getGradFeature() {

	double sum1 = 0.0, sum2 = 0.0;

	for(int i = 0; i < nBins; i++) {
	//	if(nGradPixels[i] == 0) {gradDotProduct[i] = 0.0;}
	//	else {gradDotProduct[i] = gradDotProduct[i] / (double)nGradPixels[i];}
		
		if(nInnerPixels[i] == 0) {gradDotProduct[i] = 0.0;}
		else {gradDotProduct[i] = gradDotProduct[i] / (double)nInnerPixels[i];}

		sum1 += gradDotProduct[i];
		sum2 += gradDotProduct[i] * gradDotProduct[i];
	} 

	double gradFeatureMean = sum1 / (double)nBins;
	double gradFeatureSigma = sqrt(sum2 / (double)nBins - gradFeatureMean * gradFeatureMean);

	if(gradFeatureSigma == 0.0) gradFeatureSigma = 0.001;
	
	if(verbose) {userAgent.getLogFile() << "\ngradFeatureMean= " << gradFeatureMean
	 << ", gradFeatureSigma= " << gradFeatureSigma;}

	return 10.0 * gradFeatureMean / gradFeatureSigma;
}


double MhacFeature::getCvFeature() {

	double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
	long nPositiveBins = 0;

	for(int i = 0; i < nBins; i++) {

		if(nInnerPixels[i] > 0) {innerCodeValue[i] = innerCodeValue[i] / (double)nInnerPixels[i];}
		else {innerCodeValue[i] = 0;}
		
		sum3 += innerCodeValue[i];
		
		if(innerCodeValue[i] > 0) nPositiveBins++;

		if(nOutterPixels[i] > 0) {outterCodeValue[i] = outterCodeValue[i] / (double)nOutterPixels[i];}
		else {outterCodeValue[i] = 0;}

		double deltaAvgCodeValue = innerCodeValue[i] - outterCodeValue[i];

		sum1 += deltaAvgCodeValue;
		sum2 += deltaAvgCodeValue * deltaAvgCodeValue;
	}

	double deltaCvFeatureMean = sum1 / (double)nBins;
	double deltaCvFeatureSigma = sqrt(sum2 / (double)nBins - deltaCvFeatureMean * deltaCvFeatureMean);
	double cvFeatureMean = sum3 / (double)nBins;
	
//	if(cvFeatureMean <= 0) return 0.0;

	if(deltaCvFeatureSigma == 0.0) deltaCvFeatureSigma = 0.001;
	
	if(verbose) {userAgent.getLogFile() << "\ndeltaCvFeatureMean= " << deltaCvFeatureMean
	 << ", deltaCvFeatureSigma= " << deltaCvFeatureSigma;}
	 
	double fractPositiveBins = (double)nPositiveBins / (double)nBins;

	return 10.0 * (deltaCvFeatureMean / deltaCvFeatureSigma);
}

  
void MhacFeature::write(ostream& s, long originCol, long originRow) {

	verbose = true;

	s << "\n\n***origin= " << originCol << ", " << originRow;

	pointStatistics(originCol, originRow); 

	double gradFeature = getGradFeature();  

	
	s << "\nGradient Feature= " << gradFeature;
					 
	for(int i = 0; i < nBins; i++) {
     	s << "\ni= " << i << ", dotProduct= " << gradDotProduct[i] << ", nPixels= " <<  nGradPixels[i];
	}

	double cvFeature = getCvFeature(); 
	
	s << "\nCV Feature= " << cvFeature;
					 
	for(i = 0; i < nBins; i++) {
     	s << "\ni= " << i << ", innerCV= " << innerCodeValue[i] << ", nPixels= " <<  nInnerPixels[i]
     	 << ", outterCV= " << outterCodeValue[i] << ", nPixels= " <<  nOutterPixels[i]
		 << ", deltaCV= " << innerCodeValue[i] - outterCodeValue[i]
		 << ", gradient pixels= " << nGradPixels[i];
	} 

}





