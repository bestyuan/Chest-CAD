//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "IemDicom.h" 
#include "Util.h"
#include "Iem/IemTImageIter.h"
#include <string>
#include "DicomMRIImage.h"

  
using namespace CADX_SEG;


   

     
IemDicom::IemDicom() {
	height = width = 0;
	bands = 1;
	xSpacing = ySpacing = 0;
	bitDepth = 0;
}


IemTImage<short> IemDicom::Read(const char* inFileName) {

	int rows = 0;
	int columns = 0;
	int bitsAllocated = 0;

	DicomMRIImage inputImage;
	DicomMRIIO_Status status;
	
	string inputImageFileName(inFileName); 
	
	inputImage.SetImageFileName(inputImageFileName);

	status = DMRIIO_Success;
	status = inputImage.GetImageSizeParameters(rows, columns, bitsAllocated);

	height = rows; 
	width = columns;

	IemTImage<short> img(bands, rows, columns); 
	
	IemTImageIter<short> iter(img);
	
	int bytesPerPixel = bitsAllocated / 8;
	unsigned int imageSize = rows * columns * bytesPerPixel;
	
	unsigned short *pSpace = new unsigned short[imageSize];
	unsigned short *pPixels = pSpace; 
	
	status = DMRIIO_Success;
	status = inputImage.GetPixels(pPixels);
	
	while(!iter.rowDone()) {
		while(!iter.colDone()) {

			iter[0] = (*pPixels);
          	pPixels++;

			iter.colInc();
		}
		iter.colStart(); iter.rowInc();
	}
	
	delete[] pSpace;
	
	ElementList eList;
	status = DMRIIO_Success;
	status = inputImage.GetElementList(eList); 

	try {
		bitDepth = eList[Tag::kBitsStored]; 
		classics::string tagValue = eList[Tag::kImagerPixelSpacing];
		string s = tagValue.c_str();
		xSpacing = ySpacing = atof(s.c_str()); 
	}
	catch (...) 
	{}

	return img;
}

