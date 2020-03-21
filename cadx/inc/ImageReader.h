//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef IMAGEREADER_H
#define IMAGEREADER_H


#include "Iem/Iem.h"  


namespace CADX_SEG {


class ImageReader {

	protected:  
	double xSpacing;
	double ySpacing;
	int bitDepth;

	

	public:  
	 
	ImageReader();
	   
   
	IemTImage<short> read(const char* inFileName);    
	                                    
                               
	double getXSpacing() {return xSpacing;}      
	double getYSpacing() {return ySpacing;}
	int getBitDepth() {return bitDepth;}
	
	private:
	void init();



};


} // End namespace



#endif
