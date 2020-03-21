//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef REPORT_H
#define REPORT_H   


#include "Can.h"
#include "Roi.h"      
#include "util.h"   



namespace CADX_SEG {


class Report  {

	private:



	ofstream reportFile; 


	public:
	Report();

	Report(char* fileName);

	~Report();	
	
	void makeReport(Can& can, double minProbability);

	private:    
	
	void initialize();  
	



	 
	
	 
};

 

} // End namespace


#endif
 
