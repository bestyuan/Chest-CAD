//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Cnl.h"



using namespace CADX_SEG;



Cnl::Cnl() { 
	
	initialize();	
}    


Cnl::Cnl(const char *cnlFileName) { 
	
	open(cnlFileName);
}  


void Cnl::open(const char *cnlFileName) { 
	
	initialize();
	
	ifstream data(cnlFileName);  
	
	read(data);

	data.close();
}  


void Cnl::initialize() {
	nCandidates = 0;	
	candidateColumn = NULL;
	candidateRow = NULL;              	
	candidateProb = NULL;   		
} 


Cnl::~Cnl() { 
	
	if(candidateColumn != NULL) delete[] candidateColumn;
	if(candidateRow != NULL) delete[] candidateRow;              	
	if(candidateProb != NULL) delete[] candidateProb;   		
}


void Cnl::read(istream& s) {        
	
	long row, column;
	double prob; 
	
	// Determine number of candidates.
	while(1) {
		
		s >> column >> row >> prob;   
		
		if(s.eof()) break;

		nCandidates++;
	}

	candidateColumn = new long[nCandidates];
	candidateRow = new long[nCandidates];           	
	candidateProb = new double[nCandidates];    
	
	s.clear();
	s.seekg(ios::beg);

	for(long k = 0; k < nCandidates; k++) {

		s >> candidateColumn[k] >> candidateRow[k] >> candidateProb[k];  
	}

}      


