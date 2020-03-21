//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Ftr.h"  
#include "Tokenizer.h" 



Ftr::Ftr() {
	
	initialize();
}   

Ftr::Ftr(char* _fileName) {

	initialize();

	strcpy(fileName, _fileName);
}   

Ftr::Ftr(char* _fileName, char* _title, long _nclasses, char** _clabels,
 long* _nsamples, double* _apriori, long _nfeatures, char** _flabels) {

	initialize();

	strcpy(fileName, _fileName);
	
	writeHeader(_title, _nclasses, _clabels, _nsamples, _apriori, _nfeatures, _flabels);
}

void Ftr::writeHeader(char* _title, long _nclasses, char** _clabels,
  long* _nsamples, double* _apriori, long _nfeatures, char** _flabels) {
	
	strcpy(title, _title);
	nclasses = _nclasses;
	
	for(long i = 0; i < nclasses; i++) { 
		strcpy(clabels[i], _clabels[i]);
		nsamples[i] = _nsamples[i];
		apriori[i] = _apriori[i];
	}
	
	nfeatures = _nfeatures;
	
	for(long k = 0; k < nfeatures; k++) { 
		strcpy(flabels[k], _flabels[k]);
	}
	
	valid = 0;
	
	openForWriting();
	
	writeHeader();
}   
 
void Ftr::openForReading() {
	
	file.open(fileName, ios_base::in);     
	
	if(!file.fail()) valid = 1;

	if(!isValid()) return;
	
//	readHeader();
}      

void Ftr::openForWriting() {
	
	file.open(fileName, ios_base::out);     
	
	if(!file.fail()) valid = 1;

	if(!isValid()) return;
}     
          
Ftr::~Ftr() {   
	

}  

void Ftr::initialize() {
	
	strcpy(title, "");;
	nclasses = 0;
	
	for(long i = 0; i < MAX_CLASSES; i++) { 
		strcpy(clabels[i], "");
		nsamples[i] = 0;
		apriori[i] = 1.0;
	}
	
	nfeatures = 0;
	
	for(long k = 0; k < MAX_FEATURES; k++) { 
		strcpy(flabels[k], "");
	}
	
	valid = 0;
} 

void Ftr::writeHeader() {    

	long i;
	
	file.seekp(0, ios_base::beg); 
	
	file << "#^FTR V1.0" 
	 << "\ntitle: " << title
	 << "\nnclasses: " << nclasses; 
	 
	file << "\nclabels: ";
	for(i = 0; i < nclasses; i++) {file << clabels[i] << " ";} 
	
	file << "\nnsamples: ";
	nsamplesPos = file.tellp();
	for(i = 0; i < nclasses; i++) {file << setw(15) << nsamples[i];} 
	
	file << "\napriori: ";
	for(i = 0; i < nclasses; i++) {file << apriori[i] << " ";} 
	 
	file << "\nnfeatures: " << nfeatures;
	
	file << "\nflabels: ";
	for(i = 0; i < nfeatures; i++) {file << flabels[i] << " ";} 

	file << endl;
}
/*
void Ftr::setFeatures(Roi& roi) { 
		
	nFeatures = roi.getNFeatures();

	label = new char*[nFeatures];	  
	
	for(long k = 0; k < nFeatures; k++) {
		
		label[k] = new char[1024];  
		strcpy(label[k], roi.getFeatureLabel(k)); 		
	}	
}
*/

void Ftr::readHeader() {

	char type[1024], version[1024], token[1024];
	long dataStartPos = 0;
	
	openForReading();
	file.seekg(0, ios_base::beg);  
	
	file >> type >> version;
	if(strcmp(type, "#^FTR") != 0) {setValid(0); return;}   
	
	Tokenizer tokenizer(file);
	
	while(!tokenizer.EndOfFile()) {    
				
		strcpy(token, tokenizer.NextToken());   
		
		if(strcmp(token, "title:") == 0) {
			strcpy(title, tokenizer.NextToken());
		}
		else if(strcmp(token, "nclasses:") == 0) {
			nclasses = atoi(tokenizer.NextToken());
		}
		else if(strcmp(token, "clabels:") == 0) {
			for(long k = 0; k < nclasses; k++) strcpy(clabels[k], tokenizer.NextToken());
		}
		else if(strcmp(token, "nsamples:") == 0) {
			for(long k = 0; k < nclasses; k++) nsamples[k] = atoi(tokenizer.NextToken());
		}
		else if(strcmp(token, "apriori:") == 0) {
			for(long k = 0; k < nclasses; k++) apriori[k] = atof(tokenizer.NextToken());
		}
		else if(strcmp(token, "nfeatures:") == 0) {
			nfeatures = atoi(tokenizer.NextToken());
			currentRecord.allocate(nfeatures);
		}
		else if(strcmp(token, "flabels:") == 0) {
			for(long k = 0; k < nfeatures; k++) strcpy(flabels[k], tokenizer.NextToken());
			dataStartPos = file.tellg();
			return;
		}
	}

   // End with get position at end of header.
  // file.clear();
 //	file.seekg(dataStartPos, ios_base::beg);
}          

void Ftr::writeCandidate(Roi& roi, long classNum) {
	 
	long k;
	char buffer[1024];   

	nsamples[classNum]++;

	for(k = 0; k < nfeatures; k++) file << roi.getFeatureValue(k) << " " ; 
	
	file << clabels[classNum] << " " << roi.getSourceImageName() << " " << roi.getName();
	
	file << endl;
}    

void Ftr::updateHeader() {

	char buffer[1024];

	file.seekp(nsamplesPos, ios_base::beg);

	strcpy(buffer, "");

	for(long i = 0; i < nclasses; i++) {

		file << setw(15) << nsamples[i] << flush;
	} 

}

void Ftr::readNextSample() {
		
	char buffer[1024];
	double v;
	
	for(long k = 0; k < nfeatures; k++) {
      file >> v;
      currentRecord.setFValue(v, k);
   }

   file >> buffer;
   currentRecord.setCLabel(buffer);

   file >> buffer;
   currentRecord.setSource(buffer);
   
   file >> buffer;
   currentRecord.setComment(buffer);
   
   for(long i = 0; i < nclasses; i++) {
      if(strcmp(currentRecord.getCLabel(), clabels[i]) == 0) currentRecord.setCNumber(i);
   }
}





















