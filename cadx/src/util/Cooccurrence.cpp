//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Cooccurrence.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;


Cooccurrence::Cooccurrence() {

	initialize();

	CM = IemMatrix(nLevels, nLevels, NULL);

	accum = IemMatrix(nLevels, nLevels, NULL);
	accum = 0;

	makeDisplacements();
}


void Cooccurrence::calculate(IemTImage<short>& _img, long _band, short _minValue, short _maxValue,
 IemTImage<unsigned char>& _imgMap) {

	img = _img;
	imgMap = _imgMap;
	
	minValue = _minValue;
	maxValue = _maxValue;

	band = _band;
 
	imgTexture = IemTImage<short>(1, img.rows(), img.cols());

	makeQuantizedImage();
	accumulateStatistics();

	if(nSamples == 0) return;

	makeCM();
	calcDescriptors();
}


void Cooccurrence::initialize() {
     minValue = 0;
	maxValue = 4095;
	nLevels = 64; 
	nDisplacements = 0;
	band = 0;
	nSamples = 0;
	energy = 0;
	contrast = 0;
	displacementMag = 1;
} 


void Cooccurrence::makeDisplacements() {

	nDisplacements = 8;

	displacement = IemTImage<short>(1, nDisplacements, 2);
	
	long d = displacementMag;

	displacement[0][0][0] = d;	displacement[0][0][1] = 0;
	displacement[0][1][0] = 0;	displacement[0][1][1] = d;
	displacement[0][2][0] = -d;	displacement[0][2][1] = 0;
	displacement[0][3][0] = 0;	displacement[0][3][1] = -d;
	displacement[0][4][0] = d;	displacement[0][4][1] = d;
	displacement[0][5][0] = -d;	displacement[0][5][1] = -d;
	displacement[0][6][0] = d;	displacement[0][6][1] = -d;
	displacement[0][7][0] = -d;	displacement[0][7][1] = d;
}


void Cooccurrence::makeQuantizedImage() {

	imgQuantized = IemTImage<short>(1, img.rows(), img.cols());

	double binSize = (maxValue - minValue) / nLevels;

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {

			double d = (double)(img[band][r][c] - minValue) / (double)(maxValue - minValue);

			short v = (short)((double)(nLevels - 1) * d);

			v = Util::max(0, v);
			v = Util::min(nLevels - 1, v); 
			
			imgQuantized[0][r][c] = v;
	}}
} 


void Cooccurrence::accumulateStatistics() {

	long cols = img.cols();
	long rows = img.rows(); 

	for(long c0 = 0; c0 < cols; c0++) {
		for(long r0 = 0; r0 < rows; r0++) {

			if(imgMap[0][r0][c0] != 0) {

				for(long d = 0; d < nDisplacements; d++) {

					long r1 = r0 + displacement[0][d][0];
					long c1 = c0 + displacement[0][d][1];
				
					if(r1 < 0 || r1 >= rows || c1 < 0 || c1 >= cols) continue;

					if(imgMap[0][r1][c1] == 0) continue;

					short i = imgQuantized[0][r0][c0];
					short j = imgQuantized[0][r1][c1];

					accum[i][j] += 1;

					nSamples++;
				}
				 
 


			}

		                                    	
	}}

}


void Cooccurrence::makeCM() {

	double energy = 0;
	
	if(nSamples == 0) {
		CM = 0;
		return;
	}

	for(long i = 0; i < nLevels; i++) {   
		for(long j = 0; j < nLevels; j++) {
		                                  	
			CM[i][j] = (double)accum[i][j] / (double)nSamples;
	}}
}
 

void Cooccurrence::calcDescriptors() {

	energy = 0;
	contrast = 0;

	for(long i = 0; i < nLevels; i++) {
		for(long j = 0; j < nLevels; j++) {
		                                  	
			double cm = CM[i][j];
		                                    	
			energy += cm * cm; 

			long delta = (i - j);
			contrast += delta * delta * cm;
	}}
}




 





