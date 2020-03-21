//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SHAPEFEATURE_H
#define SHAPEFEATURE_H



#include "Feature.h"   
#include "Roi.h"  
#include "Iem/Iem.h"   


namespace CADX_SEG {



class ShapeFeature : public Feature {  

	private:   	
	IemTImage<unsigned char> imgMap; 
	IemTImage<unsigned char> imgConvexHull;
	
	long area;
	long imageArea;	
	long convexHullArea;
	double colCenter;
	double rowCenter;
	double momentRR;
	double momentCC;
	double momentRC;  
	double fit; 
	double avgCodeValue;
	double sigmaCodeValue;
	double aspectRatio;   
	double avgBkCodeValue;   
	double sigmaBkCodeValue;
	short regionLabel;
	double position;
	short colAim;
	short rowAim;
	

	public:     
	
	ShapeFeature(); 
	ShapeFeature(char* labelRoot);   
	ShapeFeature(char* labelRoot, Roi& roi);     
	ShapeFeature(char* labelRoot, IemTImage<unsigned char>& imgMap);     

	void calculate();	
	
	double getScore();
	
	double getColCenter() {return colCenter;}
	double getRowCenter() {return rowCenter;}
	
	IemTImage<unsigned char> getConvexHull() {return imgConvexHull;}
	
	
	private:
	void regionStatistics();
	
	void calculateShape();
	
	void calculateConvexHull();
	
	void initialize(); 
	           

	
	



};  


} // End namespace


#endif 

