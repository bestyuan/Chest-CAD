//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef LINE_H
#define LINE_H



#include "Util.h"
#include "Point.h"



namespace CADX_SEG {


class Line {
	public:
	Point pt0, pt1;
	double w;
	bool end;



	public:
	Line() {initialize();}

	Line(double x0, double y0, double x1, double y1) {
		initialize();

		pt0.x = x0;
		pt0.y = y0;
		pt1.x = x1;
		pt1.y = y1;
	}
	
	bool atEnd() {return end;}

 
	
	Point getNextPoint() {
	     Point curPt;
	     
	     curPt.x = (1.0 - w) * pt0.x + w * pt1.x;
	     curPt.y = (1.0 - w) * pt0.y + w * pt1.y;
	     
	     w += 0.05;
	     if(w >= 1.0) end = true;

		return curPt;
	}


	void write(ostream& s) {
	//	s << "\nslope= " << slope;
	}


	protected:

	void initialize() {
		end = false;
		w = 0;
	}


};




} // Namespace CADX





#endif

