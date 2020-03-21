//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SEGMENTER_H
#define SEGMENTER_H


#include "Iem/Iem.h"    
#include "Roi.h"


namespace CADX_SEG { 



class Segmenter
{
	protected:  
	


	public:

	Segmenter();
	
	IemTImage<unsigned char> segment(Roi& roi);	
	
	IemTImage<unsigned char> growRegion(Roi& roi);
		
	IemTImage<unsigned char> decomposeRegionMap(IemTImage<unsigned char>& imgMap, Roi& roi);
	
	IemTImage<unsigned char> pruneRegions(IemTImage<unsigned char>& imgLabeled, long col, long row, IemTImage<unsigned char>& imgMap);
	
	
	private:
	void initialize();
	
	
};   


} // End namespace

#endif