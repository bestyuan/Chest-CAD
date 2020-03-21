
#include "Boundary.h"



using namespace CADX_SEG;



void Boundary::initialize() {
	boundaryList.clear();
	imgCols = 0;
	imgRows = 0;
	minCol = LONG_MAX;
	maxCol = LONG_MIN;
	minRow = LONG_MAX;
	maxRow = LONG_MIN;
}
 

void Boundary::mapToBoundary(IemTImage<unsigned char>& img) {

	boundaryList.clear();

	imgCols = img.cols();
	imgRows = img.rows();

     long colLimit = imgCols - 1;
	long rowLimit = imgRows - 1;

	for(long r = 0; r <= rowLimit; r++) {
		for(long c = 0; c <= colLimit; c++) {
			
			if(img[0][r][c] != 0) {

				short isBoundary = 0;
			   
				// Region boundary limited by ROI size.
				if(r == 0 || c == 0 || r == rowLimit || c == colLimit) {isBoundary = 1;}
			
				// Real region boundary.
				else if((img[0][r-1][c] == 0) || (img[0][r+1][c] == 0)
				 || (img[0][r][c-1] == 0) || (img[0][r][c+1] == 0)) {

			 		isBoundary = 1;
				}

				if(isBoundary) {
			 		Point point(c, r);
			 		boundaryList.push_back(point);
				}
			}
	}}
	
	setBoundingBox();
}


void Boundary::setBoundingBox() {

	minCol = minRow = LONG_MAX;
	maxCol = maxRow = LONG_MIN;
	
	list<Point>::iterator iter;

	for(iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {
		long col = (*iter).x;
		long row = (*iter).y;

		if(col < minCol) minCol = col;
		if(col > maxCol) maxCol = col;
		if(row < minRow) minRow = row;
		if(row > maxRow) maxRow = row;
	}
}


bool Boundary::isInsideBoundary(long col, long row) {

	if(col < minCol || col > maxCol) return false;
	if(row < minRow || row > maxRow) return false;
	  
	long minC = LONG_MAX, maxC = LONG_MIN;
	long minR = LONG_MAX, maxR = LONG_MIN;
	
	list<Point>::iterator iter;

	for(iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {
		if((*iter).y == row) {
			long c = (*iter).x;
			if(c < minC) minC = c; 
			if(c > maxC) maxC = c;
		}
		if((*iter).x == col) {
			long r = (*iter).y;
			if(r < minR) minR = r;
			if(r > maxR) maxR = r;
		}
	}

	if(col >= minC && col <= maxC && row >= minR && row <= maxR) return true;
				
	return false;
}


bool Boundary::isBoundary(long col, long row) {

	if(col < minCol || col > maxCol) return false;
	if(row < minRow || row > maxRow) return false;
	  
	long minC = LONG_MAX, maxC = LONG_MIN;
	long minR = LONG_MAX, maxR = LONG_MIN;
	
	list<Point>::iterator iter;

	for(iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {

		if((*iter).y == row) {
			long c = (*iter).x;
			if(c < minC) minC = c; 
			if(c > maxC) maxC = c;
		}
		if((*iter).x == col) {
			long r = (*iter).y;
			if(r < minR) minR = r;
			if(r > maxR) maxR = r;
		}
	}

	if(col == minC || col == maxC || row == minR || row == maxR) return true;

	return false;
}


void Boundary::removeInnerBoundary() {

	list<Point>::iterator iter;

	for(iter = boundaryList.begin(); iter != boundaryList.end();) {

		long col = (*iter).x;
		long row = (*iter).y;
		
		if(!isBoundary(col, row)) boundaryList.erase(iter++);
		else ++iter;  
	}

}


IemTImage<unsigned char> Boundary::boundaryToMap() {

	IemTImage<unsigned char> imgBoundary(1, imgRows, imgCols);
	imgBoundary = 0;

	for(long c = 0; c < imgCols; c++) {
		for(long r = 0; r < imgRows; r++) {   
			if(isInsideBoundary(c, r)) imgBoundary[0][r][c] = 255;
	}}
	
	return imgBoundary;
}


IemTImage<unsigned char> Boundary::getBoundaryImage() {

	IemTImage<unsigned char> imgBoundary(1, imgRows, imgCols);
	imgBoundary = 0;

	for(list<Point>::iterator iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {

		long col = (*iter).x;
		long row = (*iter).y;
		
		imgBoundary[0][row][col] = 255;
	}

	return imgBoundary;
}
