//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef BOUNDARY_H
#define BOUNDARY_H



#include "Util.h"
#include "Point.h"
#include "Iem/Iem.h"
#include <list>


namespace CADX_SEG {


class Boundary {

	public:
	list<Point> boundaryList;

	// The size of the image that contains the boundary.
	long imgCols, imgRows;
	
	// The bounding box for the boundary.
	long minCol, maxCol;
	long minRow, maxRow;



	public:
	Boundary() {initialize();}
	
	list<Point>& getList() {return boundaryList;}
	
	long getMinCol() {return minCol;}
	long getMaxCol() {return maxCol;}
	long getMinRow() {return minRow;}
	long getMaxRow() {return maxRow;}

	void mapToBoundary(IemTImage<unsigned char>& img);
	
	IemTImage<unsigned char> boundaryToMap();
	
	bool isInsideBoundary(long col, long row);
	bool isBoundary(long col, long row);
	
	void removeInnerBoundary();
	
	void setBoundingBox();
	
	void addPoint(Point& point) {
		boundaryList.push_back(point);
	}
	
	IemTImage<unsigned char> getBoundaryImage();
	
	Boundary& operator=(const Boundary& rhs) {
		boundaryList = rhs.boundaryList;
		imgCols = rhs.imgCols;
		imgRows = rhs.imgRows;
		minCol = rhs.minCol;
		maxCol = rhs.maxCol;
		minRow = rhs.minRow;
		maxRow = rhs.maxRow;
		return *this;
	}


	void write(ostream& s) {
	//	s << "\nslope= " << slope;
	}


	protected:

	void initialize();



};




} // Namespace CADX





#endif

