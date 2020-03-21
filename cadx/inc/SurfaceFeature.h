//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SURFACEFEATURE_H
#define SURFACEFEATURE_H



#include "Feature.h"   
#include "Roi.h"  
#include "PolyFit.h"
#include "Iem/Iem.h"   


namespace CADX_SEG {



class SurfaceFeature : public Feature {  

	private:   	
	IemTImage<short> img;
	IemTImage<unsigned char> imgMap; 
	IemTImage<short> imgFit;

	long colCenter;
	long rowCenter;
	
	double lamda1;
	double lamda2;
	double lamda12;
	
	PolyFit polyFit;

	public:
	
	SurfaceFeature();
	SurfaceFeature(char* labelRoot);
	SurfaceFeature(char* labelRoot, Roi& roi);
	SurfaceFeature(char* labelRoot, IemTImage<short>& img,
	 IemTImage<unsigned char>& imgMap, long colCenter, long rowCenter);

	void calculate();	
	
	double getScore();
	
	double getColCenter() {return colCenter;}
	double getRowCenter() {return rowCenter;}
	
	IemTImage<short> getFitImage() {return imgFit;}


	private:

	void initialize();
	
	void regionStatistics();
	           

	




};  


} // End namespace


#endif 

