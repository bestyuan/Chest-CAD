//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef PEAKDETECTOR_H
#define PEAKDETECTOR_H



#include "Util.h"
#include "Point.h"
#include <list>
#include "Iem/Iem.h"
#include "UserAgent.h"


namespace CADX_SEG {


class PeakDetector {

	private:

	IemTImage<short> img;

	IemTImage<unsigned char> imgMask;
	
	list<Point> peaks;
	
	long minInterPeakDistance;
	short minPeakHeight;
	
	UserAgent userAgent;


	public:
	PeakDetector();

	~PeakDetector();

	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	list<Point> detect(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);

	void write(ostream& s);

	IemTImage<short> getAnnotatedImage();

	void setMinInterPeakDistance(long d) {minInterPeakDistance = d;}
	
	list<Point> getPeaks() {return peaks;}
	void setPeaks(list<Point> _peaks) {peaks = _peaks;}
	
	void cullPeaks();


	private:
	void initialize();
	
	void findPeaks();
	




	



};



} // Namespace CADX





#endif

