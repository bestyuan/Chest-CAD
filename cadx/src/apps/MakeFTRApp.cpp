//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "cadx.h"    
#include "Ftr.h"  
#include "Tokenizer.h"

long isInList(long index);
void addToList(long index);
 
long indexList[1024], indexListSize;

int main(int argc, char* argv[])
{                    
	short narg = 0, debug = 1, k;   
	char canFileName[1024], hitFileName[1024], parmFileName[1024];  
	char token[1024], listFileName[1024], ftrFileName[1024]; 
	char plotFileName[1024], canDir[1024], hitDir[1024], imgName[1024];
	char featureName[1024], imgDir[1024], name[1024];
	double featureThreshold = 0;

	

	while((++narg) < argc) {


		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-list") == 0) {
			narg++; strcpy(listFileName, argv[narg]);
		}
 		else if(strcmp(argv[narg], "-outFile") == 0) {
			narg++; strcpy(ftrFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-canDir") == 0) {
			narg++; strcpy(canDir, argv[narg]);
		}   
		else if(strcmp(argv[narg], "-hitDir") == 0) {
			narg++; strcpy(hitDir, argv[narg]);
		}   

	}  


	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s);   

	char title[1024];
	sprintf(title, "List=%s,CanDir=%s", listFileName, canDir);

	long nclasses = 2;

	char** clabels;
	clabels = new char*[nclasses];

	for(long i = 0; i < nclasses; i++) {clabels[i] = new char[1024];}
	strcpy(clabels[0], "nodule");
	strcpy(clabels[1], "nonnodule");

	long nsamples[2];
	nsamples[0] = 0;
	nsamples[1] = 0;

	double apriori[2];
	apriori[0] = 1.0;
	apriori[1] = 1.0;

	long nfeatures = 0;
	char** flabels = NULL;


	Ftr ftr(ftrFileName);

	long classNum;
	

	
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
		
		indexListSize = 0;
			
		for(long k = 0; k < can.getNCandidates(); k++) { 
			
			cout << "\n" << baseName << setw(7) << k << flush; 
			
			Roi roi; 
			can.readNextCandidate(roi); 

			// Get info needed for the header.
			if(nfeatures == 0) {

				nfeatures = roi.getNFeatures();

				flabels = new char*[nfeatures];

				for(long i = 0; i < nfeatures; i++) {

					flabels[i] = new char[1024];
					strcpy(flabels[i], roi.getFeatureLabel(i));
				}

				ftr.writeHeader(title, nclasses, clabels, nsamples, apriori, nfeatures, flabels);
			}
			

			long gt = Util::GROUNDTRUTH_UNKNOWN;
			long index = -1;

			gt = hit.getGroundtruth(roi.getAbsCandidateCol(), roi.getAbsCandidateRow(), index);

			sprintf(name, "%s_can%d_%s", baseName, roi.getIndex(), Util::getGtDescription(gt));   
			cout << "\n\n***Processing " << name << ", gt index= " << index << flush;   
         
			if(index == -1) {;}
			else if(isInList(index)) {cout << "\nRedundent hit found" << flush; continue;}
			else addToList(index);


			
			roi.setName(name);

			if(gt == Util::GROUNDTRUTH_UNKNOWN) classNum = 1;
			else if(gt == Util::GROUNDTRUTH_NEGATIVE) classNum = 1;
			else if(gt == Util::GROUNDTRUTH_POSSIBLE) classNum = 0;
			else if(gt == Util::GROUNDTRUTH_PROBABLE) classNum = 0;
			else if(gt == Util::GROUNDTRUTH_DEFINITE) classNum = 0;	

			ftr.writeCandidate(roi, classNum);

				 			
		}  // Next ROI		    							
			
		

	}  // Next Can file

 



	ftr.updateHeader();



	if(flabels != NULL) {
		for(long i = 0; i < nfeatures; i++) delete[] flabels[i];
		delete[] flabels;
	}

	if(clabels != NULL) {
		for(long i = 0; i < nclasses; i++) delete[] clabels[i];
		delete[] clabels;
	}
	

	return 0;
} 


long isInList(long index) {
   for(long i = 0; i < indexListSize; i++) {
      if(indexList[i] == index) return 1;
   }
   return 0;
}


void addToList(long index) {
   indexListSize++;
   indexList[indexListSize - 1] = index;
}

