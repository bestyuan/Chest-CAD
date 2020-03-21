//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H



#include "util.h"
#include "roi.h"
#include "Iem/Iem.h"


class Decomposition {

	private:
	IemTImage<unsigned char> imgMap;
	IemTPlane<short> imgDecomposed;
	
	long col0;
	long row0;


	public:

	Decomposition();

	IemTImage<unsigned char> decompose(IemTImage<unsigned char>& imgMap, long col, long row);
	
	IemTImage<short> getDecomposedMap() {return imgDecomposed;}
	
	private:
	
	IemTImage<unsigned char> selectRegion();


	

};



#endif
