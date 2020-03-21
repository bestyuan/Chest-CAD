//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "SegmenterA.h"
#include "PeakDetector.h"
#include "RegionSupport.h"
#include "BlurFilter.h"
#include "RegionLabel.h"
#include "Clusterer.h"
#include "Watershed.h"
#include "CadxParm.h"
#include "Polyfit.h"
#include "line.h"
#include <list>



using namespace CADX_SEG;

 
SegmenterA::SegmenterA() {
	initialize();
}


SegmenterA::~SegmenterA() {
	if(polynomials != NULL) {
	     for(long i = 0; i < nClusters; i++) delete polynomials[i];
		delete[] polynomials;
	}
}


void SegmenterA::initialize() {
	colSeed = 0;
	rowSeed = 0;
	fail = 0;
	polynomials = NULL;
	minOverlap = 0.50; 
	primaryCluster = 0;
	area = 0;
	regionSupportThreshold = 5;
}

  
IemTImage<unsigned char> SegmenterA::segment(IemTImage<short>& _img, IemTImage<unsigned char>& _imgAnatomyMap,
 long _colSeed, long _rowSeed) {

	char outFileName[1024];

	img = _img;
	imgAnatomyMap = _imgAnatomyMap;
	
	colSeed = _colSeed;
	rowSeed = _rowSeed;

	imgSegMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgSegMap = 0;

//	imgClusterMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
//	imgClusterMap = 0;  

	if(img[0][rowSeed][colSeed] < regionSupportThreshold) {
	     fail = 1; 
	     userAgent.getLogFile() << "\n SegmenterA: img[0][rowSeed][colSeed] < regionSupportThreshold.";
		return imgSegMap;
	}

	// Candidate not in lung field.
	if(imgAnatomyMap[0][rowSeed][colSeed] == 0) {
	     fail = 1; 
	     userAgent.getLogFile() << "\n SegmenterA: Candidate not in lung field.";
		return imgSegMap;
	}
	
	RegionSupport regionSupport;
	regionSupport.setUserAgent(userAgent);
	regionSupport.setMinCodeValue(regionSupportThreshold);
	imgSupportMap = regionSupport.makeMap(img, imgAnatomyMap, colSeed, rowSeed);
	userAgent.writeDebugImage(imgSupportMap, "SegmenterA_Support");

	// Region has no support.
	if(regionSupport.getArea() == 0) {fail = 1; return imgSegMap;}
	
	// Candidate is not in support region.
	if(imgSupportMap[0][rowSeed][colSeed] == 0) {
	     fail = 1; 
	     userAgent.getLogFile() << "\n SegmenterA: Candidate is not in support region.";
		return imgSegMap;
	}
	
	BlurFilter blurFilter(11, 11, 3.0);
	imgBlurred = blurFilter.filter(img);
	userAgent.writeDebugImage(imgBlurred, "SegmenterA_Blurred");
  
	PeakDetector peakDetector;
	peakDetector.setUserAgent(userAgent);
	list<Point> peakList = peakDetector.detect(imgBlurred, imgSupportMap);
	userAgent.writeDebugImage(peakDetector.getAnnotatedImage(), "SegmenterA_Peaks");
	
	if(peakList.size() == 0) {
	     fail = 1; 
	     userAgent.getLogFile() << "\n SegmenterA: peakList.size() == 0.";
		return imgSegMap;
	}

	Watershed watershed;
	watershed.setUserAgent(userAgent);
	watershed.makeZones(imgBlurred, imgSupportMap, peakList);
	nClusters = watershed.getNZones();
	clusters = watershed.getZonePtr();

	if(nClusters == 0) {fail = 1; return imgSegMap;}
	
	primaryCluster = findPrimaryCluster();
	
	// The initial candidate was not not included in a cluster.
	// This could happen if the cv at initial candidate is > than the
	// highest peak. This condition rarely occurs and indicates a
	// non-nodule candidate.
	if(primaryCluster == nClusters) {fail = 1; return imgSegMap;}
	

	clusters[primaryCluster].setStatus(1);

//	imgClusterMap = clusterer.getClusterMap();
//	userAgent.writeDebugImage(imgClusterMap, "SegmenterA_ClusterMap");
	
//	fitClusters();
//	userAgent.writeDebugImage(getClusterFit(), "SegmenterA_ClusterFit");
	
	buildOverlapMatrix();
	
	identifySecondaryClusters();
	  
	makeSegMap(); 
	
//	userAgent.writeDebugImage(imgSegMap, "SegmenterA_SegMap");

	write(userAgent.getLogFile());

	return imgSegMap;
}


long SegmenterA::findPrimaryCluster() {

	long dmin = LONG_MAX;
	long index = 0;

	for(long i = 0; i < nClusters; i++) { 
	
		list<Point> members = clusters[i].getMemberList();

		for(list<Point>::iterator iter = members.begin(); iter != members.end(); iter++) {
			if((*iter).x == colSeed && (*iter).y == rowSeed) {return i;}
		}
	}

	return nClusters;
}


void SegmenterA::makeSegMap() {
	imgSegMap = 0;

	// Add all clusters that satisfy the overlap critera starting
	// from the primary cluster.
	for(long i = 0; i < nClusters; i++) {
		if(clusters[i].getStatus() == 1) addClusterToSegMap(clusters[i]);
	}

	// Make sure that the seg map is connected.
	CADX_SEG::RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgSegMap, 0, 255, 10000);
	IemTImage<unsigned char> imgLabeled = regionLabel.getLabeledImage();
//	userAgent.writeDebugImage(imgLabeled, "SegmenterA_Labeled");

	long label = imgLabeled[0][rowSeed][colSeed];  
	area = 0; 
	
	for(long c = 0; c < img.cols(); c++) {
 		for(long r = 0; r < img.rows(); r++) {
			if(imgLabeled[0][r][c] == label) {imgSegMap[0][r][c] = 255; area++;}
			else imgSegMap[0][r][c] = 0;
	}}  
  
}
 

void SegmenterA::addClusterToSegMap(Cluster& cluster) {

	list<Point> members = cluster.getMemberList();
	list<Point>::iterator iter;

	for(iter = members.begin(); iter != members.end(); iter++) {
		long c = (*iter).x;
		long r = (*iter).y;
		imgSegMap[0][r][c] = 255;
	}
}


void SegmenterA::identifySecondaryClusters() {

	addOverlappingClusters(primaryCluster);
}


void SegmenterA::addOverlappingClusters(long n) {

	for(long i = 0; i < nClusters; i++) {

		if(i == n) continue;
		
		// If cluster is already in list continue.
		if(clusters[i].getStatus() == 1) continue;

		if(matOverlap[i][n] >= minOverlap) {
			userAgent.getLogFile() << "\nSegmenterA::Add cluster " << clusters[i].getNumber() << " to cluster " 
			 << clusters[n].getNumber() << " overlap= " << matOverlap[i][n] << flush;
			clusters[i].setStatus(1);
//			addOverlappingClusters(i);
		}
	}
}


void SegmenterA::buildOverlapMatrix() {

	matOverlap = IemMatrix(nClusters, nClusters);
	matOverlap = 0;

	for(long i = 0; i < nClusters; i++) {
		for(long j = 0; j < nClusters; j++) {
		                                    	
		     if(i == j) {matOverlap[i][j] = 1.0; continue;}

			long xp = clusters[i].getXc();
			long yp = clusters[i].getYc();
			long zp = clusters[i].getZc();
			
			long xs = clusters[j].getXc();
			long ys = clusters[j].getYc();
			long zs = clusters[j].getZc();
			
			Line line(xp, yp, xs, ys);
			long xmin = 0, ymin = 0, zmin = LONG_MAX;

			while(!line.atEnd()) {
		     	Point pt = line.getNextPoint();
          		long x = pt.x;
				long y = pt.y;
				long z = imgBlurred[0][y][x];
				if(z < zmin) {xmin = x; ymin = y; zmin = z;}
			}

			long zavg = (zp + zs) / 2.0;
		
			if(zmin != LONG_MAX) {matOverlap[i][j] = (double)zmin / (double)zavg;}
			else {matOverlap[i][j] = 0.0;}
	}}

}


IemTImage<short> SegmenterA::getClusterFit() {

	IemTImage<short> imgFit(1, img.rows(), img.cols());
	imgFit = 0;

	for(long i = 0; i < nClusters; i++) {

		list<Point> members = clusters[i].getMemberList();
		list<Point>::iterator iter;

		for(iter = members.begin(); iter != members.end(); iter++) {
			long c = (*iter).x;
			long r = (*iter).y;
			imgFit[0][r][c] = polynomials[i]->getValue(c, r);
		}
	}

	return imgFit;
}

 
void SegmenterA::write(ostream& s) {
/*
	for(long i = 0; i < nClusters; i++) {
		s << "\n\nCluster " << clusters[i].getNumber() << endl;
		clusters[i].write(s);
		polynomials[i]->write(s);
	} 
*/

	s << "\n\nClusters:";
	for(long i = 0; i < nClusters; i++) {
	     clusters[i].write(s);;
	}

	s <<  "\n\n Primary cluster= " << clusters[primaryCluster].getNumber();

	s << "\n\nOverlap:"; 
	
	for(i = 0; i < nClusters; i++) {
	     s << "\n Cluster " << clusters[i].getNumber() << ": ";
	     for(long j = 0; j < nClusters; j++) {
			s << clusters[j].getNumber() << "=" << matOverlap[i][j] << ", ";
		}
	} 


	s << endl; 
}
 

 

	

