//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CONVEXHULL_H
#define CONVEXHULL_H   



#include "util.h"
#include "CStackX.h"
#include "Iem/Iem.h" 


namespace CADX_SEG {



class PPoint {

	public:
	long col, row;
	
	PPoint() {col = 0; row = 0;}
	
	PPoint& operator=(const PPoint& rhs) {
		col = rhs.col;
		row = rhs.row;
		return *this;
	}
	
	void write(ostream& s) {
	
		s << "col = " << col << ", row = " << row;
	}
};


class ConvexHull 
{
	private:
	IemTImage<unsigned char> imgConvexHull;
	
	PPoint pointMaxMax;
	PPoint pointMinMin;
	PPoint pointMaxMin;
	PPoint pointMinMax;
	
	long nStrips;
	double stripWidth;
	long area;
	
	PPoint* minRowStrip;
	PPoint* maxRowStrip;
	
	CStackX<PPoint> upperVertex;
	CStackX<PPoint> lowerVertex;



	public:
	ConvexHull(long _nStrips);  
	
	~ConvexHull();  
	
	IemTImage<unsigned char> calculate(IemTImage<unsigned char>& img);
	
	long getArea() {return area;}
	
	void write(ostream& s);
	
	private:
	void initialize();
	
	void findExtrema(IemTImage<unsigned char>& img);
	
	void findStripExtrema(IemTImage<unsigned char>& img);
	
	void findVertex();	
	
	void makeMap();
	
	long getLowerLimit(long c);
	
	long getUpperLimit(long c);

	

};
 




} // End Namespace


using namespace CADX_SEG;


ostream& operator << (ostream& s, PPoint& point);
ostream& operator << (ostream& s, ConvexHull& hull);






#endif
