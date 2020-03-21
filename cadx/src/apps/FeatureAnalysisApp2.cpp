//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "CadxParm.h"      
#include "FeatureStats.h" 
#include "Tokenizer.h"  
#include "Can.h"  
#include "Hit.h"  
#include "Roi.h"  


using namespace CADX_SEG;


int main(int argc, char* argv[])
{                    
	short narg = 0, debug = 1, k; 
	long nImages = 0, nDefinite = 0, nProbable = 0, nPossible = 0;
	long nDefiniteDetected = 0, nProbableDetected = 0, nPossibleDetected = 0;
	char canFileName[1024], hitFileName[1024], parmFileName[1024];
	char token[1024], listFileName[1024], outFileName[1024]; 
	char plotFileName[1024], canDir[1024], hitDir[1024], imgName[1024];
	

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
		else if(strcmp(argv[narg], "-canDir") == 0) {
			narg++; strcpy(canDir, argv[narg]);
		}   
		else if(strcmp(argv[narg], "-hitDir") == 0) {
			narg++; strcpy(hitDir, argv[narg]);
		}       
	}  


	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);

	ofstream outFile(outFileName);
	

	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s);    
	
	FeatureStats stats(plotFileName);
	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(imgName, tokenizer.NextToken());   
		cout << "\nProcessing image: " << imgName;

		Util::stripExtension(imgName);

		sprintf(hitFileName, "%s/%s.hit", hitDir, imgName, imgName);
		cout << "\nHit file: " << hitFileName;
				
		sprintf(canFileName, "%s/%s.can", canDir, imgName);
		cout << "\nCan file: " << canFileName;
			
					
		// basename is added to name of output files.
		char baseName[1024];	
		strcpy(baseName, canFileName);  
		Util::stripDirectory(baseName);    
		Util::stripExtension(baseName);    
	
		Can can(canFileName);   
			
		if(!can.isValid()) continue; 
			
		Hit hit(hitFileName); 
      
		nImages++;

		nDefinite += hit.getNumWithGroundtruth(Util::GROUNDTRUTH_DEFINITE);
		nProbable += hit.getNumWithGroundtruth(Util::GROUNDTRUTH_PROBABLE);
		nPossible += hit.getNumWithGroundtruth(Util::GROUNDTRUTH_POSSIBLE);
		
		long maxCandidates = Util::min(can.getNCandidates(), 
		 CadxParm::getInstance().getMaxCandidates());


		for(long k = 0; k < maxCandidates; k++) {
			
			cout << "\n" << baseName << setw(7) << k << flush;
			
			Roi roi; 
			roi.setName(baseName);      
			
			can.readNextCandidate(roi); 

			stats.addRoi(roi, hit);

			outFile << "\n\n";
			roi.write(outFile);
		}

		for(long j = 0; j < hit.getNumNodules(); j++) { 	
			if(hit.isDetected(j)) {
				if(hit.getNoduleStatus(j) == Util::GROUNDTRUTH_DEFINITE) {nDefiniteDetected++;}
				if(hit.getNoduleStatus(j) == Util::GROUNDTRUTH_PROBABLE) {nProbableDetected++;}
				if(hit.getNoduleStatus(j) == Util::GROUNDTRUTH_POSSIBLE) {nPossibleDetected++;}
			}
		}


	}


	outFile << "\nNumber of Images= " << nImages
	 << "\nNodules: # Definite= " << nDefinite 
	 << ", # Probable= " << nProbable 
	 << ", # Possible= " << nPossible << endl;


	outFile << "\nNodules with IC: # Definite= " << nDefiniteDetected
	 << ", # Probable= " << nProbableDetected  << endl;


	stats.write(outFile);

	stats.histograms();

	return 0;
} 

