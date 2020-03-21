//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CLUSTER_H
#define CLUSTER_H



#include "Util.h"
#include "Point.h"
#include "Polynomial.h"
#include <list>
#include "Iem/Iem.h"
#include "UserAgent.h"


namespace CADX_SEG {


class Cluster {

	public:
	long xc;
	long yc;
	long zc;
	
	long number;
	
	long status;

	list<Point> members;

	// Polynomial fit to the cluster.
	Polynomial polynomial;
	
	double fitError;

	// Points at the boundary of the cluster.
	Point minXPnt;
	Point maxXPnt;
	Point minYPnt;
	Point maxYPnt;

	public:
	Cluster();
	
	long getXc() {return xc;}
	long getYc() {return yc;}
	long getZc() {return zc;}
	long getNumber() {return number;}
	long getStatus() {return status;}

	void setXc(long x) {xc = x;}
	void setYc(long y) {yc = y;}
	void setZc(long z) {zc = z;}
	void setNumber(long _number) {number = _number;}
	void setStatus(long _status) {status = _status;}
	void setPolynomial(Polynomial& p) {polynomial = p;}
	void setFitError(double error) {fitError = error;}

	long getNMembers() {members.size();}
	
	Point& getMinXPnt() {return minXPnt;}
	Point& getMaxXPnt() {return maxXPnt;}
	Point& getMinYPnt() {return minYPnt;}
	Point& getMaxYPnt() {return maxYPnt;}
	
	Polynomial& getPolynomial() {return polynomial;}
	
	list<Point>& getMemberList() {return  members;}

	void addPoint(Point& point);
	
	long distanceFrom(long x, long y);
	
	void write(ostream& s);

	protected:

	void initialize();


};





} // Namespace CADX





#endif

