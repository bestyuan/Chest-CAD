//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MULTIMATCHEDFILTER_H
#define MULTIMATCHEDFILTER_H

 
#include "Iem/Iem.h"  



class MultiMatchedFilter {


	protected:
	IemTImage<short>* pImgTemplate;
	short nTemplates;
	short decimation;


	public:    
	MultiMatchedFilter(IemTImage<short>* _pImgTemplate, short _nTemplates, short _decimation);
	 
	~MultiMatchedFilter();
	
	
	IemTImage<short> filter(IemTImage<short>& img);  
	
	protected:
	
	void initialize();
};




#endif