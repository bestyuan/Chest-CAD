//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

 
#include "Tokenizer.h"  
#include "UserAgent.h"
#include "CadxParm.h"
#include "ImageReader.h"
#include "ImageUtil.h"
#include "CadxParm.h"
#include "Hit.h"
#include "Can.h"
#include "Roi.h"
#include "Icd.h"
#include "Annotator.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;



int main(int argc, char* argv[])
{                    
	short narg = 0;
	long log = 0, debug = 0, hitFile = 0;
	char imgFileName[1024], maskFileName[1024], parmFileName[1024];
	char hitFileName[1024], buffer[1024];
	char outCanFileName[1024], logFileName[1024], outDir[1024];


	strcpy(imgFileName, "");
	strcpy(hitFileName, "");
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
		else if(strcmp(argv[narg], "-debug") == 0) {
			debug = 1;
		}
		else if(strcmp(argv[narg], "-outDir") == 0) {
			narg++; strcpy(outDir, argv[narg]);
		}
		else {
			cerr << "\nBad command option: " << argv[narg] << endl;
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

	ImageReader reader;
	IemTImage<short> img = reader.read(imgFileName);
	userAgent.writeDebugImage(img, "Input");

 	IemTImage<unsigned char> imgMask;
	imgMask << maskFileName;
	userAgent.writeDebugImage(imgMask, "Mask");
 	
 	long nInitialCandidates = CadxParm::getInstance().getMaxCandidates();
 	Roi* roiArray = new Roi[nInitialCandidates];

	Icd icd;
	icd.setUserAgent(userAgent);
	icd.run(img, imgMask, roiArray, nInitialCandidates);
	
	Can can;
 	can.open(outCanFileName, img.cols(), img.rows(), imgFileName);
 	
	for(long k = 0; k < nInitialCandidates; k++) {
		if(roiArray[k].isValid()) {can.writeCandidate(roiArray[k]);}
	}
	
	
	if(1) { //userAgent.getDebug()) {
		Hit hit(hitFileName);
		Can annotateCan(outCanFileName); 
		short min = CadxParm::getInstance().getMinCodeValue();
		short max = CadxParm::getInstance().getMaxCodeValue();
		bool invert = false;
		
		IemTImage<short> imgProprocessed = iemInterpolate(icd.getPreprocessedImage(), img.rows(), img.cols());

		Annotator annotator(imgProprocessed, min, max, invert);
		annotator.addMask(imgMask);
		annotator.addGroundTruth(hit); 
		annotator.addCandidates(annotateCan);
//		userAgent.writeImage(annotator.getAnnotatedImage(), "annotated", "tif");
	}

	if(roiArray != NULL) delete[] roiArray;

 	return 0;
}

