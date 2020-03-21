//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "RegionFeature.h"   
#include "Util.h"   
#include "CadxParm.h"
#include "ImageUtil.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  
#include "Iem/IemAggregate.h"


using namespace CADX_SEG;


RegionFeature::RegionFeature() 
 : Feature() {     
 	

} 


RegionFeature::RegionFeature(char* _labelRoot)
 : Feature(4, _labelRoot) {
 	
 	initialize();
}


RegionFeature::RegionFeature(char* _labelRoot, Roi& roi)
 : Feature(4, _labelRoot) {
 	
 	initialize(); 
 	 	
	img = roi.getCroppedImage();
	imgMap = roi.getNoduleMap();
}


RegionFeature::RegionFeature(char* _labelRoot, IemTImage<short>& _img, 
 IemTImage<unsigned char>& _imgMap, IemTImage<unsigned char>& _imgExclude)
 : Feature(4, _labelRoot) {
 	
 	initialize();

 	img = _img; 		
	imgMap = _imgMap;
	imgExclude = _imgExclude;
}


void RegionFeature::makeBkMap() {

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


double RegionFeature::getScore() {
	return value[0];
}


void RegionFeature::calculate() {
	regionStatistics();
	
	if(area == 0) return;
	
	if(bkSubtraction) {
		makeBkMap();
		regionBkStatistics();
		value[0] = avgCodeValue - avgBkCodeValue;
		value[1] = sigmaCodeValue - sigmaBkCodeValue;
		value[2] = minCodeValue - minBkCodeValue;
		value[3] = maxCodeValue - maxBkCodeValue;
	}
	else {
		value[0] = avgCodeValue;
		value[1] = sigmaCodeValue;
		value[2] = minCodeValue;
		value[3] = maxCodeValue;
	}


}


void RegionFeature::initialize() {      

	area = 0;
	bkArea = 0;

	avgCodeValue = 0;
	sigmaCodeValue = 0;
	avgBkCodeValue = 0;
	sigmaBkCodeValue = 0;
	minCodeValue = SHRT_MAX; 
	maxCodeValue = SHRT_MIN;
	minBkCodeValue = SHRT_MAX;
	maxBkCodeValue = SHRT_MIN;

	centerCol = centerRow = 0;
	minCol = minRow = 0;
	maxCol = maxRow = 0;
	bkSubtraction = 1;
	
	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Region");

	sprintf(label[0], "%s.deltaAvgCodeValue", labelRoot);
	sprintf(label[1], "%s.deltaSigmaCodeValue", labelRoot);
	sprintf(label[2], "%s.deltaMinCodeValue", labelRoot);
	sprintf(label[3], "%s.deltaMaxCodeValue", labelRoot);
}


void RegionFeature::regionStatistics() {  

	double codeValueSum = 0, codeValueSum2 = 0;
	
	centerCol = 0;
	centerRow = 0;
	area = 0;

	for(long r = 0; r < img.rows(); r++) {
		long lineCodeValueSum = 0;
		double lineCodeValueSum2 = 0;
					                                        
		for(long c = 0; c < img.cols(); c++) {

			if(imgMap[0][r][c] == Util::NODULE_LABEL) {
				area++;
				
				centerCol += c;
				centerRow += r;
   				
				short cv = img[0][r][c];
   				
				lineCodeValueSum += cv;
				lineCodeValueSum2 += cv * cv;
				
				if(cv < minCodeValue) {minCol = c; minRow = r; minCodeValue = cv;}
				if(cv > maxCodeValue) {maxCol = c; maxRow = r; maxCodeValue = cv;}
			}
		}
   		
		codeValueSum += lineCodeValueSum;
  		codeValueSum2 += lineCodeValueSum2;
	}

	if(area == 0) return;
	
	centerCol /= area;
	centerRow /= area;
	
	avgCodeValue =  codeValueSum / (double)area; 
	sigmaCodeValue =  sqrt(codeValueSum2 / (double)area - avgCodeValue * avgCodeValue);
}


void RegionFeature::regionBkStatistics() {
	
	double bkCodeValueSum = 0, bkCodeValueSum2 = 0;
	
	bkArea = 0; 

	for(long r = 0; r < img.rows(); r++) {
		long lineBkCodeValueSum = 0;
		double lineBkCodeValueSum2 = 0;
					                                        
		for(long c = 0; c < img.cols(); c++) {

			if(imgBkMap[0][r][c] == Util::SURROUND_LABEL) {
				bkArea++;
				
				short cv = img[0][r][c];
				
				if(cv < minBkCodeValue) minBkCodeValue = cv;
				if(cv > maxBkCodeValue) maxBkCodeValue = cv;
				    
				lineBkCodeValueSum += cv;
				lineBkCodeValueSum2 += cv * cv;
			}
		}

  		bkCodeValueSum += lineBkCodeValueSum;	 
  		bkCodeValueSum2 += lineBkCodeValueSum2;	
	}     

	if(bkArea == 0) return;

	avgBkCodeValue = bkCodeValueSum / (double)bkArea;   
	sigmaBkCodeValue =  sqrt(bkCodeValueSum2 / (double)bkArea - avgBkCodeValue * avgBkCodeValue);   
}



