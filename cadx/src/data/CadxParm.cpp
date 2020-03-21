//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "CadxParm.h" 
#include "Tokenizer.h"  
#include "Iem/IemImageIO.h" 


using namespace CADX_SEG; 


CadxParm CadxParm::instance;

    
    
void CadxParm::readFile(const char *parmFileName) { 

	char token[1024];
	
	
	ifstream file(parmFileName, ios_base::in);
	
	Tokenizer tokenizer(file);
	    
	while(!tokenizer.EndOfFile()) {    
				
		strcpy(token, tokenizer.NextToken());

		if(strcmp(token, "minCodeValue") == 0) {
			minCodeValue = atoi(tokenizer.NextToken());
		}      
		else if(strcmp(token, "maxCodeValue") == 0) {
			maxCodeValue = atoi(tokenizer.NextToken());
		}    
		else if(strcmp(token, "roiResolution") == 0) {
			roiResolution = atoi(tokenizer.NextToken());
		}
		else if(strcmp(token, "invert") == 0) {
			short v = atoi(tokenizer.NextToken());
			if(v == 0) {invert = false;}
			else {invert = true;}
		}
		else if(strcmp(token, "aimPixelSpacing") == 0) {
			aimPixelSpacing = atof(tokenizer.NextToken());
		}       		      
		else if(strcmp(token, "maxCandidates") == 0) {
			maxCandidates = atoi(tokenizer.NextToken());
		}  
		else if(strcmp(token, "regionSupportThreshold") == 0) {
			regionSupportThreshold = atoi(tokenizer.NextToken());
		}
		else if(strcmp(token, "maxAnnotations") == 0) {
			maxAnnotations = atoi(tokenizer.NextToken());
		}
		else if(strcmp(token, "group") == 0) {
			short v = atoi(tokenizer.NextToken());
			if(v == 0) {group = false;}
			else {group = true;}
		}
		else if(strcmp(token, "maxOverlap") == 0) {
			maxOverlap = atof(tokenizer.NextToken());
		}
		else if(strcmp(token, "nNoduleTemplates") == 0) {
			nNoduleTemplates = atoi(tokenizer.NextToken());
			nNoduleTemplates = Util::min(nNoduleTemplates, 16);
			
			for(long j = 0; j < nNoduleTemplates; j++) {
				strcpy(noduleTemplateFile[j], tokenizer.NextToken());
				nNoduleAggregations[j] = atoi(tokenizer.NextToken());
			}				
		}  
		else if(strcmp(token, "nRibAggregations") == 0) {
			nRibAggregations = atoi(tokenizer.NextToken());
		}  
		else if(strcmp(token, "nRibTemplates") == 0) {
			nRibTemplates = atoi(tokenizer.NextToken());
			nRibTemplates = Util::min(nRibTemplates, 16);

			for(long j = 0; j < nRibTemplates; j++) {
				strcpy(ribTemplateFile[j], tokenizer.NextToken());
			}				
		}       
				
	}
	
	readTemplates();			
}


CadxParm& CadxParm::getInstance() {
//	if(instance == NULL)  instance = new CadxParm;
//	return *instance;
	return instance;
} 

 
CadxParm::CadxParm() {
	maxCodeValue = 4095;
	minCodeValue = 0;
	invert = false;
	roiResolution = 512;
	aimPixelSpacing = 0.171;
	maxCandidates = 20;
	regionSupportThreshold = 1;

	nNoduleTemplates = 0;
	pNoduleTemplates = NULL;
	
	nRibTemplates = 0;
	pRibTemplates = NULL;
	nRibAggregations = 0;
	
	lungMask = true;
	
	group = false;
	maxOverlap = 0.0;
	
	maxAnnotations = 10;
}     
    
    
CadxParm::~CadxParm() {
	if(pNoduleTemplates != NULL) delete[] pNoduleTemplates;
	if(pRibTemplates != NULL) delete[] pRibTemplates;  		
} 


void CadxParm::readTemplates() {

	if(nNoduleTemplates > 0) {
		pNoduleTemplates = new IemTImage<short>[nNoduleTemplates];
		for(short j = 0; j < nNoduleTemplates; j++) pNoduleTemplates[j] << noduleTemplateFile[j];
	}
	if(nRibTemplates > 0) {
		pRibTemplates = new IemTImage<short>[nRibTemplates];
		for(short j = 0; j < nRibTemplates; j++) pRibTemplates[j] << ribTemplateFile[j];
	}
}


void CadxParm::write(ostream& s) {    

	s << "\n\nCadxParm:";
	
	s << "\n\tnminCodeValue = " << minCodeValue
	  << ", maxCodeValue = " << maxCodeValue
	  << "\n\troiResolution = " << roiResolution
	  << ", aimPixelSpacing = " << aimPixelSpacing
	  << "\n\tmaxCandidates = " << maxCandidates
	  << "\n\tnNoduleTemplates = " << nNoduleTemplates;	                                                	
	  	
	for(long k = 0; k < nNoduleTemplates; k++) {
		s << "\n\t\t" << noduleTemplateFile[k]; 
	}
	
	s << endl << endl;
}     
