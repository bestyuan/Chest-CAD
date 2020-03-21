//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Report.h"
#include "Tokenizer.h"
#include <list>



using namespace CADX_SEG;



Report::Report() {
	initialize();
}   


Report::Report(char* fileName) {
	initialize();
	reportFile.open(fileName);
}   


Report::~Report() {

}


void Report::makeReport(Can& can, double minProbability) {

	reportFile << "LungCAD Report" << endl;

	can.seekgToFirstCandidate();
                                                 	
	for(long i = 0; i < can.getNCandidates(); i++) {
	                
		Roi roi;
		can.readNextCandidate(roi);
		
		if(roi.getProbability() >= minProbability) {
		                                           	
			reportFile << "\n Nodule detected at col= " << roi.getAbsCandidateCol()
			 << ", row= " << roi.getAbsCandidateRow() 
			 << ", with probability= " << roi.getProbability() << flush;
		}
	   

   }  // Next candidate

}


void Report::initialize() {

}



