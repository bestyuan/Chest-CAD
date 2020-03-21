//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Cluster.h"



using namespace CADX_SEG;


Cluster::Cluster() {
     initialize();
}


void Cluster::addPoint(Point& point) {
	members.push_back(point);
	
	if(point.x < minXPnt.x) minXPnt = point;
	if(point.x > maxXPnt.x) maxXPnt = point;
	if(point.y < minYPnt.y) minYPnt = point;
	if(point.y > maxYPnt.y) maxYPnt = point;
}

	
long Cluster::distanceFrom(long x, long y) {
	long dx = x - xc;
	long dy = y - yc;
	return dx * dx + dy * dy;
}


void Cluster::write(ostream& s) {
	s << "\nnumber= " << number << ", xc= " << xc
	 << ", yc= " << yc << ", zc= " << zc
	 << ", nMembers= " << members.size()
	 << ", status= " << status;
	 
	s << "\nminXPnt= "; minXPnt.write(s); 
	s << ", maxXPnt= "; maxXPnt.write(s);
	s << "\nminYPnt= "; minYPnt.write(s);
	s << ", maxYPnt= "; maxYPnt.write(s);
		 
	s << "\n\nPolynomal Fit";
		 
	polynomial.write(s);
	s << "\nfitError= " << fitError << endl;
}


void Cluster::initialize() {
	xc = 0;
	yc = 0;
	zc = 0;
	number = 1;
	status = 0;
	fitError = 0;
	
	minXPnt.x = LONG_MAX;
	maxXPnt.x = LONG_MIN;
	minYPnt.y = LONG_MAX;
	maxYPnt.y = LONG_MIN;
}






