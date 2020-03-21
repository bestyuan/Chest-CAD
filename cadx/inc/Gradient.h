//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef GRADIENT_H
#define GRADIENT_H


#include "Iem/Iem.h" 

  
namespace CADX_SEG {


class Gradient {
	
	private:
	
	long nPixels;
	double meanMag;
	

	public:

	
	Gradient();
	
	IemTImage<short> calcSobelGradient(IemTImage<short>& img);
	
	IemTImage<short> calcSobelGradient(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);

	double getMeanMag() {return meanMag;}

	private:
	
	void initialize();

	
};



} // End namepace





#endif



