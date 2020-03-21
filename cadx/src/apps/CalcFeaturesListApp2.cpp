//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "CalcFeatures.h" 
#include "CadxParm.h" 
#include "Tokenizer.h" 
#include "UserAgent.h"



using namespace CADX_SEG;


int main(int argc, char* argv[])
{                    
	short debug = 0, narg = 0, candidatesFromHitFile = 0, hitFile = 0, hitsFirst = 0, onlyHits = 0, log = 0;
	char imgFileName[1024], maskFileName[1024], parmFileName[1024]; 
	char inCanFileName[1024], hitFileName[1024], cnlFileName[1024]; 
	char outCanFileName[1024], imgName[1024];
	char imgDir[1024], inCanDir[1024], outDir[1024], hitDir[1024], maskDir[1024];
	char token[1024], listFileName[1024], logFileName[1024]; 
	double minProbability = 0;

	strcpy(logFileName, "cadx.log");
	strcpy(imgDir, "");
	strcpy(inCanDir, "");
	strcpy(outDir, "");
	strcpy(hitDir, "");
	strcpy(maskDir, "");

	while((++narg) < argc) {

		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}       
		else if(strcmp(argv[narg], "-list") == 0) {
			narg++; strcpy(listFileName, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-log") == 0) {
			log = 1;
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
		else if(strcmp(argv[narg], "-imgDir") == 0) {
			narg++; strcpy(imgDir, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-inCanDir") == 0) {
			narg++; strcpy(inCanDir, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-outDir") == 0) {
			narg++; strcpy(outDir, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-hitDir") == 0) {
			narg++; strcpy(hitDir, argv[narg]);
			hitFile = 1;
		} 
		else if(strcmp(argv[narg], "-maskDir") == 0) {
			narg++; strcpy(maskDir, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-minProbability") == 0) {
			narg++; minProbability = atof(argv[narg]);
		}
		else if(strcmp(argv[narg], "-debug") == 0) {
			debug = 1;
		}
	}  
	
	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);   

	UserAgent userAgent;
	userAgent.setOutputDir(outDir);
	userAgent.setDebug(debug);
	userAgent.setLog(log);



//	ofstream s1; s1.open("c:/tmp/test.log", ofstream::app); s1 << "\ns1 eeeeeeeee" << flush;
	
//	ofstream s2; s2.open("c:/tmp/test.log", ofstream::app); s2 << "\ns2 eeeeeeeee" << flush;
/*
	UserAgent agent1;
	agent1.setLogFileName("c:/tmp/test.log");
	agent1.getLogFile() << "\nagent1" << endl;

	UserAgent agent2 = agent1;
//	agent2.setLogFileName("c:/tmp/test.log");
	agent2.getLogFile() << "\nagent2" << endl;
*/


	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s);    
	 
	while(!tokenizer.EndOfFile()) {
		
		strcpy(imgName, tokenizer.NextToken());
		
		if(strcmp(imgName, "") == 0) break;
		
		sprintf(imgFileName, "%s/%s", imgDir, imgName);
		cout << "\n\nProcessing image: " << imgFileName;

		Util::stripExtension(imgName);
		userAgent.setTag(imgName);
		sprintf(logFileName, "%s/%s.log", outDir, imgName);
		ofstream logFile(logFileName);
		userAgent.setOutStream(logFile);  

		sprintf(hitFileName, "%s/%s.hit", hitDir, imgName);
		userAgent.getLogFile() << "\nHit file: " << hitFileName;
				
		sprintf(inCanFileName, "%s/%s.can", inCanDir, imgName);
		userAgent.getLogFile() << "\nInput Can File: " << inCanFileName;

		sprintf(outCanFileName, "%s/%s.can", outDir, imgName);
		userAgent.getLogFile() << "\nOutput Can File: " << outCanFileName;

		sprintf(maskFileName, "%s/%s.tif", maskDir, imgName);
		userAgent.getLogFile() << "\nMask file: " << maskFileName << flush;

		CalcFeatures calcFeatures;
		calcFeatures.setMinProbability(minProbability);
		calcFeatures.setUserAgent(userAgent);
		calcFeatures.run(imgFileName, maskFileName, inCanFileName, outCanFileName,
		 hitFileName, candidatesFromHitFile, hitsFirst, onlyHits);
	}

	return 0;
}
