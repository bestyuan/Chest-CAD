//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef ALTSEGMENTER_H
#define ALTSEGMENTER_H


#include "Iem/Iem.h"    
#include "Roi.h" 



class AltSegmenter
{
	protected:  
	
	char outputDir[1024];
	char name[1024];
	


	public:

	AltSegmenter();
	
	IemTImage<unsigned char> segment(Roi& roi);	
	
	IemTImage<short> excludeNonLung(IemTImage<short>& img, IemTImage<unsigned char>& map);
	

	IemTImage<short> ribFilter(IemTImage<short>& img);
	
	IemTImage<unsigned char> pruneRegions(IemTImage<unsigned char>& imgLabeled, long col, long row, IemTImage<unsigned char>& imgMap);


	
	private:
	void initialize();
	


};   


#endif
