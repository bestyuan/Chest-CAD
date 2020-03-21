//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CANNY_H
#define CANNY_H



#include "util.h"
#include "Iem/Iem.h"
#include "UserAgent.h"


namespace CADX_SEG {



class Canny {

	public:
	static short NONEDGE;
	static short HORIZONTAL;
	static short VERTICAL;
	static short POSDIAGONAL;
	static short NEGDIAGONAL;

	private:
	IemTImage<short> img;
	IemTImage<unsigned char> imgMask;
	IemTImage<short> imgBlurred;
	IemTImage<short> imgEdge;
	IemTImage<unsigned char> imgEdgeMap;
	IemTImage<unsigned char> imgNonMaxSupEdgeMap;
	IemTImage<unsigned char> imgHighThresEdgeMap;
	
	
	// The fractional penetration into the high end
	// of the edge magnitude histogram used to determine
	// highThreshold.
	double edgeMagHistPen;
	
	double lowThreshold;
	double highThreshold;

	UserAgent userAgent;


	public:
	Canny();
	
	~Canny();

	IemTImage<short> getEdgeImage() {return imgEdge;}
	
	static IemTImage<unsigned char> getColorEdgeMap(IemTImage<unsigned char>& imgEdge);

	IemTImage<unsigned char> calculate(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);

	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	private:
	void initialize();
	
	void makeEdgeMap();

	void nonMaxSuppression();
	
	void applyHighThreshold();
	
	void applyHysteresis();

	void calcThresholds();
	
	static short getOrientation(double angle);
	
	void followEdge(long col, long row);


	


	

};


} // End Namespace



#endif
