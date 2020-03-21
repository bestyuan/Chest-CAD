//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Decomposition.h"
#include "Iem/IemImageIO.h"
#include "ConSeg.h"


Decomposition::Decomposition() {


}


IemTImage<unsigned char> Decomposition::decompose(IemTImage<unsigned char>& _imgMap,
 long col, long row) {
                     	
	col0 = col;
	row0 = row;
	
	imgMap = _imgMap;

	long r, c;

	ConSeg conSeg;
	
	long rows = imgMap.rows();
	long cols = imgMap.cols();
	
	IemTPlane<short> imgIn(rows, cols);
	
	for(r = 0; r < rows; r++) {
		for(c = 0; c < cols; c++) {
            imgIn[r][c] = imgMap[0][r][c];
	}}

     imgDecomposed = conSeg.contour_segmentation(imgIn);

	return selectRegion();
}

IemTImage<unsigned char> Decomposition::selectRegion() {

	long selectedLabel = 0;
	
	long cols = imgDecomposed.cols();
	long rows = imgDecomposed.rows();
	
	double d2min = cols * cols + rows * rows;

	for(long c = 0; c < cols; c++) {
		for(long r = 0; r < rows; r++) {
			
			if(imgMap[0][r][c] == 0) continue; 

			double d2 = (c - col0) * (c - col0) + (r - row0) * (r - row0);
			
			if(d2 < d2min) {d2min = d2; selectedLabel = imgDecomposed[r][c];}
	}} 
	

	IemTImage<unsigned char> imgSelected(1, rows, cols);

	for(long c = 0; c < cols; c++) {
		for(long r = 0; r < rows; r++) {
			if(imgDecomposed[r][c] == selectedLabel) imgSelected[0][r][c] = 255;
			else imgSelected[0][r][c] = 0;
	}}  
	
	return imgSelected;
}


