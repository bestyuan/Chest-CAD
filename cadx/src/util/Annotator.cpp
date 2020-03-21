//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Annotator.h"
#include "histogram.h"
#include "Roi.h"
#include "Point.h" 
#include "Probability.h"
#include <list>


using namespace CADX_SEG;

 
Annotator::Annotator(IemTImage<short>& img, short minCodeValue, short maxCodeValue, bool invert) {
	makeAnnotatedImage(img, minCodeValue, maxCodeValue, invert);
}


Annotator::Annotator(IemTImage<unsigned char>& img) {

	imgAnnotated = IemTImage<unsigned char>(3, img.rows(), img.cols()); 

	imgAnnotated[0] = img[0];
	imgAnnotated[1] = img[0];
	imgAnnotated[2] = img[0]; 
}


void Annotator::makeAnnotatedImage(IemTImage<short>& img, short minCodeValue, 
 short maxCodeValue, bool invert) {
	
	imgAnnotated = IemTImage<unsigned char>(3, img.rows(), img.cols()); 
		
	for(long r = 0; r < img.rows(); r++) {		                                        
		for(long c = 0; c < img.cols(); c++) { 
		
			unsigned char cv; 
			
			if(invert) {cv = 255 * (1.0 - (double)img[0][r][c] / (double)maxCodeValue);}
			else {cv = 255 * (double)img[0][r][c] / (double)maxCodeValue;}

			imgAnnotated[0][r][c] = cv;
			imgAnnotated[1][r][c] = cv;
			imgAnnotated[2][r][c] = cv;  
		}
	}	

}
 

void Annotator::addMask(IemTImage<unsigned char>& imgMask) {

	for(long r = 0; r < imgAnnotated.rows(); r++) {
		for(long c = 0; c < imgAnnotated.cols(); c++) {
		
			if(imgMask[0][r][c] == 0) {
				imgAnnotated[0][r][c] *= 0.75;
				imgAnnotated[1][r][c] *= 0.75;
				imgAnnotated[2][r][c] *= 0.75;
			}
	}}
 
}


void Annotator::addGroundTruth(Hit& hit) {

	short r, g, b;

	for(long i = 0; i < hit.getNumNodules(); i++) {   
		
		if(hit.getNoduleStatus(i) == Util::GROUNDTRUTH_DEFINITE) {
			r = 255; g = 0; b = 0;
		}
		else if(hit.getNoduleStatus(i) == Util::GROUNDTRUTH_PROBABLE) {
			r =  0; g = 255; b = 0;
		} 
		else if(hit.getNoduleStatus(i) == Util::GROUNDTRUTH_POSSIBLE) {
			r =  0; g = 0; b = 255;
		}  

		for(long j = 0; j < hit.getNOutlinePoints(i); j++) {  
		
			long col = hit.getOutlineCol(i, j);
			long row = hit.getOutlineRow(i, j);
		
			makeMark(imgAnnotated, col, row, r, g, b, 3);
	  }

   }  // Next hit
			
}


void Annotator::addCandidates(Can& can, double minProbability) {

	short r, g, b;
	
	can.seekgToFirstCandidate();

	for(long i = 0; i < can.getNCandidates(); i++) {
	                
		Roi roi;
		can.readNextCandidate(roi);
		
		if(roi.getProbability() < minProbability) continue;
	   
		if(roi.getNGroups() > 0) b = roi.getGroup(0);
		else b = 0;

		if(roi.getGroundtruth() == Util::GROUNDTRUTH_DEFINITE) {
			r = 255; g = 255;
		}
		else if(roi.getGroundtruth() == Util::GROUNDTRUTH_PROBABLE) {
			r = 255; g = 255;
		}
		else if(roi.getGroundtruth() == Util::GROUNDTRUTH_POSSIBLE) {
			r = 255; g = 255;
		} 
 		else {
			r = 255; g = 255;
		}

		list<Point> boundaryList = roi.getBoundaryList();

		for(list<Point>::iterator iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {
			long col = roi.getAbsCol((*iter).x);
			long row = roi.getAbsRow((*iter).y);
			makeMark(imgAnnotated, col, row, r, g, b, 3);
		}
	  
		// Mark the candidate location with its index number.
		long index = roi.getIndex();
		if(index > 255) index = 255;
		makeMark(imgAnnotated, roi.getAbsCandidateCol(), roi.getAbsCandidateRow(), index, 255, 255, 4);

   }  // Next candidate

}


void Annotator::addRoiWithClassAndTruth(Roi& roi, long desciption) {

	short r = 0, g = 0, b = 0;


	if(desciption == Util::TRUE_POSITIVE) {
			r = 255; g = 255; b = 255;
	}
	else if(desciption == Util::FALSE_POSITIVE) {
			r = 0; g = 255; b = 255;
	}
	else if(desciption == Util::TRUE_NEGATIVE) {
			r = 255; g = 255; b = 0;
	}
 	else if(desciption == Util::FALSE_NEGATIVE) {
			r = 255; g = 0; b = 255;
	}

	list<Point> boundaryList = roi.getBoundaryList();

	for(list<Point>::iterator iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {
		long col = roi.getAbsCol((*iter).x);
		long row = roi.getAbsRow((*iter).y);
		makeMark(imgAnnotated, col, row, r, g, b);
	}
	  
	// Mark the candidate location with its index number.
	long index = roi.getIndex();
	if(index > 255) index = 255;
	makeMark(imgAnnotated, roi.getAbsCandidateCol(), roi.getAbsCandidateRow(), index, index, index);
}


void Annotator::makeMark(IemTImage<unsigned char>& img, long col, long row, 
 short r, short g, short b, short size) {

	long d = size;
   
	long rows =  img.rows();
	long cols =  img.cols();

	for(long cc = col - d; cc <= col + d; cc++) {
     	for(long rr = row - d; rr <= row + d; rr++) {

			if(rr < 0 || rr >= rows) continue;
			if(cc < 0 || cc >= cols) continue;
			
			img[0][rr][cc] = r; 
			img[1][rr][cc] = g;
			img[2][rr][cc] = b;
	}}

}


void Annotator::circleCandidates(Can& can, double minProbability, int maxAnnotations) {

	short r, g, b;
	long size = 256, radius = 115, thickness = 4;

	IemTImage<unsigned char> imgCircle = getCircleImage(size, radius, thickness);
	
	can.seekgToFirstCandidate();

	// Make a list of candidate probabilities
	list<Probability> probList;
	
	for(long i = 0; i < can.getNCandidates(); i++) {
		Roi roi;
		can.readNextCandidate(roi);
		if(roi.getProbability() >= minProbability) {
			Probability probability(i, roi.getProbability());
			probList.push_back(probability);
		}
	}

	userAgent.getLogFile() << "\n\n Probability list";	
	writeList(probList, userAgent.getLogFile());

	// Sort the list in order of decreasing probability.
	probList.sort();
	probList.reverse();
	
	userAgent.getLogFile() << "\n\n Probability list sorted";
	writeList(probList, userAgent.getLogFile());

	// Reduce the size of the list to maxAnnotations.
	if(probList.size() > maxAnnotations) {
		probList.resize(maxAnnotations);
	}
	
	userAgent.getLogFile() << "\n\n Probability list truncated";
	writeList(probList, userAgent.getLogFile());

	can.seekgToFirstCandidate();

	for(i = 0; i < can.getNCandidates(); i++) {

		Roi roi;
		can.readNextCandidate(roi);
		
		if(!isInList(probList, i)) continue;
		
		long col0 = roi.getAbsCandidateCol();
		long row0 = roi.getAbsCandidateRow();

		long cols = imgAnnotated.cols();
		long rows = imgAnnotated.rows();

		long center = size / 2;
		
		for(long c = 0; c < size; c++) {
			for(long r = 0; r < size; r++) {	
			
				if(imgCircle[0][r][c] != 0) {
				                            	
				     long cc = col0 - (c - center);
				     long rr = row0 - (r - center);
				     
				     if(cc < 0 || rr < 0) continue;
				     if(cc >= cols || rr >= rows) continue;

					imgAnnotated[0][rr][cc] = 255;
					imgAnnotated[1][rr][cc] = 255;
					imgAnnotated[2][rr][cc] = 255;
				}
			
		}}


   }  // Next candidate


}


IemTImage<unsigned char> Annotator::getCircleImage(long size, long radius, long thickness) {

	IemTImage<unsigned char> imgCircle = IemTImage<unsigned char>(1, size, size);
	imgCircle = 0;
	
	long center = size / 2;

	long d1 = (radius - thickness) * (radius - thickness);
	long d2 = radius * radius;

	for(long c = 0; c < size; c++) {
		for(long r = 0; r < size; r++) {  
		                               	
			long dc = (c - center); 
			long dr = (r - center);	

			long d = dr * dr + dc * dc;
			
			if(d > d1 && d < d2) imgCircle[0][r][c] = 255;
	}}

	return imgCircle;
}


bool Annotator::isInList(list<Probability>& probList, long i) {

	for(list<Probability>::iterator iter = probList.begin(); iter != probList.end(); iter++) {
		if(i == (*iter).index) {return true;}
	}

	return false;
}


void Annotator::writeList(list<Probability>& probList, ostream& s) {
	for(list<Probability>::iterator iter = probList.begin(); iter != probList.end(); iter++) {
		s << "\n" << (Probability)(*iter);
	}
}



