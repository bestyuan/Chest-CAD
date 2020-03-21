//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Canny.h"
#include "EdgeDetector.h"
#include "BlurFilter.h"
#include "Histogram.h"
#include "Iem/IemImageIO.h"
  

using namespace CADX_SEG;

short Canny::NONEDGE = 0;
short Canny::HORIZONTAL = 1;
short Canny::VERTICAL = 2;
short Canny::POSDIAGONAL = 3;
short Canny::NEGDIAGONAL = 4;


Canny::Canny() {
	initialize();
}
 

Canny::~Canny() {

}
	

void Canny::initialize() {
     lowThreshold = 0;
	highThreshold = 0;
	edgeMagHistPen = 0.20;
}
 

IemTImage<unsigned char> Canny::calculate(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask) {

	img = _img;
	imgMask = _imgMask;

	BlurFilter blurFilter(7, 7, 3);
	imgBlurred = blurFilter.filter(img);

	EdgeDetector edgeDetector;
	imgEdge = edgeDetector.calcSobelGradient(imgBlurred);
//	userAgent.writeDebugImage(imgEdge, "Canny_Edge");

	makeEdgeMap();

	nonMaxSuppression();
	
	imgNonMaxSupEdgeMap = imgEdgeMap.copy();
//	if(userAgent.getDebug()) userAgent.writeDebugImage(getColorEdgeMap(imgNonMaxSupEdgeMap), "Canny_NonMaxSup");

	calcThresholds();
	 
	userAgent.getLogFile() << "\nCanny::highThreshold= " << highThreshold
	 << "\nCanny::lowThreshold= " << lowThreshold;
	 
	applyHighThreshold();
	imgHighThresEdgeMap = imgEdgeMap.copy();
//	if(userAgent.getDebug()) userAgent.writeDebugImage(getColorEdgeMap(imgHighThresEdgeMap), "Canny_HighThreshold");
	
	applyHysteresis();
//	if(userAgent.getDebug()) userAgent.writeDebugImage(getColorEdgeMap(imgEdgeMap), "Canny");

	return imgEdgeMap;
}
  

void Canny::calcThresholds() {
	Histogram histogram(0, 500);
	histogram.build(imgEdge, imgEdgeMap, 0);
	highThreshold = histogram.getUpperPenetration(edgeMagHistPen);
	lowThreshold = highThreshold / 4.0;
}
   

void Canny::makeEdgeMap() {

	imgEdgeMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgEdgeMap = NONEDGE;

	for(long c = 1; c < img.cols() - 1; c++) {
		for(long r = 1; r < img.rows() - 1; r++) {
		     if(imgMask[0][r][c] != 0) {
				imgEdgeMap[0][r][c] = getOrientation((double)imgEdge[1][r][c]);
			}
	}}

}
   

IemTImage<unsigned char> Canny::getColorEdgeMap(IemTImage<unsigned char>& imgEdgeMap) {

	IemTImage<unsigned char> imgColorEdgeMap(3, imgEdgeMap.rows(), imgEdgeMap.cols());
	imgColorEdgeMap = 0;

	for(long c = 0; c < imgEdgeMap.cols(); c++) {
		for(long r = 0; r < imgEdgeMap.rows(); r++) {

			short dir = imgEdgeMap[0][r][c];
			
			if(dir == VERTICAL) {imgColorEdgeMap[0][r][c] = 255; imgColorEdgeMap[1][r][c] = 0; imgColorEdgeMap[2][r][c] = 0;}
			else if(dir == HORIZONTAL) {imgColorEdgeMap[0][r][c] = 0; imgColorEdgeMap[1][r][c] = 255; imgColorEdgeMap[2][r][c] = 0;}
			else if(dir == POSDIAGONAL) {imgColorEdgeMap[0][r][c] = 0; imgColorEdgeMap[1][r][c] = 0; imgColorEdgeMap[2][r][c] = 255;}
			else if(dir == NEGDIAGONAL) {imgColorEdgeMap[0][r][c] = 255; imgColorEdgeMap[1][r][c] = 255; imgColorEdgeMap[2][r][c] = 0;}
	}}

	return imgColorEdgeMap;
}


void Canny::nonMaxSuppression() {

	short mag1, mag2, mag;

	for(long c = 1; c < img.cols() - 1; c++) {
		for(long r = 1; r < img.rows() - 1; r++) {
		                                         	
		     short orientation = imgEdgeMap[0][r][c];

		     if(orientation == NONEDGE) continue;

			if(orientation == VERTICAL) {
				mag1 = imgEdge[0][r][c-1];
				mag2 = imgEdge[0][r][c+1];
			}
			else if(orientation == HORIZONTAL) {
				mag1 = imgEdge[0][r-1][c];
				mag2 = imgEdge[0][r+1][c];
			}
			else if(orientation == POSDIAGONAL) {
				mag1 = imgEdge[0][r-1][c-1];
				mag2 = imgEdge[0][r+1][c+1];
			}
			else if(orientation == NEGDIAGONAL) {
				mag1 = imgEdge[0][r+1][c-1];
				mag2 = imgEdge[0][r-1][c+1];
			}
			
			mag = imgEdge[0][r][c];
			
			if(mag1 > mag || mag2 > mag) imgEdgeMap[0][r][c] = NONEDGE;

	}}

}


void Canny::applyHighThreshold() {

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgEdge[0][r][c] < highThreshold) imgEdgeMap[0][r][c] = NONEDGE;
	}}
}
 


void Canny::applyHysteresis() { 

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgHighThresEdgeMap[0][r][c] != NONEDGE) {
				followEdge(c, r);
			}
	}}
}


void Canny::followEdge(long col, long row) {

	for(long deltaCol = -1; deltaCol <= 1; deltaCol++) {
		for(long deltaRow = -1; deltaRow <= 1; deltaRow++) {

			long c = col + deltaCol;
			long r = row + deltaRow;

			if(imgNonMaxSupEdgeMap[0][r][c] == NONEDGE) continue;

			if(imgEdge[0][r][c] >= lowThreshold && imgEdgeMap[0][r][c] == NONEDGE) {
				imgEdgeMap[0][r][c] = imgNonMaxSupEdgeMap[0][r][c];
				followEdge(c, r);
			}
	}}

}


short Canny::getOrientation(double angle) {
	if(angle >= 337.5 || angle < 22.5) return VERTICAL;
	else if(angle >= 22.5 && angle < 67.5) return NEGDIAGONAL;
	else if(angle >= 67.5 && angle < 112.5) return HORIZONTAL;
	else if(angle >= 112.5 && angle < 157.5) return POSDIAGONAL;
	else if(angle >= 157.5 && angle < 202.5) return VERTICAL;
	else if(angle >= 202.5 && angle < 247.5) return NEGDIAGONAL;
	else if(angle >= 247.5 && angle < 292.5) return HORIZONTAL;
	else if(angle >= 292.5 && angle < 337.5) return POSDIAGONAL;
	else return NONEDGE;
}












