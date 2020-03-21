//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "ConvexHull.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;


ConvexHull::ConvexHull(long _nStrips) {

	initialize();

	nStrips = _nStrips;
}


ConvexHull::~ConvexHull() {	
	if(minRowStrip != NULL) delete [] minRowStrip;
	if(maxRowStrip != NULL) delete [] maxRowStrip;
}
	

void ConvexHull::initialize() {
	nStrips = 0;
	stripWidth = 0;	
	area = 0;	
	minRowStrip = NULL;
	maxRowStrip = NULL;
}


IemTImage<unsigned char> ConvexHull::calculate(IemTImage<unsigned char>& img) {

	imgConvexHull = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgConvexHull = 0;

	findExtrema(img);

	stripWidth = (double)(pointMaxMin.col - pointMinMin.col) / (double)nStrips;
	
	if(stripWidth < 1) {
	
		stripWidth = 1;
		nStrips = pointMaxMin.col - pointMinMin.col;
	}
	
	findStripExtrema(img);
	findVertex();
	makeMap();

	return imgConvexHull;
}
	
	
void ConvexHull::findExtrema(IemTImage<unsigned char>& img) {

	long minCol = LONG_MAX, maxCol = LONG_MIN;
		
	// Find the min and max columns.
	for(long r = 0; r < img.rows(); r++) {		                                        
		for(long c = 0; c < img.cols(); c++) { 
		
			if(img[0][r][c] != 0) {
			
				if(c < minCol) minCol = c;
				if(c > maxCol) maxCol = c;
			}
	}}    
	
	pointMinMin.col = minCol;
	pointMinMax.col = minCol;
	pointMaxMin.col = maxCol;
	pointMaxMax.col = maxCol;
	
	
	long minColMinRow = LONG_MAX, minColMaxRow = LONG_MIN;
	long maxColMinRow = LONG_MAX, maxColMaxRow = LONG_MIN;

	for(r = 0; r < img.rows(); r++) {	
	
		if(img[0][r][minCol] != 0) {

			if(r < minColMinRow) minColMinRow = r;
			if(r > minColMaxRow) minColMaxRow = r;
		}
		if(img[0][r][maxCol] != 0) {

			if(r < maxColMinRow) maxColMinRow = r;
			if(r > maxColMaxRow) maxColMaxRow = r;
		}
	}  	

	pointMinMin.row = minColMinRow;
	pointMinMax.row = minColMaxRow;
	pointMaxMin.row = maxColMinRow;
	pointMaxMax.row = maxColMaxRow;		
}


void ConvexHull::findStripExtrema(IemTImage<unsigned char>& img) {

	minRowStrip = new PPoint[nStrips + 1];
	maxRowStrip = new PPoint[nStrips + 1];

	minRowStrip[0] = pointMinMin;
	maxRowStrip[0] = pointMinMax;
	
	minRowStrip[nStrips] = pointMaxMin;
	maxRowStrip[nStrips] = pointMaxMax;
	
	double slopeMaxLine = (double)(pointMaxMax.row - pointMinMax.row) / (double)(pointMaxMax.col - pointMinMax.col);
	double slopeMinLine = (double)(pointMaxMin.row - pointMinMin.row) / (double)(pointMaxMin.col - pointMinMin.col);
	
	for(long k = 1; k < nStrips; k++) {
	
		long colStart = k * stripWidth + pointMinMin.col;
		long colEnd = colStart + stripWidth;
		
		minRowStrip[k].col = colStart;
		minRowStrip[k].row = slopeMinLine * (colStart - pointMinMin.col) + pointMinMin.row;
		
		for(long c = colStart; c < colEnd; c++) {
			for(long r = minRowStrip[k].row; r >= 0; r--) {
		
				if(img[0][r][c] != 0 && r < minRowStrip[k].row) {
				
					minRowStrip[k].row = r; 
					minRowStrip[k].col = c;
				}
		}}

		maxRowStrip[k].col = colStart;
		maxRowStrip[k].row = slopeMaxLine * (colStart - pointMinMax.col) + pointMinMax.row;
		
		for(c = colStart; c < colEnd; c++) {
			for(long r = maxRowStrip[k].row; r < img.rows(); r++) {
		
				if(img[0][r][c] != 0 && r > maxRowStrip[k].row) {
			
					maxRowStrip[k].row = r; 
					maxRowStrip[k].col = c;
				}
		}}	
		

	}

}


void ConvexHull::findVertex() {

	upperVertex.push(maxRowStrip[0]);
	
	for(long k = 1; k < nStrips; k++) {
	
		while(upperVertex.getSize() >= 2) {
		
			PPoint p0 = upperVertex.peek(0);
			PPoint p1 = upperVertex.peek(1);
			/*
			
			double slope = (double)(p0.col - p1.col) / (double)(p0.row - p1.row);
			
			long col = slope * (maxRowStrip[k].row - p0.row) + p0.col;
	
			if(maxRowStrip[k].col > col) break;
			
			*/
			
			double slope = (double)(p0.row - p1.row) / (double)(p0.col - p1.col);
			
			long row = slope * (maxRowStrip[k].col - p0.col) + p0.row;
	
			if(maxRowStrip[k].row > row) upperVertex.pop();
			else break;				
		}
		upperVertex.push(maxRowStrip[k]);
	
	}
	
	upperVertex.push(maxRowStrip[nStrips]);
	
	
	
	lowerVertex.push(minRowStrip[0]);
	
	for(k = 1; k < nStrips; k++) {
	
		while(lowerVertex.getSize() >= 2) {
		
			PPoint p0 = lowerVertex.peek(0);
			PPoint p1 = lowerVertex.peek(1);
			
			/*
			
			double slope = (double)(p0.col - p1.col) / (double)(p0.row - p1.row);
			
			long col = slope * (minRowStrip[k].row - p0.row) + p0.col;
	
			if(minRowStrip[k].col > col) break;
			
			*/
			
			double slope = (double)(p0.row - p1.row) / (double)(p0.col - p1.col);
			
			long row = slope * (minRowStrip[k].col - p0.col) + p0.row;
	
			if(minRowStrip[k].row < row) lowerVertex.pop();
			else break;
		}
		lowerVertex.push(minRowStrip[k]);
	
	}
	
	lowerVertex.push(minRowStrip[nStrips]);
}


void ConvexHull::makeMap() {

		
	for(long c = minRowStrip[0].col; c <= minRowStrip[nStrips].col; c++) {
	
		long minRow = getLowerLimit(c);
		long maxRow = getUpperLimit(c);		
		
		for(long r = minRow; r <= maxRow; r++) {
			
			area++;
			imgConvexHull[0][r][c] = 255;
		}
	}		
}

long ConvexHull::getLowerLimit(long c) {

	for(long k = lowerVertex.getSize() - 1; k > 0; k--) {
	
		if(c >= lowerVertex.peek(k).col && c < lowerVertex.peek(k-1).col) break;	
	}
	
	PPoint p0 = lowerVertex.peek(k);
	PPoint p1 = lowerVertex.peek(k-1);

	double slope = (double)(p1.row - p0.row) / (double)(p1.col - p0.col);
			
	return slope * (c - p0.col) + p0.row;
}


long ConvexHull::getUpperLimit(long c) {

	for(long k = upperVertex.getSize() - 1; k > 0; k--) {
	
		if(c >= upperVertex.peek(k).col && c < upperVertex.peek(k-1).col) break;	
	}
	
	PPoint p0 = upperVertex.peek(k);
	PPoint p1 = upperVertex.peek(k-1);

	double slope = (double)(p1.row - p0.row) / (double)(p1.col - p0.col);
			
	return slope * (c - p0.col) + p0.row;
}

/*
IemTImage<unsigned char> ConvexHull::makeMap() {
	
	for(long k = 0; k < nStrips; k++) {
	
		long col1 = minRowStrip[k].col;
		long col2 = minRowStrip[k+1].col;
		
		long minRow = (minRowStrip[k].row + minRowStrip[k+1].row) / 2; 
		long maxRow = (maxRowStrip[k].row + maxRowStrip[k+1].row) / 2; 
		
		for(long c = col1; c < col2; c++) {
			
			for(long r = minRow; r <= maxRow; r++) {
		
				imgConvexHull[0][r][c] = 255;
			}
		}
	}
}
*/

void ConvexHull::write(ostream& s) {

	s << "\n\nConvexHull:";

	s << "\npointMinMin:" << pointMinMin
	 << "\npointMinMax:" << pointMinMax
	 << "\npointMaxMin:" << pointMaxMin
	 << "\npointMaxMax:" << pointMaxMax;
	 
	s << "\nnStrips = " << nStrips << ", stripWidth = " << stripWidth;

	s << "\nmaxRowStrip:"; 	

	for(long k = 0; k <= nStrips; k++) s << "\n" << maxRowStrip[k];
	
	s << "\nUpperVertex:"; 
	for(k = upperVertex.getSize() - 1; k >= 0; k--) {
	
		s << "\n" << upperVertex.peek(k);
	}
	
	s << "\nminRowStrip:"; 	
	for(k = 0; k <= nStrips; k++) s << "\n" << minRowStrip[k];
	
	s << "\nLowerVertex:"; 
	
	for(k = lowerVertex.getSize() - 1; k >= 0; k--) {
	
		s << "\n" << lowerVertex.peek(k);
	}
}


ostream& operator << (ostream& s, PPoint& point) {point.write(s); return s;}
ostream& operator << (ostream& s, ConvexHull& hull) {hull.write(s); return s;}
	








