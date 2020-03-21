//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "PeakDetector.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemTImageIter.h"

using namespace CADX_SEG;



PeakDetector::PeakDetector() {
	initialize();
}


PeakDetector::~PeakDetector() {
}


void PeakDetector::initialize() {
	minInterPeakDistance = 10;
	minPeakHeight = 20;
}

 
list<Point> PeakDetector::detect(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask) {
	img = _img.copy();
	imgMask = _imgMask;
	
	peaks.clear();
 
	findPeaks(); 

/*
	userAgent.getLogFile() << "\n\nPeaks:";
	write(userAgent.getLogFile());
*/	
	cullPeaks();
//	userAgent.getLogFile() << "\n\nFinal Peaks culled:";
//	write(userAgent.getLogFile());
	  
	return peaks;
}
    

IemTImage<short> PeakDetector::getAnnotatedImage() {

	IemTImage<short> imgAnnotated(3, img.rows(), img.cols());
	
	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			short v = img[0][r][c];
			imgAnnotated[0][r][c] = v;
			imgAnnotated[1][r][c] = v;
			imgAnnotated[2][r][c] = v;
	}}
	
	list<Point>::iterator iter;
	
	for (iter = peaks.begin(); iter != peaks.end(); iter++) {
		long col = (*iter).x;
		long row = (*iter).y;

		imgAnnotated[0][row][col] = SHRT_MAX;
		imgAnnotated[1][row][col] = 0;
		imgAnnotated[2][row][col] = 0;
	}

	return imgAnnotated;
}


void PeakDetector::findPeaks() {

	IemTImageIter<short> center(img);

	IemTImageIter<short> north(img);
	IemTImageIter<short> south(img);
	IemTImageIter<short> east(img);
	IemTImageIter<short> west(img);

	IemTImageIter<short> northEast(img);
	IemTImageIter<short> southEast(img);
	IemTImageIter<short> northWest(img);
	IemTImageIter<short> southWest(img);

	IemTImageIter<unsigned char> centerMask (imgMask);

	long maxCol = img.cols() - 1;
	long maxRow = img.rows() - 1;

	for(long r = 1; r < maxRow; r++) {
	           
		centerMask.setPos(r,1);  

		center.setPos(r,1);
		north.setPos(r-1,1);
		south.setPos(r+1,1);
		east.setPos(r,2);
		west.setPos(r,0);
		northEast.setPos(r-1,2);
		southEast.setPos(r+1,2);
		northWest.setPos(r-1,0);
		southWest.setPos(r+1,0);
 
		for(long c = 1; c < maxCol; c++) {

			if(centerMask.value(0) > 0) {

			     short v = center.value(0);
			     
			     if(v >= minPeakHeight 
				 && v >= north.value(0) && v >= south.value(0)
			      && v >= east.value(0) && v >= west.value(0)
				 && v >= northEast.value(0) && v >= southEast.value(0)
				 && v >= northWest.value(0) && v >= southWest.value(0)) {
			                                                
/*
			          userAgent.getLogFile() << "\n Peak " << ", at col=" << c << ", row=" << r
			           << ", center=" << center.value(0) << ", north=" << north.value(0)
			           << ", south=" << south.value(0) << ", east=" << east.value(0)
			           << ", west=" << west.value(0);
*/

			          Point peak(c, r, v);
			          peaks.push_back(peak);
			      }
			}
			
			centerMask.colInc();
			center.colInc();
			north.colInc();
			south.colInc();
			east.colInc();
			west.colInc();
			northEast.colInc();
			southEast.colInc();
			northWest.colInc();
			southWest.colInc();
		}
	}

}


void PeakDetector::cullPeaks() {

	// Make sure state of all peaks is initially 0.
	for(list<Point>::iterator iter0 = peaks.begin(); iter0 != peaks.end(); iter0++) {
		(*iter0).state = 0;
	}

	long dmin2 = minInterPeakDistance * minInterPeakDistance;
	
	list<Point>::iterator iter1;
	list<Point>::iterator iter2;

	list<Point>::iterator stop = peaks.end();
	stop--;

	for(iter1 = peaks.begin(); iter1 != stop; iter1++) {
	                                                          	
		long col1 = (*iter1).x;
		long row1 = (*iter1).y;
		
		for(iter2 = iter1, iter2++; iter2 != peaks.end(); iter2++) {

			// Peak was already elliminated;
			if((*iter2).state == 1) continue;

			long col2 = (*iter2).x;
			long row2 = (*iter2).y;

			long dCol = col2 - col1;
			long dRow = row2 - row1;

			long d2 = dCol * dCol + dRow * dRow;

			if(d2 < dmin2) {
				if((*iter1).z > (*iter2).z) (*iter2).state = 1;
				else (*iter1).state = 1;
			}
		}
	}
	
	list<Point>::iterator iter;

	for (iter = peaks.begin(); iter != peaks.end();) {
		if((*iter).state == 1) peaks.erase(iter++);
		else ++iter;
	}

} 


void PeakDetector::write(ostream& s) {
	s << "\n\nPeakDetector:";

	for(list<Point>::iterator iter = peaks.begin(); iter != peaks.end(); iter++) {
	     s << "\n\t"; (*iter).write(s);
	}
}

