//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//





#include "FeatureSet.h"   
#include "Util.h"



using namespace CADX_SEG;



FeatureSet::FeatureSet() {
	
	initialize();			
}   


FeatureSet::~FeatureSet() {    
	
	if(value != NULL) delete [] value;  
	
	if(label != NULL) {  
		
		for(long k = 0; k < maxFeatures; k++) {
		
			delete [] label[k];
		}  
		
		delete [] label;
	}				
}   


void FeatureSet::initialize() {
	
	nFeatures = 0;   
	maxFeatures = 100;
		
	value = new double[maxFeatures];     
	
	label = new char*[maxFeatures];	
	
	for(long k = 0; k < maxFeatures; k++) {
		
		label[k] = new char[1024];
	}				
}     



void FeatureSet::addFeature(char* _label, double _value) { 
	
	if(nFeatures >= maxFeatures) return;
	
	strcpy(label[nFeatures], _label);	
		
	value[nFeatures] = _value;	
	
	nFeatures++;		
}

FeatureSet& FeatureSet::operator=(const FeatureSet& rhs) {  
	
	nFeatures = rhs.nFeatures;	
	
	for(long k = 0; k < nFeatures; k++) {  
		
		strcpy(label[k], rhs.label[k]);  
		value[k] = rhs.value[k];
	}

    return *this;	
}

double FeatureSet::getValue(char* name) {

	for(long k = 0; k < nFeatures; k++) {  
		
		if(strcmp(name, label[k]) == 0) return value[k];
	}
	
	return 0;
}

short FeatureSet::hasFeature(char* name) {

	for(long k = 0; k < nFeatures; k++) {  
		
		if(strcmp(name, label[k]) == 0) return 1;
	}
	
	return 0;
}

void FeatureSet::write(ostream& s) { 

	s << "\nFeatureSet: " << "\nnFeatures: " << nFeatures;

	for(long k = 0; k < nFeatures; k++) {  
		
		s << "\n\t" << label[k] << ": " << value[k];
	}
}







