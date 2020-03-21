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
#include "Culler.h"
#include "Annotator.h"
#include "SegFeatures.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;



int main(int argc, char* argv[])
{                    
	short narg = 0;
	long log = 0, debug = 0, hitFile = 0;
	char imgFileName[1024], maskFileName[1024], parmFileName[1024];
	char hitFileName[1024], baseName[1024], tag[256];
	char outCanFileName[1024], logFileName[1024], outDir[1024];
	bool onlyTP = false, onlyIC = false;


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
		else if(strcmp(argv[narg], "-onlyTP") == 0) {
			onlyTP = true;
		}
		else if(strcmp(argv[narg], "-onlyIC") == 0) {
			onlyIC = true;
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
	
	Hit hit(hitFileName);
	
	strcpy(baseName, imgFileName);
	Util::stripDirectory(baseName);
	Util::stripExtension(baseName);
	userAgent.setTag(baseName);

	sprintf(logFileName, "%s/%s.log", outDir, baseName);
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

	long startTime = time(0);
 	
 	long maxCandidates = CadxParm::getInstance().getMaxCandidates();
 	Roi* roiArray = new Roi[maxCandidates];

	Icd icd;
	icd.setUserAgent(userAgent);
	
	cout << "\n\nICD ";
	long icdStartTime = time(0);
	long nInitialCandidates = icd.run(img, imgMask, roiArray, maxCandidates);
	long icdEndTime = time(0);
	cout << ", processing time " << icdEndTime - icdStartTime << " sec";
	
	if(!onlyIC) {

		SegFeatures segFeatures;
	
		for(long k = 0; k < nInitialCandidates; k++) {
			if(roiArray[k].isValid()) {
				cout << "\nProcessing ROI " << k;
			
				userAgent.getLogFile() << "\n\n**************Processing ROI " << k << "*******************";

				if(hit.isValid()) {
			   	  long gt = hit.getGroundtruth(roiArray[k].getAbsCandidateCol(), roiArray[k].getAbsCandidateRow());
					roiArray[k].setGroundtruth(gt);
					if(onlyTP && !(gt == Util.GROUNDTRUTH_DEFINITE
					 || gt == Util.GROUNDTRUTH_PROBABLE)) continue;
				}
			
				roiArray[k].setSource(baseName);

				long roiStartTime = time(0);
				userAgent.setTag(roiArray[k].getName());
				segFeatures.setUserAgent(userAgent);
				segFeatures.calculate(roiArray[k]);
				long roiEndTime = time(0);
				cout << ", processing time " << roiEndTime - roiStartTime << " sec";
			}
		}
	 
	
		if(CadxParm::getInstance().doGrouping()) {
			Culler culler(roiArray, nInitialCandidates);
			culler.assignGroups();
		}
	
	} // calc seg features

	Can can;
 	can.open(outCanFileName, img.cols(), img.rows(), imgFileName);
 	
	for(long k = 0; k < nInitialCandidates; k++) {
		if(roiArray[k].isValid()) {can.writeCandidate(roiArray[k]);}
	}

	long endTime = time(0);
	cout << "\n\nTotal processing time " << endTime - startTime << " sec";

	if(1) { //userAgent.getDebug()) {
		Can annotateCan(outCanFileName);
		short min = CadxParm::getInstance().getMinCodeValue();
		short max = CadxParm::getInstance().getMaxCodeValue();
		bool invert = false;
		IemTImage<short> imgPreprocessed = iemInterpolate(icd.getPreprocessedImage(), img.rows(), img.cols());
		Annotator annotator(imgPreprocessed, min, max, invert);
		annotator.addMask(imgMask);
		if(hit.isValid()) {annotator.addGroundTruth(hit);} 
		annotator.addCandidates(annotateCan);
		userAgent.setTag(baseName);
//		userAgent.writeImage(annotator.getAnnotatedImage(), "annotated", "tif");
	}

	if(roiArray != NULL) delete[] roiArray;

 	return 0;
}

