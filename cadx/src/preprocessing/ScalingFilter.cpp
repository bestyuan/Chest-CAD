//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "ScalingFilter.h"   
#include "util.h"  
#include "CadxParm.h"  
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"


using namespace CADX_SEG;


ScalingFilter::ScalingFilter() {
	initialize();
}


ScalingFilter::ScalingFilter(double _meanAim, double _sigmaAim, short _minCodeValue, short _maxCodeValue) {
	initialize();
	
	meanAim = _meanAim; 
	sigmaAim = _sigmaAim;
	minCodeValue = _minCodeValue; 
	maxCodeValue = _maxCodeValue;
}


void ScalingFilter::initialize() {

	windowHalfWidth = 5;
	subsample = 1;
	meanAim = 2000;
	sigmaAim = 200;
	minCodeValue = 0;
	maxCodeValue = 4095;
}



     
IemTImage<short> ScalingFilter::windowedScaling(IemTImage<short>& img, long _windowHalfWidth) {     
		
	long tpad, lpad, rpad, bpad;
	
	_windowHalfWidth = windowHalfWidth;
	
	tpad = lpad = rpad = bpad = windowHalfWidth;
		
	IemTImage<short> imgPadded = iemPadOut(img, tpad, lpad, rpad, bpad);    
	
	IemTImage<short> imgFiltered(1, img.rows() / subsample, img.cols() / subsample); 
	
	
	for(long c = 0; c < img.cols(); c += subsample) {
		for(long r = 0; r < img.rows(); r += subsample) {  
						
			imgFiltered[0][r/subsample][c/subsample] = (short)scale(imgPadded, c, r);
	}}  
	
	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}  


double ScalingFilter::scale(IemTImage<short>& img, long c, long r) {
	
	long sum = 0;
	double sum2 = 0; 
	
	long ccEnd = c + 2 * windowHalfWidth;
	long rrEnd = r + 2 * windowHalfWidth;
	
	long c0 = c + windowHalfWidth;
	long r0 = r + windowHalfWidth;
	
	long size = Util::square(2 * windowHalfWidth + 1); 
	
	double cv0 = img[0][r + windowHalfWidth][c + windowHalfWidth];
			
	for(long cc = c; cc <= ccEnd; cc++) {  	
		for(long rr = r; rr <= rrEnd; rr++) {
				
			short v = img[0][rr][cc]; 
			
			sum += v;
			sum2 += v * v;
		}  
	}

	double mean = (double)sum / (double)size;
	double sigma = sqrt((double)sum2 / (double)size - mean * mean); 
	
	double scale = (sigmaAim / sigma) * (cv0 - mean) + meanAim;
	
	return scale;
}


IemTImage<short> ScalingFilter::globalScaling(IemTImage<short>& img) {     
		
	double sum = 0;
	double sum2 = 0; 
	long size = img.cols() * img.rows();
	
	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {  
		
			short v = img[0][r][c];
			
			sum += v; 
			sum2 += v * v;
	}}  
	
	double mean = (double)sum / (double)size; 
	double sigma = sqrt((double)sum2 / (double)size - mean * mean); 
/*	
	userAgent.getLogFile() << "\nScalingFilter::globalScaling()"
	  << "\nmean= " << mean << ", sigma= " << sigma
	  << "\nmeanAim= " << meanAim << ", sigmaAim= " << sigmaAim;
*/	
	IemTImage<short> imgScaled(1, img.rows(), img.cols());   
		
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {  
		
			double v0 = img[0][r][c];
						
			short v = (short)((sigmaAim / sigma) * (v0 - mean) + meanAim);
			
			//if(v < minCodeValue) v =  minCodeValue;
			//else if(v > maxCodeValue) v =  maxCodeValue;
						
			imgScaled[0][r][c] = v;						
	}}  
	
	return imgScaled;		
}

IemTImage<short> ScalingFilter::globalScaling(IemTImage<short>& img, long band, IemTImage<unsigned char>& imgMask) {
		
	double sum = 0;
	double sum2 = 0; 
	long size = 0;
	
	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {  
		
			if(imgMask[0][r][c] != 0) {
		
				size++;
		
				short v = img[band][r][c];
			
				sum += v;
				sum2 += v * v;
			}
	}}  

	double mean = (double)sum / (double)size;
	double sigma = sqrt((double)sum2 / (double)size - mean * mean); 
	
	userAgent.getLogFile() << "\nScalingFilter::globalScaling()"
	  << "\nmean= " << mean << ", sigma= " << sigma
	  << "\nmeanAim= " << meanAim << ", sigmaAim= " << sigmaAim;
	
	IemTImage<short> imgScaled(1, img.rows(), img.cols());   
		
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {  
		
			double v0 = img[band][r][c];
						
			short v = (short)((sigmaAim / sigma) * (v0 - mean) + meanAim);
			
			if(v < minCodeValue) v =  minCodeValue;
			else if(v > maxCodeValue) v =  maxCodeValue;
						
			imgScaled[0][r][c] = v;						
	}}  
	
	return imgScaled;		
}





