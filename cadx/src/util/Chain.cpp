//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Chain.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;


short Chain::NOCHAIN = 0;
short Chain::ENDPOINT = 1;
short Chain::BRANCHPOINT = 2;
short Chain::INTERNALPOINT = 3;


void Chain::initialize() {
	correlation = 0.0;
}


void Chain::fitToLine() {

	long sumX = 0, sumY = 0, sumXX = 0, sumYY = 0, sumXY = 0;
	
	long n = getSize();

	for(list<Point>::iterator iter = pointList.begin(); iter != pointList.end(); iter++) {
		long x = (*iter).x;
		long y = (*iter).y;

		sumX += x;
		sumY += y;
		sumXX += x * x;
		sumYY += y * y;	
		sumXY += x * y;
	}
	
	double a = n * sumXY - sumX * sumY;
	double b = n * sumXX - sumX * sumX;
	double c = n * sumYY - sumY * sumY;

	correlation = a / (sqrt(b) * sqrt(c));
	if(correlation < 0.0) correlation = -correlation; 
}



ostream& operator << (ostream& s, Chain& chain) {
	chain.write(s);
	return s;
}


















