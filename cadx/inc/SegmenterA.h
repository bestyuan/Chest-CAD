//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#ifndef SEGMENTER_H
#define SEGMENTER_H


#include "Iem/Iem.h"
#include "Util.h"
#include "UserAgent.h"
#include "Clusterer.h"
#include "Polynomial.h"
#include <list>


namespace CADX_SEG {

  
class SegmenterA {

	protected:

	// The image to be segmented.
	IemTImage<short> img;
	
	IemTImage<short> imgBlurred;

	// A map of anatomy.
	IemTImage<unsigned char> imgAnatomyMap;

	// The region of support for the surface fit.
	IemTImage<unsigned char> imgSupportMap;	
	
	// A map of clusters.
//	IemTImage<unsigned char> imgClusterMap;

	// The region of support for the surface fit.
	IemTImage<unsigned char> imgSegMap;

	long colSeed, rowSeed;
	
	IemMatrix	matOverlap;  
	
	double minOverlap;

	short regionSupportThreshold;

	long nClusters;
	Cluster *clusters;
	
	long primaryCluster; 
	
	long area;

//     list<long> includedClusters;
     
	Polynomial** polynomials;
     
	short fail;

	UserAgent userAgent;

	public:

	SegmenterA();
	
	~SegmenterA();

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	void addOverlappingClusters(long n);
	
	IemTImage<unsigned char> segment(IemTImage<short>& img, IemTImage<unsigned char>& imgAnatomyMap,
	 long colSeed, long rowSeed);
	 
	short hasFailed() {return fail;}
	
	IemTImage<short> getClusterFit();

	void write(ostream& s);


	private:

	void initialize();
	
	long findPrimaryCluster();

	void buildOverlapMatrix();
	
	void makeSegMap();
	
	void addClusterToSegMap(Cluster& cluster);

	void identifySecondaryClusters();

};


} // Namespace CADX_SEG


#endif
