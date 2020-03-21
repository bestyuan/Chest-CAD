//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef FEATURESET_H
#define FEATURESET_H   



#include "util.h"


namespace CADX_SEG {


class FeatureSet {

	private:
	long nFeatures;
	long maxFeatures; 
	double* value;   
	char** label;



	public:
	FeatureSet();
	
	~FeatureSet();  
	
	void initialize();
	
	void addFeature(char* label, double value = 0.0);
	
	double getValue(long i) {return value[i];}  
	
	void setValue(long i, double v) {value[i] = v;}
	
	// Returns the value of the feature with the specified name.
	double getValue(char* name);   
	
	// Returns 1 if a feature with the specified name exists otherwise return 0.
	short hasFeature(char* name);   
	
	char* getLabel(long i) {return label[i];}    
	
	long getNFeatures() {return nFeatures;}         
	
	
	FeatureSet& operator=(const FeatureSet& rhs);
	
	void write(ostream& s);	
	
	
};



} // End namespace


#endif
