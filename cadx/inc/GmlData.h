//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef GMLDATA_H
#define GMLDATA_H


#include "FeatureSet.h"  
#include "GmlProb.h"
#include "util.h"   



namespace CADX_SEG {


class GmlData  {

	private:
	char classifierFileName[1024];
	long nClasses;
	long candidateIndex;
	long nFeatures;
	
	bool valid;
   
	FeatureSet featureSet;
	
	char** classLabels;

	fstream file;

	long startDataPos;


	public:
	GmlData();

	// Constructor to create a Can for reading.	
	GmlData(char* fileName);

	~GmlData();


	void open(char* fileName);

	void close() {file.close();}
	

 	void seekgToFirstCandidate() {file.seekg(startDataPos, ios_base::beg); file.clear();}
	

	long getNClasses() {return nClasses;}
	long getNFeatures() {return nFeatures;}    


	double getFeatureValue(char* name) {return featureSet.getValue(name);}
	short hasFeature(char* name) {return featureSet.hasFeature(name);}
	void addFeature(char* label, double value = 0.0) {featureSet.addFeature(label, value);}
	
	FeatureSet& getFeatureSet() {return featureSet;}	                                     

	void readNext(GmlProb& prob); 

	void write(ostream& s);
   

	private:
	
	void initialize();  

	void readHeader();
	
	void setFeatures(GmlProb& prob);
	


	
	
	
};



} // End namespace


#endif
