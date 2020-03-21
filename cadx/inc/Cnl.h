//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CNL_H
#define CNL_H


#include "Util.h"  


namespace CADX_SEG {



class Cnl
{

	private:  
	long nCandidates;	
	long *candidateColumn;
	long *candidateRow;              	
	double *candidateProb;   



	public:  
	 
	// Default contructor.
	Cnl();
	    
	Cnl(const char *cnlFileName);       
	
	void open(const char *cnlFileName);

	// Destructor.
	~Cnl();    
	
	long getNCandidates() {return nCandidates;}     
	long getCandidateColumn(long i) {return candidateColumn[i];}     
	long getCandidateRow(long i) {return candidateRow[i];} 
	double getCandidateProb(long i) {return candidateProb[i];}   
	
	void read(istream& s); 
	
	               
            
	private:                     
                  
	void initialize();
     









};


} // End namespace

#endif