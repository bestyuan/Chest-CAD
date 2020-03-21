//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "PolyFit.h"
#include "Iem/IemImageIO.h"

using namespace CADX_SEG;

PolyFit::PolyFit() {
	initialize();
}


PolyFit::PolyFit(long order) {
	initialize();
	polynomial.create(order);
}


void PolyFit::initialize(long order) {
	initialize();
	polynomial.create(order);
}


PolyFit::~PolyFit() {

}


void PolyFit::initialize() {
	sample = 1;
	nSamples = 0;
	xMean = 0;
	yMean = 0;
	xMin = 0;
	yMin = 0;
	xMax = 0;
	yMax = 0;
	fitError = 0.0;
	maskValue = 255;
}


void PolyFit::calcFit(IemTImage<short>& _img, long _sample) {
	img = _img;

	imgMask = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgMask = maskValue;
	
	sample = _sample;
	
	nSamples = (img.rows() * img.cols()) / (sample * sample);
//	cout <<  "\nnSamples " << nSamples;

	long nTerms = polynomial.getNTerms();

	matVandermonde = IemMatrix(nSamples, nTerms);
	matList = IemMatrix(nSamples, 1);

	xMin = 0;
	yMin = 0;
	
	xMax = img.cols() - 1;
	yMax = img.rows() - 1;
	
	xMean = (xMax + xMin) / 2;
	yMean = (yMax + yMin) / 2;

	buildMatrix();
	
//	ofstream out("h:/schildkr/polyfit.dat");
//	dump(out);

	solve();
}


void PolyFit::calcFit(IemTImage<short>& _img, long _xMin, long _xMax,
 long _yMin, long _yMax, long _xMean, long _yMean, long _sample) {

	img = _img;
	
	imgMask = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgMask = maskValue;
	
	sample = _sample;
	
	xMin = _xMin;
	yMin = _yMin;

	xMax = _xMax;
	yMax = _yMax;
	
	xMean = _xMean;
	yMean = _yMean;

	nSamples = ((xMax - xMin) * (yMax - yMin)) / (sample * sample);
	
	long nTerms = polynomial.getNTerms();

	matVandermonde = IemMatrix(nSamples, nTerms);
	matList = IemMatrix(nSamples, 1);

	buildMatrix();

	solve();
}


void PolyFit::calcFit(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask, long _sample) {

	img = _img;
	imgMask = _imgMask;
	
	sample = _sample;
	
	examineMask();

	long nTerms = polynomial.getNTerms();

	matVandermonde = IemMatrix(nSamples, nTerms);
	matList = IemMatrix(nSamples, 1);

	buildMatrix();

	solve();
}


void PolyFit::calcFit(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask,
 long _xMean, long _yMean, long _sample) {

	img = _img;
	imgMask = _imgMask;
	
	sample = _sample;

	examineMask();
	
	xMean = _xMean;
	yMean = _yMean;

	long nTerms = polynomial.getNTerms();

	matVandermonde = IemMatrix(nSamples, nTerms);
	matList = IemMatrix(nSamples, 1);

	buildMatrix();

	solve();
}


void PolyFit::buildMatrix() {

	long index = 0;
	  
	long nTerms = polynomial.getNTerms();
	polynomial.setCenter(xMean, yMean);

	for(long x = xMin ; x <= xMax; x += sample) {
		for(long y = yMin; y <= yMax; y += sample) {
	  		                                            	
			if(index >= nSamples) return;
				
			if(imgMask[0][y][x] == maskValue) {
				polynomial.setPosition(x, y);
				
				double *term = polynomial.getTerms();

				for(long i = 0; i < nTerms; i++) matVandermonde[index][i] = term[i];

				matList[index][0] = img[0][y][x];

				index++;
			}
	}}

}


void PolyFit::examineMask() {

	nSamples = 0;
	xMin = yMin = LONG_MAX;
	xMax = yMax = LONG_MIN;
	
	long xSum = 0, ySum = 0;

	for(long x = 0 ; x < imgMask.cols(); x += sample) {
		for(long y = 0; y < imgMask.rows(); y += sample) {

			if(imgMask[0][y][x] == maskValue) {			                          	
			     if(x < xMin) xMin = x;
			     if(y < yMin) yMin = y;
			     if(x > xMax) xMax = x;
			     if(y > yMax) yMax = y;
			     
			     xSum += x;
			     ySum += y;

				nSamples++;
			}
	}}

	if(nSamples != 0) {
		xMean = xSum / nSamples;
		yMean = ySum / nSamples;
	}
}


void PolyFit::solve() {
	IemMatrix matTransposeVander = matrixTranspose(matVandermonde);
	IemMatrix	matVanderTVander = matrixMultiply(matTransposeVander, matVandermonde);
	IemMatrix	matInverseVanderTVander = matVanderTVander.inverse();
	IemMatrix matPseudoInverse = matrixMultiply(matInverseVanderTVander, matTransposeVander);
	matCoef = matrixMultiply(matPseudoInverse, matList);

	IemMatrix	matEstimate = matrixMultiply(matVandermonde, matCoef);
	fitError = getError(matEstimate, matList);
	
	long nTerms = polynomial.getNTerms();
	for(long i = 0; i < nTerms; i++) polynomial.setCoef(i, matCoef[i][0]);
}
/*
IemTImage<short> PolyFit::getFit() {

	IemTImage<short> imgFit(1, img.rows(), img.cols());
	imgFit = 0;

	for(long x = xMin; x <= xMax; x++) {
	  	for(long y = yMin; y <= yMax; y++) {

	  	     if(imgMask[0][y][x] != 0) {
	  			long xp = xMean - x;
	  			long yp = y - yMean;

				imgFit[0][y][x] = (short)polynomial->getValue(xp, yp);
			}
	}}

	return imgFit;
}
*/

IemTImage<short> PolyFit::getFitImage() {

	IemTImage<short> imgFit(1, img.rows(), img.cols());

	for(long x = 0; x < img.cols(); x++) {
	  	for(long y = 0; y < img.rows(); y++) {

	  		polynomial.setPosition(x, y);
	  		
	  		double v = polynomial.getValue();
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;
			imgFit[0][y][x] = (short)v;
	}}

	return imgFit;
}


IemTImage<short> PolyFit::getMaskedFitImage() {

	IemTImage<short> imgFit(1, img.rows(), img.cols());
	imgFit = 0;

	for(long x = 0; x < img.cols(); x++) {
	  	for(long y = 0; y < img.rows(); y++) {
	  	                                     	
	  	     if(imgMask[0][y][x] == maskValue) {
	  			polynomial.setPosition(x, y);
	  			double v = polynomial.getValue();
	  			if(v > SHRT_MAX) v = SHRT_MAX;
	  			else if(v < SHRT_MIN) v = SHRT_MIN;
				imgFit[0][y][x] = (short)v;
			}
	}}

	return imgFit;
}


IemTImage<short> PolyFit::getErrorImage() {

	IemTImage<short> imgError(1, img.rows(), img.cols());

	for(long x = 0; x < img.cols(); x++) {
	  	for(long y = 0; y < img.rows(); y++) {

			polynomial.setPosition(x, y);

	  		double v = img[0][y][x] - polynomial.getValue();
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;
			imgError[0][y][x] = (short)v;
	}}

	return imgError;
}


IemTImage<short> PolyFit::getFitAndDer() {

	IemTImage<short> imgFit(3, img.rows(), img.cols());
	imgFit = 0;

	for(long x = 0; x < img.cols(); x++) {
	  	for(long y = 0; y < img.rows(); y++) {

	  		polynomial.setPosition(x, y);
	  		
	  		double v = polynomial.getValue();
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;
			imgFit[0][y][x] = (short)v;
			
			v = polynomial.getdX();
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;
			imgFit[1][y][x] = (short)v;
			
			v = polynomial.getdY();  
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;
			imgFit[2][y][x] = (short)v;
	}}

	return imgFit;
}
 

IemTImage<short> PolyFit::getFitFirstDer() {
 
	IemTImage<short> imgFit(1, img.rows(), img.cols());
	imgFit = 0;

	for(long x = 0; x < img.cols(); x++) {
	  	for(long y = 0; y < img.rows(); y++) {

	  		polynomial.setPosition(x, y);

	  		double v = (short)polynomial.getFirstDer();
	  		
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;

			imgFit[0][y][x] = (short)v;
	}}

	return imgFit;
}


double PolyFit::getValueAt(long x, long y) {
	return polynomial.getValue(x, y);
}


void PolyFit::dump(ostream& s) {

	s << "\n\nVandermonde Matrix";
	
	long nTerms = polynomial.getNTerms();

	for(long i = 0; i < nSamples; i++) {
		s << "\n";
		for(long k = 0; k < nTerms; k++) {
			s << setw(15) << matVandermonde[i][k];
		}
		s << setw(15) << matList[i][0];
	}

	s << endl;
}


void PolyFit::write(ostream& s) {

	s << "\n\nPolyFit: ";

	s << ", xMean= " << xMean << ", yMean= " << yMean
	 << ", nSamples= " << nSamples << ", error= " << fitError;
	 
	polynomial.write(s);
}


void PolyFit::test() {

	char outFileName[1024];
	char* outputDir = "c:/tmp";
	
	sprintf(outFileName, "%s/PolyFit.res", outputDir);
	ofstream outFile(outFileName);

	IemTImage<short> imgTarget(1, 512, 512);
	imgTarget = 0;

/*  
	long order = 2;
	Polynomial2 polynomial;
	double coef[6] = {100.0, -0.2, -0.2, -0.01, -0.05, -0.05};
*/
	long order = 4;
	Polynomial polynomial(4);
	double coef[15] = {100.0, -0.2, -0.2, -0.01, -0.05, -0.05, -.0001, 0, 0, -0.00004, 0, 0, 0, 0, 0.0000005};

	polynomial.setCoef(coef);

	outFile << "\n\nTarget polynomial:";
	polynomial.write(outFile);
	
	long xMean = 512/2;
	long yMean = 512/2;

	polynomial.setCenter(xMean, yMean);
	
	for(long x = 0; x < imgTarget.cols(); x++) {
	  	for(long y = 0; y < imgTarget.rows(); y++) {

	  		double v = (short)polynomial.getValue(x, y);
	  		
	  		if(v > SHRT_MAX) v = SHRT_MAX;
	  		else if(v < SHRT_MIN) v = SHRT_MIN;

			imgTarget[0][y][x] = (short)v;
	}}
	
//	imgTarget << "c:/tmp/target1.tif";

	IemTImage<unsigned char> imgSupport(1, imgTarget.rows(), imgTarget.cols());
	imgSupport = 255;


	long xMin = 0, xMax = 511;
	long yMin = 0, yMax = 511;

	PolyFit polyFit(order);
//	polyFit.calcFit(imgTarget, xMin, xMax, yMin, yMax, xMean, yMean, 1);
	
	polyFit.calcFit(imgTarget, imgSupport, xMean, yMean, 1);
	
	outFile << "\n\nFit result:";
	polyFit.write(outFile);
	
	outFile << "\nfitError: " << polyFit.getFitError();
	
	sprintf(outFileName, "%s/target.tif", outputDir);
	iemWrite(imgTarget, outFileName);
	
	sprintf(outFileName, "%s/fit.tif", outputDir);
	iemWrite(polyFit.getFitImage(), outFileName);
	
	sprintf(outFileName, "%s/error.tif", outputDir);
	iemWrite(polyFit.getErrorImage(), outFileName);
}



