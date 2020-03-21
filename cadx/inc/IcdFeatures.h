//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef ICDFEATURES_H
#define ICDFEATURES_H


#include "Iem/Iem.h"
#include "UserAgent.h"
#include "Roi.h"
#include "Point.h"
#include "MhacFeature.h"
#include <list>
  

namespace CADX_SEG {


class IcdFeatures {

	private:
	IemTImage<short> img;
	IemTImage<unsigned char> imgMask;
	IemTImage<short> imgDif;

	long nMhacAggregations;
	long nMhacRadius;
	long nMhacBins;
	long minInterPeakDistance;

	list<Point> peakList;
	
	MhacFeature mhacFeature;
	MhacFeature mhacFeature2;

	UserAgent userAgent;

	public:

	IcdFeatures();
	
	~IcdFeatures(){;}
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	void calculate(IemTImage<short>& img, IemTImage<short>& imgDif,
	 IemTImage<unsigned char>& imgMask);

	void setNMhacAggregations(long v) {nMhacAggregations = v;}
	void setNMhacRadius(long v) {nMhacRadius = v;}
	void setMinInterPeakDistance(long d) {minInterPeakDistance = d;}
	
	MhacFeature& getMhacFeature() {return mhacFeature;}
	
	void writeFeatureInfo(ostream& s, long col, long row);

     list<Point> getPeakList() {return peakList;}
     
     void addToRoi(Roi& roi);
	 void addToRoiribsup(Roi& roi);

	private:
	
	void initialize();
	


	
};





} // End Namespace







#endif



