//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "GmlData.h"
#include "Tokenizer.h" 
#include "Point.h"
#include <list>



using namespace CADX_SEG;



GmlData::GmlData() {
	initialize();
}   


GmlData::GmlData(char* fileName) {
	open(fileName);
} 


GmlData::~GmlData() {   	
	if(classLabels != NULL) {
		for(long k = 0; k < nClasses; k++) delete[] classLabels[k];
		delete[] classLabels;
	}
}


void GmlData::initialize() {
	strcpy(classifierFileName, "unknown");
	nClasses = 0;
	candidateIndex = 0;
	nFeatures = 0;
	valid = false;
	classLabels = NULL;

	startDataPos = 0;
}


void GmlData::open(char* fileName) {
	initialize();
		
	file.open(fileName, ios_base::in);

	if(!file.fail()) valid = true;
	if(!valid) return;
	file.clear();
	readHeader();
}
                

void GmlData::readHeader() {
	
	char type[1024], version[1024], token[1024];	

	file.seekg(0, ios_base::beg);  
	 
	file >> type >> version;
	if(strcmp(type, "#^GML") != 0) return;
	if(strcmp(version, "V1.5") != 0) return;
	
	Tokenizer tokenizer(file);
	
	while(!tokenizer.EndOfFile()) {    
				
		strcpy(token, tokenizer.NextToken());   

		if(strcmp(token, "GmlClassifier:") == 0) {
			strcpy(classifierFileName, tokenizer.NextToken());
		}
		else if(strcmp(token, "nclasses:") == 0) {
			nClasses = atoi(tokenizer.NextToken());
			classLabels = new char*[nClasses];
			for(long k = 0; k < nClasses; k++) classLabels[k] = new char[256];
		}
		else if(strcmp(token, "clabels:") == 0) {
			for(long k = 0; k < nClasses; k++) strcpy(classLabels[k], tokenizer.NextToken());
		}
		else if(strcmp(token, "numFeatures:") == 0) {
			nFeatures = atoi(tokenizer.NextToken()); 
		}
		else if(strcmp(token, "flabels:") == 0) {
			for(long k = 0; k < nFeatures; k++) addFeature(tokenizer.NextToken());
		}    
		else if(strcmp(token, "endHeader:") == 0) {
			startDataPos = file.tellg();
			break;
		} 
	}
	 
	file.clear();	
	file.seekg(0, ios_base::beg);
}          
        

void GmlData::readNext(GmlProb& prob) {
		
	char token[1024];	 

	Tokenizer tokenizer(file);

	// Allocated memory.
	prob.allocate(nFeatures);

	while(!tokenizer.EndOfFile()) {

		strcpy(token, tokenizer.NextToken());  
		
		if(strcmp(token, "classIndex:") == 0) {
			long classIndex = atoi(tokenizer.NextToken());
			prob.setLabel(classLabels[classIndex]);
		}
		else if(strcmp(token, "prior:") == 0) {
			prob.setPrior(atof(tokenizer.NextToken()));
		}
		else if(strcmp(token, "lnDet:") == 0) {
			prob.setLogCovarDet(atof(tokenizer.NextToken()));
		}
		else if(strcmp(token, "mean:") == 0) {
			for(long k = 0; k < nFeatures; k++) {
				prob.setMean(k, atof(tokenizer.NextToken()));
			}
		}
		else if(strcmp(token, "icov:") == 0) {
			for(long r = 0; r < nFeatures; r++) {
			     for(long c = 0; c < nFeatures; c++) {
					prob.setInvCovar(c, r, atof(tokenizer.NextToken()));
			}}
		}
		else if(strcmp(token, "endClass:") == 0) {
			break;
		}	

	}
		

}


void GmlData::write(ostream& s) {
	long k;

	s << "#^GML V1.5";
	s << "\nGmlClassifier: X";
	s << "\nnclasses: " << nClasses;
	s << "\nclabels:";
	for(k = 0; k < nClasses; k++) s << " " << classLabels[k];

	s << "\nnumFeatures: " << nFeatures;
	s << "\nflabels:";
	for(k = 0; k < nFeatures; k++) s << " " << featureSet.getLabel(k);
	s << "endHeader:" << endl;
}














