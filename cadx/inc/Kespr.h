//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef KESPR_H
#define KESPR_H


#include "Iem/Iem.h" 
#include "Util.h"   


namespace CADX_SEG {


class Kespr
{

	private:  
	
	static const long HEIGHT_OFFSET;          
	static const long WIDTH_OFFSET; 
	static const long SPACING_OFFSET;  
	static const long HEADER_SIZE; 
	
	long height, width, bands;  	// Image dimensions.
	double xSpacing, ySpacing; 	// Physical distance between pixels.
              	




	public:  
	 
	// Default contructor.
	Kespr();
	   
	// Read a KESPR file into an IEM image object.	   
	IemTImage<short> Read(const char* InFileName);    
	                                    
	double getHeight() {return height;}      
	double getWidth() {return width;}      
	double getBands() {return bands;}      	                                    
	double getXSpacing() {return xSpacing;}      
	double getYSpacing() {return ySpacing;}



};


} // End namespace

#endif
