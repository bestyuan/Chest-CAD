//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef WATERSHED_H
#define WATERSHED_H



#include "Util.h"
#include "Point.h"
#include "Cluster.h"
#include <list>
#include "Iem/Iem.h"
#include "UserAgent.h"


namespace CADX_SEG {


class Watershed {

	private:
	IemTImage<short> img;
	IemTImage<unsigned char> imgSupportMap;
	IemTImage<unsigned char> imgZoneMap;
	IemTImage<short> imgAltitude;
	
	short zMax;

	long nZones;
	Cluster *zones;
	
	long nHistBins;
	double histScale;

	list<Point> *hist;
	
	list<Point> *boundary;

	UserAgent userAgent;


	public:
	Watershed();

	~Watershed();

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	void makeZones(IemTImage<short>& img, IemTImage<unsigned char>& imgSupportMap, list<Point>& peaks);

	long getNZones() {return nZones;}
	Cluster* getZonePtr() {return zones;}
	
	IemTImage<unsigned char> getZoneMap() {return imgZoneMap;}

	private:
	void initialize();
	
	void buildHistogram();
	
	void add(long k);
	
	long findClosestZone(long x, long y);

	void updateBoundarys();


};



} // End namespace





#endif

