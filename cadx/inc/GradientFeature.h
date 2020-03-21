//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef GRADIENTFEATURE_H
#define GRADIENTFEATURE_H



#include "Feature.h"   
#include "Roi.h" 
#include "UserAgent.h"
#include "Iem/Iem.h" 


namespace CADX_SEG {  



class GradientFeature : public Feature {  

	private:   	
	IemTImage<short> img;    
	IemTImage<unsigned char> imgMap; 
	IemTImage<short> imgEdge; 
	IemTImage<unsigned char> imgCosine; 
	
	double dirCoherence;
	double nonuniformity;
	
	long sectorCount[8];
	double sectorHist[8];
	double sectorCosine[8];
	
	double minGradientMag;
	double maxGradientMag;

	double originCol; 
	double originRow;
	
	short regionLabel;
	
	UserAgent userAgent;
	

	public:     
	
	GradientFeature();
	
	GradientFeature(char* labelRoot);

	GradientFeature(char* labelRoot, Roi& roi, double originCol, double originRow);     
	
	GradientFeature(char* labelRoot, IemTImage<short>& img, IemTImage<unsigned char>& imgMap,
	 double originCol, double originRow);  
	 
	GradientFeature(char* labelRoot, IemTImage<short>& img, IemTImage<unsigned char>& imgMap);   
	
	void setMinGradientMag(double x) {minGradientMag = x;}
	void setMaxGradientMag(double x) {maxGradientMag = x;}

	void init();

	void calculate();	
	
	void calcNonuniformity();
	
	double getScore();
	
	IemTImage<short> getEdgeImage() {return imgEdge;} 	
	
	IemTImage<unsigned char> getCosineImage() {return imgCosine;} 	
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	private:          
	
	void regionStatistics();

	void initialize();
	
	void calcGradient();
	
	void findBestOrigin();

	void markCosineImg(long col, long row, long sectorNum, double cosine);
	
	void markCosineImgOrigin(long col, long row);

};  


} // End namepace


#endif 

