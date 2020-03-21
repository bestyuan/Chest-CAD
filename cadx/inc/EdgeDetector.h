//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef EDGEDETECTOR_H
#define EDGEDETECTOR_H    


#include "Iem/Iem.h" 
  
namespace CADX_SEG { 


class EdgeDetector {   
	
	private:
	
	double upperHistPen;
	
	
	public:
	
	
	EdgeDetector();	
	
	IemTImage<short> calcSobelGradient(IemTImage<short>& img);

	void setUpperHistPen(double t) {upperHistPen = t;}
	

	
};



} // End namepace





#endif



