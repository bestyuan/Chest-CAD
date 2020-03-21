//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef HISTOGRAM_H
#define HISTOGRAM_H


#include "Iem/Iem.h" 
#include "Util.h" 


namespace CADX_SEG {


class Histogram {

 	private:
 	IemTImage<short> img; 
 	IemTImage<unsigned char> imgMask;

 	double* histogram;    	
 	long minCodeValue;  
	long maxCodeValue; 	
	long noLevels;
	long band;
 	
 	

	public:  
	Histogram();
	Histogram(long minCodeValue, long maxCodeValue);
	
	~Histogram();

	void build(IemTImage<short>& img, long band = 0);

	void build(IemTImage<short>& img, IemTImage<unsigned char>& imgMask, long band = 0);

	IemTImage<short> equalize(IemTImage<short>& img, long band = 0);
	

	// Returns the code value for which the specified fraction of pixels
	// have a code value greater or equal to.
	short getUpperPenetration(double fraction);
	
	short getLowerPenetration(double fraction);
	
	void write(ostream& s);
	   

	protected:

 	void initialize();


};


} // End namespace

#endif
