//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "CalcFeatures.h" 
#include "Tokenizer.h"  


int main(int argc, char* argv[])
{                    
	short narg = 0, candidatesFromHitFile = 0, hitFile = 0, hitsFirst = 0;   
	char imgFileName[1024], maskFileName[1024], parmFileName[1024]; 
	char inCanFileName[1024], hitFileName[1024], cnlFileName[1024]; 
	char outCanFileName[1024];
	char token[1024], listFileName[1024], logFileName[1024]; 

	strcpy(logFileName, "cadx.log");

	while((++narg) < argc) {
		

		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}       
		else if(strcmp(argv[narg], "-list") == 0) {
			narg++; strcpy(listFileName, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-log") == 0) {
			narg++; strcpy(logFileName, argv[narg]);
		}     
		else if(strcmp(argv[narg], "-candidatesFromHitFile") == 0) {
			candidatesFromHitFile = 1; 
		} 
		else if(strcmp(argv[narg], "-hitsFirst") == 0) {
			hitsFirst = 1; 
		}
	}  
	
	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);   
	CadxParm::getInstance().setLogFileName(logFileName);
		
	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s);   
	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(token, tokenizer.NextToken());   

		if(strcmp(token, "[") == 0) { 
			
			strcpy(imgFileName, "");
			strcpy(cnlFileName, "");
			strcpy(hitFileName, "");
			strcpy(inCanFileName, "");
			strcpy(outCanFileName, "");	
			strcpy(maskFileName, "");	
			hitFile = 0;						
		}
		else if(strcmp(token, "-outCan") == 0) {
			strcpy(outCanFileName, tokenizer.NextToken());
		}	
		else if(strcmp(token, "-hit") == 0) {
			strcpy(hitFileName, tokenizer.NextToken());  
			hitFile = 1;
		}	
		else if(strcmp(token, "-img") == 0) {
			strcpy(imgFileName, tokenizer.NextToken());
		}			
		else if(strcmp(token, "-mask") == 0) {
			strcpy(maskFileName, tokenizer.NextToken());
		}	
		else if(strcmp(token, "-cnl") == 0) {
			strcpy(cnlFileName, tokenizer.NextToken());
		}	
		else if(strcmp(token, "-inCan") == 0) {
			strcpy(inCanFileName, tokenizer.NextToken());
		}				
		else if(strcmp(token, "]") == 0) { 
			
			CalcFeatures calcFeatures(imgFileName, inCanFileName, outCanFileName, hitFileName);
			
			if(candidatesFromHitFile && hitFile) calcFeatures.processHit();  
			else {
			                        
				if(hitFile && hitsFirst) calcFeatures.processCan(1);			                        
				calcFeatures.processCan();  
			}
				
		//	calcFeatures.processCnl();									
		}		
		

	}

	return 0;
}