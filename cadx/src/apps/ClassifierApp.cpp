//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Can.h"
#include "Roi.h"
#include "Culler.h"
#include "Tokenizer.h"
#include "ImageReader.h"
#include "GmlData.h"
#include "GmlClassifier.h"



using namespace CADX_SEG;


int main(int argc, char* argv[])
{
	short narg = 0, k;
	char inCanFileName[1024], outCanFileName[1024], dataFileName[1024];
	char imageSource[1024];


	while((++narg) < argc) {

		if(strcmp(argv[narg], "-i") == 0) {
			narg++; strcpy(inCanFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-o") == 0) {
			narg++; strcpy(outCanFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-data") == 0) {
			narg++; strcpy(dataFileName, argv[narg]);
		}
	}


	// Read the data for GML classifier.
	GmlData gmlData(dataFileName);
	
	// Setup the classifier.
	GmlClassifier gmlClassifier(gmlData);

	ofstream outFile("c:/luck/g.txt");
	gmlClassifier.write(outFile);


	// Read the input CAN file.
	Can inCan(inCanFileName);
	
	long nCandidates = inCan.getNCandidates();
	strcpy(imageSource,	inCan.getImageSource());
	long rows = inCan.getHeight();
	long cols = inCan.getWidth();

	// Create an array of Regions-Of-Interest to hold all candidates.
	Roi* roiArray = new Roi[nCandidates];

	// Read all candidates into a ROI and classify.
	for(k = 0; k < nCandidates; k++) {
		inCan.readNextCandidate(roiArray[k]);
		gmlClassifier.classify(roiArray[k]);
	}

	// If candidates belong to the same group (boundaries overlap)
	// remove all but the candidate with highest probability.
	// A candidate is removed by setting its probability to a 
	// negative number.
	Culler  culler(roiArray, nCandidates);
	culler.removeDuplicates();


	// Write the output CAN file.
	Can outCan(outCanFileName, cols, rows, imageSource);
	
	for(k = 0; k < nCandidates; k++) {
		outCan.writeCandidate(roiArray[k]);
	}

	// Free memory.
	if(roiArray != NULL) delete[] roiArray;

	return 0;
}
