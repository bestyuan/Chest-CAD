//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Histogram.h"
#include "CadxParm.h"  



using namespace CADX_SEG;



Histogram::Histogram() {
	initialize();
}



Histogram::Histogram(long _minCodeValue, long _maxCodeValue) {
	initialize();
	minCodeValue = _minCodeValue;
	maxCodeValue = _maxCodeValue;
	noLevels = maxCodeValue - minCodeValue + 1;

	histogram = new double[noLevels];
}


Histogram::~Histogram() {
	if(histogram != NULL) delete[] histogram;
}   


void Histogram::initialize() {
	histogram = NULL;
	minCodeValue = 0;  
	maxCodeValue = 0;  
	noLevels = 0;	
	band = 0;
}


void Histogram::build(IemTImage<short>& _img, long _band) {
	img = _img;
	band = _band;

	for(long k = 0; k < noLevels; k++) {
		histogram[k] = 0;
	}  

	for(long r = 0; r < img.rows(); r++) {
		for(long c = 0; c < img.cols(); c++) {

			short l = img[band][r][c] - minCodeValue;
			
			if(l < 0) l = 0;
			else if(l >= noLevels) l = noLevels - 1;
			
			histogram[l]++;

	}}    
	       
	double nPixels = img.rows() * img.cols();
	                               
	// Normalize histogram.	                             
	for(long j = 0; j < noLevels; j++) {
		histogram[j] /= nPixels;
	}
} 
 

void Histogram::build(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask, long _band) {
	img = _img;	
	imgMask = _imgMask; 
	band = _band;
	
	long nPixels = 0;

	for(long k = 0; k < noLevels; k++) {
		histogram[k] = 0;
	}  

	for(long r = 0; r < img.rows(); r++) {
		for(long c = 0; c < img.cols(); c++) {
		                       	
			if(imgMask[band][r][c] != 0) {
				short l = img[band][r][c] - minCodeValue;

				if(l < 0) l = 0;
				else if(l >= noLevels) l = noLevels - 1;
			
				histogram[l]++;
				nPixels++;
			}
	}}

	// Normalize histogram.	                             
	for(long j = 0; j < noLevels; j++) {
		histogram[j] /= (double)nPixels;
	}
}

  
IemTImage<short>  Histogram::equalize(IemTImage<short>& _img, long _band) {

	double sum = 0.0;
	band = _band;
                                                       
	if(histogram == NULL) build(_img);
	 
   	double* shape = new double[noLevels];

	for(long k = 0; k < noLevels; k++) {
		sum += histogram[k];
		shape[k] = sum;
	}

     IemTImage<short> imgEq(1, img.rows(), img.cols());

	for(long r = 0; r < img.rows(); r++) {		                                        
		for(long c = 0; c < img.cols(); c++) { 

			imgEq[0][r][c] = (short)(minCodeValue + (maxCodeValue - minCodeValue)
				* shape[(long)(img[band][r][c] - minCodeValue)]);
		}
	}

	delete[] shape;  
	
	return imgEq;
}


short Histogram::getUpperPenetration(double fraction) {

	double sum = 0.0;

	for(long k = noLevels - 1; k >= 0; k--) {      
		sum += histogram[k];
		if(sum >= fraction) break;
	}
	  
	return k + minCodeValue;
}


short Histogram::getLowerPenetration(double fraction) {

	double sum = 0.0;

	for(long k = 0; k < noLevels; k++) {
		sum += histogram[k];
		if(sum >= fraction) break;
	}
	  
	return k + minCodeValue;
}

void	Histogram::write(ostream& s) {

	double sum = 0.0;

	for(long k = 0; k < noLevels; k++) {
	     sum += histogram[k];
		s << "\nlevel= " << k + minCodeValue << ", sum= " << sum;
	}
}
	               
	               
