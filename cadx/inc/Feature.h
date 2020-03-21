//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef FEATURE_H
#define FEATURE_H



#include "Roi.h"
#include "UserAgent.h"



namespace CADX_SEG {



class Feature { 
    
	protected:
	long nValues;
	double* value;    
	char** label; 
	char labelRoot[1024];

	UserAgent userAgent;
	
	
	public:
	Feature();     
	Feature(long nValues, char *_labelRoot);  
	
	~Feature();
	              

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	              
	void free();
	void initialize();
	void allocate();

	long getNValues() {return nValues;}
	double getValue(long i) {return value[i];}    
	char* getLabel(long i) {return label[i];} 
	
	void setLabelRoot(char *root) {strcpy(labelRoot, root);}

	Feature& operator =(const Feature& rhs);   

	void addToRoi(Roi& roi);
	
	void write(ostream& s);

};  



} // End namespace


#endif 

