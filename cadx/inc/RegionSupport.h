//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#ifndef  REGIONSUPPORT_H
#define REGIONSUPPORT_H


#include "Iem/Iem.h"    
#include "Util.h"
#include "UserAgent.h"


namespace CADX_SEG {

  
class RegionSupport {

	protected:

	// The image to be segmented.
	IemTImage<short> img;

	// A map of anatomy.
	IemTImage<unsigned char> imgAnatomyMap;

	// The region of support for the surface fit.
	IemTImage<unsigned char> imgSupportMap;
	
	IemTImage<unsigned char> imgLabeled;

	long colSeed, rowSeed;

	short erosionKernelSize;
	short aggregation;

	// Minimum code value of input image that will be included in the segment.
	short minCodeValue;
	
	long area;

	UserAgent userAgent;

	public:

	RegionSupport();
	
	void setMinCodeValue(short v) {minCodeValue = v;}

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	IemTImage<unsigned char> makeMap(IemTImage<short>& img, IemTImage<unsigned char>& imgAnatomyMap, 
	 long colSeed, long rowSeed);
	 
	long getArea() {return area;}

	private:

	void initialize();

	void initializeMap();

	// Erode bridges between regions.
	void erodeMap();

     // Label connected regions.
	void labelMap();

	void pruneMap();

	void dilateMap();

};


} // Namespace CADX_SEG


#endif
