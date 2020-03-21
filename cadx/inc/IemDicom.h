//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef IEMDICOM_H
#define IEMDICOM_H


#include "Iem/Iem.h" 
#include "Util.h" 

namespace CADX_SEG {



class IemDicom
{
	private:


	long height, width, bands;  	// Image dimensions.
	double xSpacing, ySpacing; 	// Physical distance between pixels.
	long bitDepth;
              	

	public:  
	 
	// Default contructor.
	IemDicom();
	   
	// Read a Dicom file into an IEM image object.
	IemTImage<short> Read(const char* InFileName);    
	                                    
	double getHeight() {return height;}      
	double getWidth() {return width;}      
	double getBands() {return bands;}      	                                    
	double getXSpacing() {return xSpacing;}      
	double getYSpacing() {return ySpacing;}
	long getBitDepth() {return bitDepth;}



};


} // End namespace

#endif
