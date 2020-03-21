#include "ImageReader.h"
#include "Util.h"
#include "Kespr.h"
#include "IemDicom.h"
#include "Iem/IemConvert.h"
#include "Iem/IemImageIO.h" 


using namespace CADX_SEG;


ImageReader::ImageReader() {
	init();
}


void ImageReader::init() {
	xSpacing = 0;
	ySpacing = 0;
	bitDepth = 0;
} 


IemTImage<short> ImageReader::read(const char* imgFileName) {

	IemTImage<short> img;

	char name[1024];
	strcpy(name, imgFileName);

	char extension[1024];
	strcpy(extension, Util::getExtension(name));
	
	if(strcmp(extension, ".img") == 0) {
		Kespr kespr;
		img = kespr.Read(imgFileName);
		xSpacing = kespr.getXSpacing();
		ySpacing = kespr.getYSpacing(); 
		bitDepth=12;
	}
	else if(strcmp(extension, ".dicom") == 0 || strcmp(extension, ".dcm") == 0) {
		IemDicom iemDicom;
		img = iemDicom.Read(imgFileName);
		xSpacing = iemDicom.getXSpacing();
		ySpacing = iemDicom.getYSpacing();
		bitDepth = iemDicom.getBitDepth();
	}
	else { 
		// This is a work around for a problem reading 8-bit image
		// data into a short data type Iem image.
		IemImage temp = iemRead(imgFileName);
		img = iemConvert(temp, IemShort);
	}

	return img;
}






