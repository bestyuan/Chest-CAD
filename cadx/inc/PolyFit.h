//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef POLYFIT_H
#define POLYFIT_H



#include "Util.h"
 
#include "Iem/Iem.h"
//#include "analyzePair.h"
//#include "CCA.h"
//#include "geoDistance.h"
#include "matrixTranspose.h"
#include "matrixMultiply.h"
#include "getError.h"
#include "Polynomial.h"


namespace CADX_SEG {


class PolyFit {

	private:
	Polynomial polynomial;

	IemMatrix	matVandermonde;
	IemMatrix	matList;
	IemMatrix	matCoef;

	IemTImage<short> img;
	IemTImage<unsigned char> imgMask;

	// The code value in the mask that indicates pixels to be 
	// included in the fit.
	short maskValue;
      
	short sample;
	long nSamples;

	long xMean, yMean;
	long xMin, yMin;
	long xMax, yMax;
	
	double fitError;

	public:
	PolyFit();
	PolyFit(long order);
	
	~PolyFit();

	// Fits the whole image.
	void calcFit(IemTImage<short>& img, long sample = 1);

	// Fits a window in the image.	
	void calcFit(IemTImage<short>& _img, long _xMin, long _xMax,
 	 long _yMin, long _yMax, long _xMean, long _yMean, long _sample = 1);

	// Fits a region in the image where the mask is non-zero.
	void calcFit(IemTImage<short>& img, IemTImage<unsigned char>& imgMask, long sample = 1);
	
	void calcFit(IemTImage<short>& img, IemTImage<unsigned char>& imgMask,
	 long xMean, long yMean, long sample = 1);
   
	double getCoef(long i) {return matCoef[i][0];}

	double getValueAt(long x, long y);
	
	double getFitError() {return fitError;}
	
	void setMaskValue(short v) {maskValue = v;}
	
	Polynomial& getPolynomial() {return polynomial;}

	IemTImage<short> getFitImage();
	
	IemTImage<short> getMaskedFitImage();
	
	IemTImage<short> getErrorImage();
	
	IemTImage<short> getFitAndDer();

	IemTImage<short> getFitFirstDer();
	
	void write(ostream& s);

	void initialize(long order);
	
	static void test();

	private:
	void initialize();
		
	void buildMatrix();
	
	void examineMask();
		
	void solve();
		
	void dump(ostream& s);
	



};



} // Namespace CADX





#endif

