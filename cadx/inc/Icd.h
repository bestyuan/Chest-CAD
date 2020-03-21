//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef ICD_H
#define ICD_H


#include "Iem/Iem.h"
#include "UserAgent.h"
#include "Roi.h"
#include "Point.h"
#include "IcdFeatures.h"
#include <list>
  

namespace CADX_SEG {


class Icd {

	private:
	IemTImage<short> img;
	IemTImage<short> imgribsup;
	IemTImage<unsigned char> imgMask;
	
	IemTImage<short> imgLowRes;
	IemTImage<unsigned char> imgMaskLowRes;
	
	IemTImage<short> imgPreprocessed;
	IemTImage<short> imgPreprocessedribsup;

	IemTImage<short> imgDif0;
	IemTImage<short> imgDif0ribsup;
	IemTImage<short> imgDif1;
	IemTImage<short> imgDif1ribsup;

	// Center or clear lung field.	
	Point clfCenter;

	IemTImage<short> imgProbability;
	IemTImage<short> imgProbabilityribsup;
	
	IcdFeatures icdFeatures0;
	IcdFeatures icdFeatures0ribsup;
	IcdFeatures icdFeatures1;
	IcdFeatures icdFeatures1ribsup;
	
	Roi* roiArray;

	short aggregation;
	long maxCandidates;
	long nCandidates;
	
	long maxScale0Candidates;
	long maxScale1Candidates;
	
	double noduleTemplateHeight;
	double ribTemplateHeight;

	list<Point> peakList;
	
	
	UserAgent userAgent;

	public:

	Icd();
	
	~Icd(){;}
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	long run(IemTImage<short>& img, IemTImage<short>& imgribsup, IemTImage<unsigned char>& imgMask, 
	 Roi* roiArray, long maxCandidates);
	 
	short getAggregation() {return aggregation;}

	IemTImage<short> getPreprocessedImage() {return imgPreprocessed;}
	IemTImage<short> getPreprocessedImageribsup() {return imgPreprocessedribsup;}
	
	void peaksToRoi();


	private:
	
	void initialize();
	
	void makePeakList(list<Point>& peakList0, list<Point>& peakList1);
	

	
};





} // End Namespace







#endif



