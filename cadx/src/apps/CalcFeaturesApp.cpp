//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "CalcFeatures.h" 
#include "Tokenizer.h"  
#include "UserAgent.h"
#include "CadxParm.h"


using namespace CADX_SEG;

void help();


int main(int argc, char* argv[])
{                    
	short narg = 0, candidatesFromHitFile = 0, hitFile = 0, hitsFirst = 0, onlyHits = 0;
	long onlyCandidate = -1, log = 0, debug = 0;
	double minProbability = 0;
	char imgFileName[1024], maskFileName[1024], parmFileName[1024]; 
	char inCanFileName[1024], hitFileName[1024], buffer[1024]; 
	char outCanFileName[1024], logFileName[1024], outDir[1024];

	if(argc <= 1) {help(); return 1;}

	strcpy(imgFileName, "");
	strcpy(hitFileName, "");
	strcpy(inCanFileName, "");
	strcpy(outCanFileName, "");	
	strcpy(maskFileName, "");	
	strcpy(logFileName, "");	
	

	while((++narg) < argc) {
		

		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}       
		else if(strcmp(argv[narg], "-log") == 0) {
			log = 1;
		}
		else if(strcmp(argv[narg], "-outCan") == 0) {
			narg++; strcpy(outCanFileName, argv[narg]);
		}	
		else if(strcmp(argv[narg], "-hit") == 0) {
			narg++; strcpy(hitFileName, argv[narg]);  
			hitFile = 1;
		}	
		else if(strcmp(argv[narg], "-img") == 0) {
			narg++; strcpy(imgFileName, argv[narg]);
		}			
		else if(strcmp(argv[narg], "-mask") == 0) {
			narg++; strcpy(maskFileName, argv[narg]);
		}	
		else if(strcmp(argv[narg], "-inCan") == 0) {
			narg++; strcpy(inCanFileName, argv[narg]);
		}	
		else if(strcmp(argv[narg], "-candidatesFromHitFile") == 0) {
			candidatesFromHitFile = 1; 
		} 
		else if(strcmp(argv[narg], "-hitsFirst") == 0) {
			hitsFirst = 1;
		}  
		else if(strcmp(argv[narg], "-onlyHits") == 0) {
			onlyHits = 1; 
		}
		else if(strcmp(argv[narg], "-onlyCandidate") == 0) {
			narg++; onlyCandidate = atoi(argv[narg]);
		}
		else if(strcmp(argv[narg], "-debug") == 0) {
			debug = 1;
		}
		else if(strcmp(argv[narg], "-outDir") == 0) {
			narg++; strcpy(outDir, argv[narg]);
		}
		else if(strcmp(argv[narg], "-minProbability") == 0) {
			narg++; minProbability = atof(argv[narg]);
		}
		else {
			cerr << "\nBad command option: " << argv[narg] << endl;
			help();
			return 1;
		}
	}  	
	
	UserAgent userAgent;
	userAgent.setOutputDir(outDir);
	userAgent.setDebug(debug);
	userAgent.setLog(log);
	
	strcpy(buffer, imgFileName);
	Util::stripDirectory(buffer);
	Util::stripExtension(buffer);
	userAgent.setTag(buffer);
	sprintf(logFileName, "%s/%s.log", outDir, buffer);
	ofstream logFile(logFileName);
	userAgent.setOutStream(logFile);

	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName); 

	CalcFeatures calcFeatures;
	calcFeatures.setMinProbability(minProbability);
	calcFeatures.setUserAgent(userAgent);
	calcFeatures.run(imgFileName, maskFileName, inCanFileName, outCanFileName,
	 hitFileName, candidatesFromHitFile, hitsFirst, onlyHits, onlyCandidate);

 	return 0;
}

void help() {

	char buf[1024];

	cout << "\n\nUsage:"
	 << "\n\t-parm  parameterFile        # Parameter file"
	 << "\n\t-img  imageFile             # kespr image file"
	 << "\n\t-inCan inCanFile            # Input CAN file"
	 << "\n\t-outCan inCanFile           # Output CAN file"
	 << "\n\t-log 		                 # Write log file"
	 << "\n\t-debug 		            # Write debug images"
	 << "\n\t-hit hitFile                # HIT file for image  (optional)"	
	 << "\n\t-hitsFirst                  # Process candidates in CAN file that are labeled" 
	 << "\n\t                            # definite or probable in HIT file first"	
	 << "\n\t-candidatesFromHitFile      # Use HIT file to generate candidates" 
	 << "\n" << endl;	
	
}

