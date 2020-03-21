//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//           



#ifndef SCALESELECTOR_H
#define SCALESELECTOR_H


#include "Iem/Iem.h"  
#include "UserAgent.h"


  
namespace CADX_SEG {

class ScaleSelector {
	
	protected:

	UserAgent userAgent;

	
	public:
	
	ScaleSelector();
	
	double getScore(IemTImage<short>& img, IemTImage<unsigned char>& imgRegionMap,
	 IemTImage<unsigned char>& imgLungMask);
	
	void setUserAgent(UserAgent& agent) {userAgent = agent;}



	

	
};



} // End namespace



#endif



