//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


 
#include "RegionSupport.h"
#include "RegionLabel.h"
#include "ImageUtil.h"
#include "Iem/IemMorphological.h"


using namespace CADX_SEG;


RegionSupport::RegionSupport() {
	initialize();
}


void RegionSupport::initialize() {
	colSeed = 0;
	rowSeed = 0;
	minCodeValue = 1;
//	erosionKernelSize = 15;
	erosionKernelSize = 7;
	aggregation = 1;
	area = 0;
}

  
IemTImage<unsigned char> RegionSupport::makeMap(IemTImage<short>& _img, IemTImage<unsigned char>& _imgAnatomyMap,
 long _colSeed, long _rowSeed) {

	img = _img;
	imgAnatomyMap = _imgAnatomyMap;
	
	colSeed = _colSeed;
	rowSeed = _rowSeed;

	imgSupportMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgSupportMap = 0;

	if(img[0][rowSeed][colSeed] < minCodeValue) return imgSupportMap;

	initializeMap();

	erodeMap();
	
	// If the erosion has reduced the support region to nothing return.
	if(area == 0) {
		imgSupportMap = 0;
 		return imgSupportMap;
	}

	labelMap();
	pruneMap();
	dilateMap();

	return imgSupportMap;
}

 
void RegionSupport::initializeMap() {

	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {
			if(img[0][y][x] >= minCodeValue && imgAnatomyMap[0][y][x] == 255) {
			     imgSupportMap[0][y][x] = 255;
			     area++;
			}
	}}
	
//	userAgent.writeDebugImage(imgSupportMap, "SupportInitial");
}


void RegionSupport::erodeMap() {
	short d = erosionKernelSize;
//	imgSupportMap = iemErode(imgSupportMap, d, d);
	imgSupportMap = ImageUtil::erodeMap(imgSupportMap, d, aggregation);
//	userAgent.writeDebugImage(imgSupportMap, "SupportEroded");
	
	area = 0;
	
	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgSupportMap[0][r][c] != 0) {
				area++;
			}
	}}
}
	

void RegionSupport::labelMap() {
	CADX_SEG::RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgSupportMap, 0, 255, 10000);
	imgLabeled = regionLabel.getLabeledImage();
//	userAgent.writeDebugImage(imgLabeled, "SupportLabeled");
}


void RegionSupport::pruneMap() {

	long label = 0;
	double d2min = DBL_MAX;
 
	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgSupportMap[0][r][c] != 0) {
				double delCol = c - colSeed;
				double delRow = r - rowSeed;
				double d2 = delCol * delCol + delRow * delRow;
				if(d2 < d2min) {d2min = d2; label = imgLabeled[0][r][c];}
			} 
	}} 
	
	area = 0;  
	
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgLabeled[0][r][c] == label) {imgSupportMap[0][r][c] = 255; area++;}
			else imgSupportMap[0][r][c] = 0;
		}  
	}

//	userAgent.writeDebugImage(imgSupportMap, "SupportPruned");
}  


void RegionSupport::dilateMap() {
	short d = erosionKernelSize;
//	imgSupportMap = iemDilate(imgSupportMap, d, d);
	imgSupportMap = ImageUtil::dilateMap(imgSupportMap, d, aggregation);
//	userAgent.writeDebugImage(imgSupportMap, "SupportDilated");
}


