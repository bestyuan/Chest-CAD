//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef POINT_H
#define POINT_H



#include "Util.h"



namespace CADX_SEG {


class Point {

	public:
	long x;
	long y;
	short z; 
	short state;
	long source;
	
	public:
	Point() {initialize();}

	Point(long _x, long _y, short _z = 0, short _state = 0, long _source = 0) {
		initialize();
		x = _x;
		y = _y;
		z = _z;
		state = _state;
		source = _source;
	}
	
	Point(const Point& p) {
		*this = p;
	}

	Point& operator=(const Point& p) {
		x = p.x;
		y = p.y;
		z = p.z;
		state = p.state;
		source = p.source;
	          
		return *this;
	}
	
	
	bool operator<(const Point& p) {
		if(z < p.z) return true;
		return false;
	}


	bool operator>(const Point& p) {
		if(z > p.z) return true;
		return false;
	}


	bool operator==(const Point& p) {
		if(z == p.z) return true;
		return false;
	}
	

	void write(ostream& s) {
		s << "x= " << x << ", y= " << y << ", z= " << z 
		 << ", state= " << state << ", source= " << source;
	}


	protected:

	void initialize() {
		x = 0;
		y = 0;
		z = 0;
		state = 0;
		source = 0;
	}

};



} // Namespace CADX





#endif

