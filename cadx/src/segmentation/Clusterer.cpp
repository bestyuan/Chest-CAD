//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Clusterer.h"
#include "Polyfit.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemTImageIter.h"

using namespace CADX_SEG;



Clusterer::Clusterer() {
	initialize();
}


Clusterer::~Clusterer() {
	if(clusters != NULL) delete[] clusters;
}


void Clusterer::initialize() {
	nClusters = 0;
	clusters = NULL;
	avgFitError = 0;
}

 
void Clusterer::makeClusters(IemTImage<short>& _img, IemTImage<unsigned char>& _imgSupportMap, list<Point>& prototypes) {

	img = _img;
	imgSupportMap = _imgSupportMap;
	
	imgClusterMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgClusterMap = 0;

	nClusters = prototypes.size();
	 
	clusters = new Cluster[nClusters]; 

	list<Point>::iterator iter;
   
	long i = 0;
	for(iter = prototypes.begin(); iter != prototypes.end(); iter++, i++) {
		clusters[i].setXc((*iter).x);
		clusters[i].setYc((*iter).y);
		clusters[i].setZc((*iter).z);
		clusters[i].setNumber(i+1);   
	} 
 
	assignPointsToClusters();

	makeClusterMap();
	userAgent.writeDebugImage(imgClusterMap, "Clusterer_map0");

	fitClusters();
	getFitImage();
	userAgent.writeDebugImage(imgFit, "Clusterer_Fit0");
	
	refineClusterMap(); 

	makeClusterMap();
	userAgent.writeDebugImage(imgClusterMap, "Clusterer_map1");
}
     
   
void Clusterer::assignPointsToClusters() {

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
		                                     	
			if(imgSupportMap[0][r][c] != 0) {

				long dMin = LONG_MAX;
				long indexMin = 0;

				for(long i = 0; i < nClusters; i++) {
					long d = clusters[i].distanceFrom(c, r);
					if(d < dMin) {dMin = d; indexMin = i;}
				}
				
				Point point(c, r, img[0][r][c]);
				clusters[indexMin].addPoint(point);
			}
	}}

}


void Clusterer::makeClusterMap() {

	for(long i = 0; i < nClusters; i++) {
	                                    	
	     long n = clusters[i].getNumber();
	         
		list<Point> members = clusters[i].getMemberList();
		list<Point>::iterator iter;

		for(iter = members.begin(); iter != members.end(); iter++) {
			long c = (*iter).x;
			long r = (*iter).y;
			imgClusterMap[0][r][c] =  n;
		}

	}
} 


void Clusterer::fitClusters() {

	PolyFit polyFit(4);
	
	avgFitError = 0;

	for(long i = 0; i < nClusters; i++) {
		polyFit.setMaskValue(clusters[i].getNumber());
		polyFit.calcFit(img, imgClusterMap, clusters[i].getXc(), clusters[i].getYc());
		clusters[i].setPolynomial(polyFit.getPolynomial());
		clusters[i].setFitError(polyFit.getFitError());
		avgFitError += polyFit.getFitError();
	}
	
	avgFitError /= (double)nClusters;
	
	userAgent.getLogFile() << "\nClusterer::avgFitError= " << avgFitError;
}


IemTImage<short> Clusterer::getFitImage() {

	imgFit = IemTImage<short>(1, img.rows(), img.cols());
	imgFit = 0;

	for(long i = 0; i < nClusters; i++) {
		list<Point> members = clusters[i].getMemberList();

		for(list<Point>::iterator iter = members.begin(); iter != members.end(); iter++) {
			long c = (*iter).x;
			long r = (*iter).y;
			
			imgFit[0][r][c] = clusters[i].getPolynomial().getValue(c, r);
		}
	}
	
	return imgFit;
}


void Clusterer::refineClusterMap() {

	for(long i = 0; i < nClusters; i++) {
	                                    	
	     long n = clusters[i].getNumber();
	         
		list<Point> members = clusters[i].getMemberList();
		list<Point>::iterator iter;

		for(iter = members.begin(); iter != members.end();) {
			long c = (*iter).x;
			long r = (*iter).y;
			long z = (*iter).z;
			
			short minDelta = SHRT_MAX;
			long minIndex = 0;

			for(long j = 0; j < nClusters; j++) {
				short v = clusters[j].getPolynomial().getValue(c, r);
				short delta = abs(z - v);
				if(delta < minDelta) {minDelta = delta; minIndex = j;}
			}
			
			if(minIndex != i) {
				userAgent.getLogFile() << "\nClusterer:: cluster= " << clusters[i].getNumber() << ", at c= " << c
			 	 << " r= " << r << " z= " << z << ", reassign to cluster= " << clusters[minIndex].getNumber();
			 	 
			 	 members.erase(iter++);
			 	 
			 	 Point point(c, r, z);
			 	 clusters[minIndex].getMemberList().push_back(point);
			}
			else iter++;
		}

	}

}


