//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "ChainMaker.h"
#include "Util.h"
#include "Canny.h"

using namespace CADX_SEG;
 

ChainMaker::ChainMaker() {
	initialize();	
}


void ChainMaker::initialize() {
	minChainSize = 30;
	minLinearity = 0.85;
}
	 
		
IemTImage<unsigned char> ChainMaker::apply(IemTImage<unsigned char>& _img) {

	img = _img;

	imgLabeled = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgLabeled = Chain::NOCHAIN;

	imgChainMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgChainMap = 0;
	 
	findEndPoints();
	growEndPoints();
	refineChainMap();
	
//	userAgent.writeDebugImage(getColorLabelMap(), "ChainMaker_Labeled");
//	userAgent.writeDebugImage(imgChainMap, "ChainMaker_ChainMap");

	return imgChainMap;
}


void ChainMaker::findEndPoints() {

	// Used to keep track of which neighbors are edge pixels.
	bool neighbor[8];

	for(long c = 1; c < img.cols() - 1; c++) {
		for(long r = 1; r < img.rows() - 1; r++) {

			if(img[0][r][c] == Canny::NONEDGE) continue;

			long n = 0;  
			if(img[0][r-1][c-1] != Canny::NONEDGE) {n++; neighbor[0] = true;}
			else {neighbor[0] = false;}

			if(img[0][r-1][c] != Canny::NONEDGE) {n++; neighbor[1] = true;}
			else {neighbor[1] = false;}

			if(img[0][r-1][c+1] != Canny::NONEDGE) {n++; neighbor[2] = true;}
			else {neighbor[2] = false;}

			if(img[0][r][c+1] != Canny::NONEDGE) {n++; neighbor[3] = true;}
			else {neighbor[3] = false;}

			if(img[0][r+1][c+1] != Canny::NONEDGE) {n++; neighbor[4] = true;}
			else {neighbor[4] = false;}

			if(img[0][r+1][c] != Canny::NONEDGE) {n++; neighbor[5] = true;}
			else {neighbor[5] = false;}

			if(img[0][r+1][c-1] != Canny::NONEDGE) {n++; neighbor[6] = true;}
			else {neighbor[6] = false;}

			if(img[0][r][c-1] != Canny::NONEDGE) {n++; neighbor[7] = true;} 
			else {neighbor[7] = false;}

			// An isolated edge pixel is not part of a chain.
			if(n == 0) {imgLabeled[0][r][c] = Chain::NOCHAIN;}
			// If only one neighbor is an edge the pixel is definitely
			// the end of a chain.
			else if(n == 1) {imgLabeled[0][r][c] = Chain::ENDPOINT;}  
			// If two neighbors are an edge pixel and those neighbors are
			// touching then the pixel is an edge.  Otherwise, it is an
			// internal point.
			else if(n == 2){ 
				if(neighbor[7] && neighbor[0]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[0] && neighbor[1]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[1] && neighbor[2]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[2] && neighbor[3]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[3] && neighbor[4]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[4] && neighbor[5]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[5] && neighbor[6]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else if(neighbor[6] && neighbor[7]) imgLabeled[0][r][c] = Chain::ENDPOINT;
				else imgLabeled[0][r][c] = Chain::INTERNALPOINT;
			}
			else {imgLabeled[0][r][c] = Chain::INTERNALPOINT;}

			if(imgLabeled[0][r][c] == Chain::ENDPOINT) {
				Point endPoint(c, r);
				endPointList.push_back(endPoint);
			}

	}}

}


void ChainMaker::growEndPoints() {


	for(list<Point>::iterator iter = endPointList.begin(); iter != endPointList.end(); iter++) {

		// End point already belongs to a chain that was grown
		// starting at another end point.
		if(imgChainMap[0][(*iter).y][(*iter).x] != 0) continue;
	                                                                                           	
		Chain chain;
		chain.setOrigin(*iter);
		addPointRecursively(chain, *iter);
		chainList.push_back(chain);
	}

}

 
void ChainMaker::addPointRecursively(Chain& chain, Point& point) {

	chain.addPoint(point);
	imgChainMap[0][point.y][point.x] = 255;
		
	for(long deltaCol = -1; deltaCol <= 1; deltaCol++) {
		for(long deltaRow = -1; deltaRow <= 1; deltaRow++) {

			long c = point.x + deltaCol;
			long r = point.y + deltaRow;

			if(imgChainMap[0][r][c] == 0 && img[0][r][c] != Canny::NONEDGE) {
				Point newPoint(c, r);
				addPointRecursively(chain, newPoint);
			}
	}}
}


void ChainMaker::refineChainMap() {

	imgChainMap = 0;

	for(list<Chain>::iterator iter1 = chainList.begin(); iter1 != chainList.end(); iter1++) {

		if(!chainTest(*iter1)) continue;

		list<Point> pointList = (*iter1).getPointList();

		for(list<Point>::iterator iter2 = pointList.begin(); iter2 != pointList.end(); iter2++) {
             imgChainMap[0][(*iter2).y][(*iter2).x] = 255;
		}
	}
}   


bool ChainMaker::chainTest(Chain& chain) {

	long size = chain.getSize();

	if(size < minChainSize) return false;
     
	chain.fitToLine(); 
     
//	userAgent.getLogFile() << "\nChain: "; chain.write(userAgent.getLogFile());
     
	if(chain.getCorrelation() < minLinearity) return false;
 
	return true;
}
  

IemTImage<unsigned char> ChainMaker::getColorLabelMap() {

	IemTImage<unsigned char> imgColorMap(3, img.rows(), img.cols());
	imgColorMap = 0;

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {

			long v = imgLabeled[0][r][c];
			
			if(v == Chain::ENDPOINT) {imgColorMap[0][r][c] = 255; imgColorMap[1][r][c] = 0; imgColorMap[2][r][c] = 0;}
			else if(v == Chain::INTERNALPOINT) {imgColorMap[0][r][c] = 0; imgColorMap[1][r][c] = 255; imgColorMap[2][r][c] = 0;}
			else if(v == Chain::BRANCHPOINT) {imgColorMap[0][r][c] = 0; imgColorMap[1][r][c] = 0; imgColorMap[2][r][c] = 255;}
	}}

	return imgColorMap;
}

