//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "Can.h" 
#include "Hit.h"
#include "Roi.h"
#include "CadxParm.h"
#include "Annotator.h"
#include "Tokenizer.h"  
#include "Report.h"
#include "ImageReader.h"
#include "Iem/Iem.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;


int main(int argc, char* argv[])
{                    
	short narg = 0;
	char inFileName[1024], outFileName[1024], canFileName[1024], reportFileName[1024];
	char hitFileName[1024], parmFileName[1024];
	double minProbability = 0;
	bool circles = false;


	strcpy(hitFileName, "");


	while((++narg) < argc) {

		if(strcmp(argv[narg], "-i") == 0) {
			narg++; strcpy(inFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-o") == 0) {
			narg++; strcpy(outFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-can") == 0) {
			narg++; strcpy(canFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-hit") == 0) {
			narg++; strcpy(hitFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-report") == 0) {
			narg++; strcpy(reportFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-minProbability") == 0) {
			narg++; minProbability = atof(argv[narg]);
		}
		else if(strcmp(argv[narg], "-circles") == 0) {
			circles = true;
		}
	}  
	
	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);

	ImageReader reader;
	IemTImage<short> img = reader.read(inFileName);

	Can can(canFileName);
	
	short min = CadxParm::getInstance().getMinCodeValue();
	short max = CadxParm::getInstance().getMaxCodeValue();
	bool invert = CadxParm::getInstance().doInversion();
	Annotator annotator(img, min, max, invert);

	if(circles) {annotator.circleCandidates(can, minProbability);}
	else {annotator.addCandidates(can, minProbability);}

	if(strcmp(hitFileName, "") != 0) {
		Hit hit(hitFileName);
		annotator.addGroundTruth(hit); 
	}
	
	outFileName << annotator.getAnnotatedImage();

	Report report(reportFileName);
	report.makeReport(can, minProbability);


	return 0;
}
