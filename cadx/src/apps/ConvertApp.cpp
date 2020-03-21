//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Iem/Iem.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemInterpolate.h"
#include "ImageReader.h"
#include "Iem/IemTImageIter.h"



using namespace CADX_SEG;



int main(int argc, char* argv[])
{                    
	short narg = 0, upsample = 0;
	char inFileName[1024], outFileName[1024];

	while((++narg) < argc) {

		if(strcmp(argv[narg], "-i") == 0) {
			narg++; strcpy(inFileName, argv[narg]);
		}       
		else if(strcmp(argv[narg], "-o") == 0) {
			narg++; strcpy(outFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-upsample") == 0) {
			narg++; upsample = atoi(argv[narg]);
		}

	}


	ImageReader reader;
	IemTImage<short> img = reader.read(inFileName);


	img *= (255.0 / 4095.0);


	IemTImage<unsigned char> img8bit(1, img.rows(), img.cols());



	IemTImageIter<short> iter(img);
	IemTImageIter<unsigned char> iter8bit(img8bit);

	
	while(!iter.rowDone()) {
		while(!iter.colDone()) {
		                        	
			iter8bit[0] = iter[0];
			iter.colInc(); iter8bit.colInc(); 
		}

		iter.colStart(); iter8bit.colStart(); 
		iter.rowInc(); iter8bit.rowInc(); 
	}


	img8bit >> outFileName;

	return 0;
}
