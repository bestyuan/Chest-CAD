//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef ANNOTATE_H
#define ANNOTATE_H


#include "Iem/Iem.h" 
#include "Util.h"   
#include "cadx.h"
#include "Probability.h"
#include "UserAgent.h"
  

namespace CADX_SEG {
  

class Annotator {

	protected:
	IemTImage<unsigned char> imgAnnotated;
	
	UserAgent userAgent;


	public:

	Annotator(IemTImage<short>& img, short minCodeValue,
	 short maxCodeValue, bool invert);
	
	Annotator(IemTImage<unsigned char>& img);
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	void addGroundTruth(Hit& hit);
	
	// Adds a Roi color coded as true positive, false positive,
	// true negative, and false negative.
	void addRoiWithClassAndTruth(Roi& roi, long desciption);
	
	void addCandidates(Can& can, double minProbability = 0.0);
	
	void addMask(IemTImage<unsigned char>& imgMask);

	void setAnnotatedImage(IemTImage<unsigned char>& img) {imgAnnotated = img.copy();}

	void circleCandidates(Can& can, double minProbability = 0.0, int maxAnnotations = 10);
	
	IemTImage<unsigned char> getAnnotatedImage() {return imgAnnotated;}

	protected:

	void makeAnnotatedImage(IemTImage<short>& img, short minCodeValue,
	 short maxCodeValue, bool invert);
	
	static void makeMark(IemTImage<unsigned char>& img, long col, long row, 
	 short r, short g, short b, short size = 3);

	IemTImage<unsigned char> getCircleImage(long size, long radius, long thickness);
	
	bool isInList(list<Probability>& probList, long i);
	
	void writeList(list<Probability>& probList, ostream& s);

	

};


} // End namespace

#endif
