//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Hit.h" 
#include "Tokenizer.h" 
#include "Util.h"      


using namespace CADX_SEG;


Hit::Hit() {
	initialize();
}
	    

Hit::Hit(const char *hitFileName) { 
	
	open(hitFileName);	
}   


void Hit::open(const char *hitFileName) {     
	
	initialize(); 
	
	if(hitFileName == NULL) return;
	if(strcmp(hitFileName, "") == 0) return;
	
	ifstream data(hitFileName);  

	if(!data.fail()) valid = 1;

	if(!isValid()) return;
	
	     
	// Make sure EOF flag is reset.
	data.clear();  	

	read(data);
     
	data.close();	
}


void Hit::initialize() {
	strcpy(xraySourceID, "\0");
	imageWidth = 0;
	imageHeight = 0;
	numNodules = 0;   
	valid = 0;

	noduleIndex = NULL;
	status = NULL;
	centroidRow = NULL;
	centroidCol = NULL;
	nOutlinePoints = NULL;
	outlineRow = NULL; 
	outlineCol = NULL;
	detected = NULL;
}


void Hit::allocateNodulesArrays() {
	noduleIndex = new long[numNodules];
	status = new long[numNodules];
	centroidRow = new long[numNodules];
	centroidCol = new long[numNodules];
	nOutlinePoints = new long[numNodules];
	outlineRow = new long*[numNodules]; 
	outlineCol = new long*[numNodules];
	detected = new bool[numNodules]; 
	
	for(long i = 0; i < numNodules; i++) {
		detected[i] = false;	                                     	
	}
}


void Hit::allocateNoduleArrays(long i) {
	outlineRow[i] = new long[nOutlinePoints[i]];
	outlineCol[i] = new long[nOutlinePoints[i]];					
}  


Hit::~Hit() {	   
	
	long i;

	if(outlineRow != NULL) {
		for(i = 0; i < numNodules; i++) {
			if(outlineRow[i] != NULL) delete[] outlineRow[i];       
	}}

	if(outlineCol != NULL) {
		for(i = 0; i < numNodules; i++) {
			if(outlineCol[i] != NULL) delete[] outlineCol[i];  
	}}
	
	if(noduleIndex != NULL) delete[] noduleIndex;    
	if(status != NULL) delete[] status;              	
	if(centroidRow != NULL) delete[] centroidRow;   		
	if(centroidCol != NULL) delete[] centroidCol;
	if(nOutlinePoints != NULL) delete[] nOutlinePoints;
	if(outlineRow != NULL) delete[] outlineRow;    	
	if(outlineCol != NULL) delete[] outlineCol;  
	if(detected != NULL) delete[] detected;
}            



long Hit::getGroundtruth(long col, long row) {

	for(long i = 0; i < numNodules; i++) {
		if(isInsideOutline(i, col, row)) return getNoduleStatus(i);
	}
		
	return Util::GROUNDTRUTH_NEGATIVE;
}


long Hit::getGroundtruth(long col, long row, long& index) {

	for(long i = 0; i < numNodules; i++) {
		if(isInsideOutline(i, col, row)) {index = i; return getNoduleStatus(i);}
	}
		
	return Util::GROUNDTRUTH_NEGATIVE;
}



long Hit::getNumWithGroundtruth(long gt) {

   long n = 0;

	for(long i = 0; i < numNodules; i++) {
		if(getNoduleStatus(i) == gt) n++;
	}
		
	return n;
}


bool Hit::isInsideOutline(long i, long col, long row) {
	  
	long minCol = LONG_MAX, maxCol = LONG_MIN; 
	long minRow = LONG_MAX, maxRow = LONG_MIN;
	
	for(long k = 0; k < nOutlinePoints[i]; k++) {
		if(outlineRow[i][k] == row) {
			long c = outlineCol[i][k];
			if(c < minCol) minCol = c;
			if(c > maxCol) maxCol = c;
		}
		if(outlineCol[i][k] == col) {
			long r = outlineRow[i][k];
			if(r < minRow) minRow = r;
			if(r > maxRow) maxRow = r;
		}
	}
			
	if(col >= minCol && col <= maxCol && row >= minRow && row <= maxRow) return true;
				
	return false;
}


long Hit::getNoduleArea(long i) {    
		
	long minCol = LONG_MAX, maxCol = LONG_MIN;
	long minRow = LONG_MAX, maxRow = LONG_MIN;
	long area = 0;
	
	for(long k = 0; k < nOutlinePoints[i]; k++) {
		long c = outlineCol[i][k];
		long r = outlineRow[i][k];        
						
		if(c < minCol) minCol = c;  
		if(c > maxCol) maxCol = c;        
		
		if(r < minRow) minRow = r;  
		if(r > maxRow) maxRow = r;
	}
		
	for(long c = minCol; c <= maxCol; c++) {
		for(long r = minRow; r <= maxRow; r++) {		
			
			if(isInsideOutline(i, c, r)) area++;	
	}}

	return area;					
}


long Hit::getEnclosingNodule(long col, long row) {  
	
	for(long i = 0; i < numNodules; i++) {
		if(isInsideOutline(i, col, row)) return i;
	}
		
	return -1;	
}


void Hit::read(istream& s) { 
	      
	char type[1024], version[1024], token[1024];	
	long i = 0;
	
	s >> type >> version;
	if(strcmp(type, "#^HIT") != 0) Util::fatalError("Hit::read()- wrong file type.");   
	
	Tokenizer tokenizer(s);
	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(token, tokenizer.NextToken());

		cout << "\n" << token << flush;  
		
		if(strcmp(token, "xraySourceID:") == 0) {
			strcpy(xraySourceID, tokenizer.NextToken());
		}
		else if(strcmp(token, "imageSize:") == 0) {
			imageWidth = atoi(tokenizer.NextToken());     
			imageHeight = atoi(tokenizer.NextToken());
		}     
		else if(strcmp(token, "numNodules:") == 0) {
			numNodules = atoi(tokenizer.NextToken()); 
			allocateNodulesArrays();	    
		}
		else if(strcmp(token, "noduleIndex:") == 0) {
		
			if(i >= numNodules) Util::fatalError("Hit::read()- nodule index out of range."); 
		
			noduleIndex[i] = atoi(tokenizer.NextToken());
			
			if(strcmp("status:", tokenizer.NextToken()) == 0) {  
				strcpy(token, tokenizer.NextToken());
				if(strcmp("definite", token) == 0) status[i] = Util::GROUNDTRUTH_DEFINITE;  
				if(strcmp("probable", token) == 0) status[i] = Util::GROUNDTRUTH_PROBABLE;  
				if(strcmp("possible", token) == 0) status[i] = Util::GROUNDTRUTH_POSSIBLE;  
			}   
			else Util::fatalError("Hit::read()- nodule status missing."); 
			   
			if(strcmp("centroid:", tokenizer.NextToken()) == 0) {  
				centroidCol[i] = atoi(tokenizer.NextToken());
				centroidRow[i] = atoi(tokenizer.NextToken());					
			}   
			else Util::fatalError("Hit::read()- nodule centroid missing.");	
			
			if(strcmp("nPoints:", tokenizer.NextToken()) == 0) {  
				nOutlinePoints[i] = atoi(tokenizer.NextToken());				
			}   
			else Util::fatalError("Hit::read()- nodule nOutlinePoints missing.");			
			 
			allocateNoduleArrays(i);	
			
			for(long j = 0; j < nOutlinePoints[i]; j++) {
				outlineCol[i][j] = atoi(tokenizer.NextToken());
				outlineRow[i][j] = atoi(tokenizer.NextToken()); 								
			}    
			
			i++;
			
		} // End nodule	
		
	} // End Hit file
	
} 


void Hit::write(ostream& s) {

 	s << "\nxraySourceID: " << xraySourceID << "\nimageWidth: " << imageWidth
	 << "\nimageHeight: " << imageHeight << "\nnumNodules: " << numNodules;
}
