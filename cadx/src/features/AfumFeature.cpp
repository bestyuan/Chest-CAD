//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "AfumFeature.h"   
#include "Util.h"   
#include "CadxParm.h"
#include "ImageUtil.h"
#include "Histogram.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  
#include "Iem/IemAggregate.h"


using namespace CADX_SEG;


AfumFeature::AfumFeature()
 : Feature() {     
 	

} 


AfumFeature::AfumFeature(char* _labelRoot)
 : Feature(1, _labelRoot) {
 	
 	initialize();
}


AfumFeature::AfumFeature(char* _labelRoot, Roi& roi)
 : Feature(1, _labelRoot) {
 	
 	initialize(); 
 	 	
	img = roi.getCroppedImage();
	imgMap = roi.getNoduleMap();
}


AfumFeature::AfumFeature(char* _labelRoot, IemTImage<short>& _img, 
 IemTImage<unsigned char>& _imgMap, IemTImage<unsigned char>& _imgExclude)
 : Feature(1, _labelRoot) {
 	
 	initialize();

 	img = _img; 		
	imgMap = _imgMap;
	imgExclude = _imgExclude;
}


void AfumFeature::makeBkMap() {

	double bkSize = 2.0;

	long reff = bkSize * sqrt(area / Util::PI);
	
	long cols = imgMap.cols();
	long rows = imgMap.rows();

	imgBkMap = imgMap.copy();
	
	long decimation  = 3;
	long d = reff / pow(2, decimation);
	d = Util::max(d, 3);
	d = Util::min(d, 13);

	imgBkMap = iemAggregateDown(imgBkMap, decimation, 0);
	imgBkMap = iemDilate(imgBkMap, d, d);
	imgBkMap = iemInterpolate(imgBkMap, rows, cols);

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(imgMap[0][y][x] == Util::NODULE_LABEL) imgBkMap[0][y][x] = 0;
			else if(imgExclude[0][y][x] == 0) imgBkMap[0][y][x] = 0;
			else if(imgBkMap[0][y][x] != 0) imgBkMap[0][y][x] = Util::SURROUND_LABEL;
	}}

}


double AfumFeature::getScore() {
	return value[0];
}


void AfumFeature::calculate() {
	regionStatistics();
	
//	if(area == 0) return;

	makeBkMap();
	regionBkStatistics();

	if(bkArea == 0) value[0] = 0;
	else value[0] = (double)bkCount / (double)bkArea;
}


void AfumFeature::initialize() {

	area = 0;
	bkArea = 0;

	lowerCodeValue = 0;
	bkCount = 0;
	
	lowerHistPenPnt = 0.05;

	centerCol = centerRow = 0;

	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Region");

	sprintf(label[0], "%s.afum", labelRoot);
}


void AfumFeature::regionStatistics() {
	Histogram histogram(0, 4095);
	histogram.build(img, imgMap);
	lowerCodeValue = histogram.getLowerPenetration(lowerHistPenPnt);
	
	userAgent.getLogFile() << "\nAfumFeature::lowerCodeValue= " << lowerCodeValue;
}


void AfumFeature::regionBkStatistics() {

	bkCount = 0;
	bkArea = 0;

	for(long r = 0; r < img.rows(); r++) {
		for(long c = 0; c < img.cols(); c++) {

			if(imgBkMap[0][r][c] == Util::SURROUND_LABEL) {
				bkArea++;
				short cv = img[0][r][c];
				if(cv < lowerCodeValue) bkCount++;
			}
	}}

	userAgent.getLogFile() << "\nAfumFeature::bkCount= " << bkCount 
	 << "bkArea= " << bkArea << flush;
}



