//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef REGIONFEATURE_H
#define REGIONFEATURE_H



#include "Feature.h"   
#include "Roi.h"  
#include "Iem/Iem.h"


namespace CADX_SEG {



class RegionFeature : public Feature {  

	private:
	IemTImage<short> img;    
	IemTImage<unsigned char> imgMap;
	IemTImage<unsigned char> imgBkMap;
	IemTImage<unsigned char> imgExclude;

	long area;	
	long bkArea;

	double avgCodeValue;
	double sigmaCodeValue;
	double aspectRatio;   
	double avgBkCodeValue;   
	double sigmaBkCodeValue; 
	short minCodeValue;
	short maxCodeValue;
	short minBkCodeValue;
	short maxBkCodeValue;

	long centerCol, centerRow;
	long minCol, minRow;
	long maxCol,maxRow;
	
	long bkSubtraction;
	

	public:     
	
	RegionFeature();
	RegionFeature(char* labelRoot);
	RegionFeature(char* labelRoot, Roi& roi);     
	RegionFeature(char* labelRoot, IemTImage<short>& img, IemTImage<unsigned char>& imgMap,
	 IemTImage<unsigned char>& imgExclude);
   
	IemTImage<unsigned char> getBkMap() {return imgBkMap;}

	void calculate();	
	
	double getScore();
	
	long getMinRow() {return minRow;}
	long getMinCol() {return minCol;}
	long getMaxRow() {return maxRow;}
	long getMaxCol() {return maxCol;}
	
	void doBkSubtraction(long v) {bkSubtraction = v;}

	
	private:          
	
	void regionStatistics();
	
	void regionBkStatistics();
	
	void initialize();
	
	void makeBkMap();
	           

	




}; 


} // End namespace 



#endif 

