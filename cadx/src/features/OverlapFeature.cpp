//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "OverlapFeature.h"
#include "Canny.h"
#include "Util.h"   
#include "CadxParm.h" 
#include "ChainMaker.h"
#include <list>
#include "Iem/IemImageIO.h" 


using namespace CADX_SEG;  


OverlapFeature::OverlapFeature()
 : Feature() {      
 	

}  
 

OverlapFeature::OverlapFeature(char* _labelRoot)
 : Feature(1, _labelRoot) {

	initialize();
}


OverlapFeature::OverlapFeature(char* _labelRoot, Roi& roi)
 : Feature(1, _labelRoot) {

 	initialize(); 

	img = roi.getCroppedImage();
	imgMask = roi.getAnatomicalMap();
	boundary = roi.getBoundary();
} 
 

OverlapFeature::OverlapFeature(char* _labelRoot, IemTImage<short>& _img, Boundary& _boundary, 
 IemTImage<unsigned char>& _imgMask) : Feature(1, _labelRoot) {

 	initialize();  
 	
 	img = _img; 		 
	imgMask = _imgMask;
	boundary = _boundary;
}

 


double OverlapFeature::getScore() {
	return value[0];
} 


void OverlapFeature::calculate() {

	imgOverlapMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgOverlapMap = 0;

	Canny canny;
	canny.setUserAgent(userAgent);
	imgEdgeMap = canny.calculate(img, imgMask);
	
	ChainMaker chainMaker;
	chainMaker.setUserAgent(userAgent);
	imgEdgeMap = chainMaker.apply(imgEdgeMap);
	
	list<Point> boundaryList =  boundary.getList();
	
	long nBoundaryPts = boundaryList.size();
	long nOverlapPts = 0;
	
	list<Point>::iterator iter; 

	for(iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {
		long col = (*iter).x;
		long row = (*iter).y;
		
		if(isOverlapping(col, row)) {nOverlapPts++; imgOverlapMap[0][row][col] = 255;}
	}

	overlap = (double)nOverlapPts / (double)nBoundaryPts;
	
	userAgent.getLogFile() << "\nOverlap::nOverlapPts= " << nOverlapPts
	 << ", nBoundaryPts= " << nBoundaryPts << ", overlap= " << overlap;
	 
//	userAgent.writeDebugImage(imgOverlapMap, "Overlap");

	value[0] = overlap; 
} 


bool OverlapFeature::isOverlapping(long col, long row) {

	long cols = imgEdgeMap.cols();
	long rows = imgEdgeMap.rows();

	for(long deltaCol = -lookAround; deltaCol <= lookAround; deltaCol++) {
		for(long deltaRow = -lookAround; deltaRow <= lookAround; deltaRow++) {

			long c = col + deltaCol;
			long r = row + deltaRow;
			
			if(c < 0 || r < 0 || c >= cols || r >= rows) return false;

			if(imgEdgeMap[0][r][c] != 0) return true;
	}}
		
	return false;
}


void OverlapFeature::initialize() {
 
	regionLabel = Util::NODULE_LABEL;  
	
	lookAround = 3;  
	
	overlap = 0;


	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Overlap");
	
	sprintf(label[0], "%s.overlap", labelRoot);
}




