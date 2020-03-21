//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef STRUCTREMOVER_H
#define STRUCTREMOVER_H

 
#include "Iem/Iem.h" 
#include "Roi.h"


namespace CADX_SEG {



class StructRemover {

	public:    
	StructRemover();
	
	void initialize();
		
	IemTImage<short> filter(Roi& roi);         

};


} // End namespace


#endif
