//
//  (c) Copyright Eastman Kodak Company
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef COOCCURRENCE_H
#define COOCCURRENCE_H



#include "Roi.h"
#include "Iem/Iem.h"   
#include "Iem/IemMatrix.h"

namespace CADX_SEG {

class Cooccurrence {

	private:

     // Input image.
	IemTImage<short> img;

	// Map of pixels that should be included in statistics.
	IemTImage<unsigned char> imgMap;

     // The image after quantization to nLevels.
	IemTImage<short> imgQuantized;

	IemTImage<short> imgTexture;

	// Accumulator for number of occurrences.
	IemMatrix accum;
	
	// Accumulator for number of occurrences.
	IemTImage<short> displacement;

	// Cooccurrence matrix.
	IemMatrix CM;
	
	// Code value range of input image.
	short minValue, maxValue;

	// The band of input image to use in calculation.
	long band;

	// The number of levels in cooccurrence matrix.
	long nLevels;
	
	// The number of displacements.
	long nDisplacements;
	
	// Magnitude if displacement.
	long displacementMag;

	// Number of samples that contribute to statistics.
	long nSamples;
	
	// Cooccurrence matrix descriptors.
	double energy;
	double contrast;

	public:     
	
	Cooccurrence();

	void calculate(IemTImage<short>& img, long band, short minValue, short maxValue,
 	 IemTImage<unsigned char>& imgMap);

	double getEnergy() {return energy;}
 	double getConstrast() {return contrast;}
 	
 	IemTImage<unsigned char> getMap() {return imgMap;}
	IemTImage<short> getQuantizedImage() {return imgQuantized;}

	private:
	
	void initialize();
	
	void accumulateStatistics();
	
	void makeCM();
	
	void makeQuantizedImage();  
	
	void makeDisplacements();
	
	void calcDescriptors();

};

} // end namespace

#endif 

