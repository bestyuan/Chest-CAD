//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef TEXTUREFEATURE_H
#define TEXTUREFEATURE_H



#include "Feature.h"   
#include "Roi.h"  
#include "Cooccurrence.h"
#include "Iem/Iem.h"


namespace CADX_SEG {



class TextureFeature : public Feature {

	private:   	
	IemTImage<short> img;    
	IemTImage<unsigned char> imgMap; 
	
	IemTImage<short> imgAgg;
	IemTImage<unsigned char> imgMapAgg;
	IemTImage<short> imgEdge;

	short aggregation;
	

	public:     

	TextureFeature();
	
	TextureFeature(char* labelRoot);

	TextureFeature(char* labelRoot, IemTImage<short>& img, IemTImage<unsigned char>& imgMap);

	void calculate();

	double getScore();
	
	IemTImage<short> getImage() {return imgAgg;}
	IemTImage<unsigned char> getImageMap() {return imgMapAgg;}
	IemTImage<short> getEdgeImage() {return imgEdge;}
	
//	Cooccurrence& getCooccurrence() {return cooccurrence;}
	
	private:

	void initialize();
	
	void scaleImage();





};  


} // End namespace

#endif 

