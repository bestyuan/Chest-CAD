//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "StructRemover.h"   
#include "util.h"  
#include "CadxParm.h" 
#include "Iem/IemImageIO.h"
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  
#include "FFT.h" 



StructRemover::StructRemover() {

	initialize();

}

void StructRemover::initialize() {



}

     
IemTImage<short> StructRemover::filter(Roi& roi) {  
	
	char outFileName[1024], outputDir[1024];
	
	strcpy(outputDir, CadxParm::getInstance().getOutputDir());

	IemTImage<short> img = roi.getCroppedImage();

		
	IemTImage<short> imgClosed = iemDilate(roi.getCroppedImage(), 5, 5);
//	imgClosed = iemErode(imgClosed, 9, 9);		

	if(CadxParm::getInstance().getDebug()) {			
		sprintf(outFileName, "%s/%s_morph.tif", outputDir, roi.getName());
		iemWrite(imgClosed, outFileName);    
	}    
	

	IemTImage<float> imgFFT = FFT::calcFFT(roi.getCroppedImage());		
	
	IemTImage<float> imgFFTMag = FFT::magFFT(imgFFT);		

	if(CadxParm::getInstance().getDebug()) {			
		sprintf(outFileName, "%s/%s_fft_mag.tif", outputDir, roi.getName());
		iemWrite(imgFFTMag, outFileName);    
	}  
	
	
	IemTImage<float> imgFFTFiltered = FFT::filterFFT(imgFFT);		
	
	IemTImage<float> imgFFTFilteredMag = FFT::magFFT(imgFFTFiltered);		

	if(CadxParm::getInstance().getDebug()) {			
		sprintf(outFileName, "%s/%s_fft_filtered_mag.tif", outputDir, roi.getName());
		iemWrite(imgFFTFilteredMag, outFileName);    
	}  
	
	
	IemTImage<short> imgFFTInverse = FFT::calcInverseFFT(imgFFTFiltered);		

	if(CadxParm::getInstance().getDebug()) {			
		sprintf(outFileName, "%s/%s_fft_inverse.tif", outputDir, roi.getName());
		iemWrite(imgFFTInverse, outFileName);    
	}  
	

	
	return img;
}  

/*
IemTImage<short> StructRemover::filter2(IemTImage<short>& img, char* name) {  
	
	char outFileName[1024], outputDir[1024];
	
	strcpy(outputDir, CadxParm::getInstance().getOutputDir());
	
	double n = log10(img.cols());
	n = n / log10(2);
	n = floor(n);
	long Ncols = pow(2, n);
	long Nrows = Ncols * (

	IemTImage<short> imgResized = iemInterpolate(img, Nrows, Ncols);

	

	
	return img;
}  
*/

