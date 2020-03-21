//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Feature.h"    
#include "Util.h" 


using namespace CADX_SEG;
   
   
Feature::Feature() {  
	
	initialize();
} 


Feature::Feature(long _nValues, char *_labelRoot) {  

	initialize();
	
	nValues = _nValues;
	
	strcpy(labelRoot, _labelRoot);
	
	allocate();
}


void Feature::initialize() {  
	
	nValues = 0;
	value = NULL;   
	label = NULL;
	strcpy(labelRoot, "");
} 


Feature::~Feature() {
	
	free();
} 


void Feature::free() {
	
	if(value != NULL) delete[] value;
	
	if(label != NULL) {
		
		for(long k = 0; k < nValues; k++) delete[] label[k];
		
		delete[] label;			
	}		
}  


void Feature::allocate() {
	
	value = new double[nValues];   
	label = new char*[nValues];
	
	for(long k = 0; k < nValues; k++) {
		label[k] = new char[1024];    
		strcpy(label[k], "");
		value[k] = 0.0;
	}
} 


Feature& Feature::operator =(const Feature& rhs) {
	
	free();
	
	nValues = rhs.nValues;
	strcpy(labelRoot, rhs.labelRoot);
	
	allocate();
		
	for(long k = 0; k < nValues; k++) {  
		
		strcpy(label[k], rhs.label[k]);
		value[k] = rhs.value[k];
	}
	
	return *this;
}       


void Feature::addToRoi(Roi& roi) {
	
	for(long j = 0; j < getNValues(); j++) {		
		roi.addFeature(getLabel(j), getValue(j));	
	}            
}   


void Feature::write(ostream& s) {
	
	for(long j = 0; j < getNValues(); j++) {
	
		s << "\n" << getLabel(j) << " = " << getValue(j);	
	}            
}   