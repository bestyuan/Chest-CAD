//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CLUSTERER_H
#define CLUSTERER_H



#include "Util.h"
#include "Point.h"
#include "Polynomial.h"
#include "Cluster.h"
#include <list>
#include "Iem/Iem.h"
#include "UserAgent.h"


namespace CADX_SEG {


class Clusterer {

	private:

	IemTImage<short> img;
	IemTImage<unsigned char> imgSupportMap;
	IemTImage<unsigned char> imgClusterMap;
	IemTImage<short> imgFit;

	long nClusters;
	Cluster *clusters;
	
	double avgFitError; 


	UserAgent userAgent;


	public:
	Clusterer();

	~Clusterer();

	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	void makeClusters(IemTImage<short>& img, IemTImage<unsigned char>& imgSupportMap, list<Point>& prototypes);
	
	void write(ostream& s);

	IemTImage<unsigned char> getClusterMap() {return imgClusterMap;}

	long getNClusters() {return nClusters;}

	Cluster* getClusterPtr() {return clusters;}
	
	IemTImage<short> getFitImage();

	private:
	void initialize();
	
	void assignPointsToClusters();
	
	void makeClusterMap();
	
	void fitClusters();
	
	void refineClusterMap();


};



} // Namespace CADX





#endif

