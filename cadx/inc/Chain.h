//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CHAIN_H
#define CHAIN_H



#include "Util.h"
#include "Point.h"
#include <list>


namespace CADX_SEG {


class Chain {
	public:

	// Types of points in a chain.
	static short NOCHAIN;
	static short ENDPOINT;
	static short BRANCHPOINT;
	static short INTERNALPOINT;
	
	// List of points in the chain.
	list<Point> pointList;
	
	Point origin;
	
	double correlation;


	public:
	Chain() {initialize();}
	
	Chain(const Chain& chain) {
		*this = chain;
	}

	Chain& operator=(const Chain& chain) {
		pointList = chain.pointList;
		origin = chain.origin;
		correlation = chain.correlation;
		return *this;
	}
	
	
	void addPoint(Point& point) {	                            	
		pointList.push_back(point);
	}


	list<Point> getPointList() {
		return pointList;
	}
	
	long getSize() {return getPointList().size();}

	void setOrigin(Point& point) {origin = point;}
	
	double getCorrelation() {return correlation;}
	
	void fitToLine();

	void write(ostream& s) {
		s << "origin= " << origin.x << " " << origin.y << ", size= " << pointList.size()
		 << ", correlation= " << getCorrelation();
	}


	protected:

	void initialize();


};




} // Namespace CADX



using namespace CADX_SEG;


ostream& operator << (ostream& s, Chain& chain);


#endif

