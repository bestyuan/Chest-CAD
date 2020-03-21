//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CHAINMAKER_H
#define CHAINMAKER_H



#include "Feature.h"
#include "Chain.h"
#include "UserAgent.h"
#include "Point.h"
#include "Iem/Iem.h"
#include <list>


namespace CADX_SEG {  



class ChainMaker {

	private:
	
	// Input image with edge pixels labeled.
	IemTImage<unsigned char> img;
	
	// Image with pixels labeled as ENDPOINT, INTERNALPOINT, etc.
	IemTImage<unsigned char> imgLabeled;
	
	IemTImage<unsigned char> imgChainMap;
	
	list<Point> endPointList;
	
	list<Chain> chainList;
	

	long minChainSize;
	double minLinearity;


	UserAgent userAgent;


	public:     
	
	ChainMaker();

	// Input is an edge map. The output map contains chains.	
	IemTImage<unsigned char> apply(IemTImage<unsigned char>& img);
	
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	IemTImage<unsigned char> getColorLabelMap();

	
	private:
	
	void initialize();

	void findEndPoints();
	
	void growEndPoints();
	
	void addPointRecursively(Chain& chain, Point& point);
	
	void refineChainMap();
	
	// Returns true if chain is long and straight.
	bool chainTest(Chain& chain);





};


} // End namepace


#endif 

