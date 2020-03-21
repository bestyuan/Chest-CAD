//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "cadx.h"      
#include "FeatureStats.h" 
#include "Tokenizer.h"  


int main(int argc, char* argv[])
{                    
	short narg = 0, debug = 1, k;   
	char canFileName[1024], hitFileName[1024], parmFileName[1024];  
	char token[1024], listFileName[1024], outFileName[1024]; 
	char plotFileName[1024];
	

	while((++narg) < argc) {

		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}       		
		else if(strcmp(argv[narg], "-list") == 0) {
			narg++; strcpy(listFileName, argv[narg]);
		}
 		else if(strcmp(argv[narg], "-o") == 0) {
			narg++; strcpy(outFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-plot") == 0) {
			narg++; strcpy(plotFileName, argv[narg]);
		}
	}  


	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);

	ofstream outFile(outFileName);
	

	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s);    
	
	FeatureStats stats(plotFileName);
	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(token, tokenizer.NextToken());   

		if(strcmp(token, "[") == 0) { 
			
			strcpy(hitFileName, "");
			strcpy(canFileName, "");						
		}
		else if(strcmp(token, "-outCan") == 0) {
			strcpy(canFileName, tokenizer.NextToken());
		}	
		else if(strcmp(token, "-hit") == 0) {
			strcpy(hitFileName, tokenizer.NextToken());
		}			
		else if(strcmp(token, "]") == 0) {    
			
					
			// basename is added to name of output files.
			char baseName[1024];	
			strcpy(baseName, canFileName);  
			Util::stripDirectory(baseName);    
			Util::stripExtension(baseName);    
	
			Can can(canFileName);   
			
			if(!can.isValid()) continue; 
			
			Hit hit(hitFileName);   
			
			for(long k = 0; k < can.getNCandidates(); k++) { 
			
				cout << "\n" << baseName << setw(7) << k << flush; 
			
				Roi roi; 
				roi.setName(baseName);      
			
				can.readNextCandidate(roi);  	
			
				stats.addRoi(roi, hit);     
																
				outFile << "\n" << roi << endl;   			
			}   		    				
		}		
		

	}   

     outFile << "\n\n" << stats << endl;  
     
     stats.histograms();
     

	return 0;
} 

