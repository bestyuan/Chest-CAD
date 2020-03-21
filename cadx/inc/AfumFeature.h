//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef AFUMFEATURE_H
#define AFUMFEATURE_H



#include "Feature.h"   
#include "Roi.h"  
#include "Iem/Iem.h"


namespace CADX_SEG {



class AfumFeature : public Feature {

	private:
	IemTImage<short> img;    
	IemTImage<unsigned char> imgMap;
	IemTImage<unsigned char> imgBkMap;
	IemTImage<unsigned char> imgExclude;

	long area;	
	long bkArea;

	double lowerHistPenPnt;
	short lowerCodeValue;
	
	long bkCount;

	long centerCol, centerRow;


	public:     

	AfumFeature();
	AfumFeature(char* labelRoot);
	AfumFeature(char* labelRoot, Roi& roi);
	AfumFeature(char* labelRoot, IemTImage<short>& img, IemTImage<unsigned char>& imgMap,
	 IemTImage<unsigned char>& imgExclude);
   
	IemTImage<unsigned char> getBkMap() {return imgBkMap;}

	void calculate();	
	
	double getScore();

	private:

	void regionStatistics();
	
	void regionBkStatistics();
	
	void initialize();
	
	void makeBkMap();
	           

	




}; 


} // End namespace 



#endif 

