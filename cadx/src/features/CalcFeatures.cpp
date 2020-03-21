//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "CalcFeatures.h"
#include "MorphSegmenter.h"
#include "Preprocessor.h"  
#include "ImageReader.h"
#include "ImageUtil.h" 
#include "Culler.h"
#include "CadxParm.h"
#include "Annotator.h"
#include "Iem/IemCore.h" 
#include "Iem/IemImageIO.h"  


using namespace CADX_SEG; 


CalcFeatures::CalcFeatures() {
	initialize();
}

            
void CalcFeatures::initialize() {
	strcpy(imgFileName, "");
	nCandidates = 0;  
	roiArray = NULL;
	minProbability = 0;
	runtime = 0.0;
}


CalcFeatures::~CalcFeatures() {
	if(roiArray != NULL) delete[] roiArray;
}
 

void CalcFeatures::run(char* _imgFileName, char* _maskFileName, char* inCanFileName, char* _outCanFileName, char* hitFileName,
 short candidatesFromHitFile, short hitsFirst, short onlyHits, long onlyCandidate) {
 	
	nCandidates = 0;

	strcpy(imgFileName, _imgFileName);
	strcpy(maskFileName, _maskFileName);
	strcpy(outCanFileName, _outCanFileName);

	char fileName[1024];    
 
	// basename is added to name of output files.	
	strcpy(baseName,imgFileName); 
	Util::stripDirectory(baseName);
	Util::stripExtension(baseName); 
 
	ImageReader reader;  
	img = reader.read(imgFileName);
	userAgent.writeDebugImage(img, "Input");

 	imgAnatomicalMap << maskFileName;
 	
	 
	userAgent.getLogFile() << "\n\nImage " << imgFileName
     << "\nWidth= " << img.cols() << ", Height= " << img.rows()
     << "\nxSpacing= " << reader.getXSpacing() << ", ySpacing= " << reader.getYSpacing() 
	 << " (mm/pixel-to-pixel)" << endl;
	

	inCan.open(inCanFileName);
	outCan.open(outCanFileName, img.cols(), img.rows(), imgFileName);
	hit.open(hitFileName);

	roiArray = new Roi[inCan.getNCandidates()];	

	// The image needs to be scaled by this factor 	
//	double scaleFactor = CadxParm::getInstance().getRoiSpacing() / kespr.getXSpacing();  
//	cout << "\n scaleFactor: " << scaleFactor;


	long startTime = time(0);
 
	if(candidatesFromHitFile && hit.isValid()) processHit();
	else {
		if((hitsFirst || onlyHits) && hit.isValid()) processCan(1, onlyCandidate);
		if(!onlyHits) processCan(0, onlyCandidate);
	}

	if(CadxParm::getInstance().doGrouping()) {
		Culler culler(roiArray, nCandidates);
		culler.assignGroups();
	}
	
	long endTime = time(0);
	
	runtime = endTime - startTime;
	userAgent.getLogFile() << "\n\nRuntime= " << runtime << " sec";

	for(long k = 0; k < inCan.getNCandidates(); k++) {
		if(roiArray[k].isValid()) {
			outCan.writeCandidate(roiArray[k]);
	}}

	if(1) { //userAgent.getDebug()) {
		Can annotateCan(outCanFileName); 
		short min = CadxParm::getInstance().getMinCodeValue();
		short max = CadxParm::getInstance().getMaxCodeValue();
		bool invert = CadxParm::getInstance().doInversion();
		Annotator annotator(img, min, max, invert);
		annotator.addMask(imgAnatomicalMap);
		annotator.addGroundTruth(hit);
		annotator.addCandidates(annotateCan);
//		userAgent.writeImage(annotator.getAnnotatedImage(), "annotated", "tif");
	}

}
 
  
void CalcFeatures::processHit() {
			
	char name[1024], outFileName[1024];
	
		
	for(long k = 0; k < hit.getNumNodules(); k++) {     
	
		long t0 = time(0);
			
		long gt = hit.getNoduleStatus(k);
		long index = hit.getNoduleIndex(k);	

		if(gt == Util::GROUNDTRUTH_POSSIBLE) continue;

		sprintf(name, "%s_can%d_nodule_%s", baseName, index, Util::getGtDescription(gt));  
		userAgent.getLogFile() << "\n\n***Processing " << name << flush;
		cout << "\nProcessing " << name << flush;
				
		Roi roi(name, img, imgAnatomicalMap, hit.getNoduleCol(k), hit.getNoduleRow(k), gt);
		 
		roi.setSource(imgFileName);
		 
		process(roi); 
		
		roi.compareRegionToGroundtruth(hit); 
			
		outCan.writeCandidate(roi); 
		
//		userAgent.writeDebugImage(roi.getAnnotated(hit.getOutlineColArray(k), hit.getOutlineRowArray(k), 
//		 hit.getNOutlinePoints(k)), "annotated"); 
		
		userAgent.getLogFile() << "\n\n";
		roi.write(userAgent.getLogFile()); 		

		long t1 = time(0);

		userAgent.getLogFile() << "\n\nProcessing time " + (t1 - t0);

		nCandidates++;
	}   
		
}   
   
 
void CalcFeatures::processCan(long hitsOnly, long onlyCandidate) {
   	 
	char name[1024], outFileName[1024];

	inCan.reset();
						
	for(long k = 0; k < inCan.getNCandidates(); k++) { 	
	
		if(nCandidates >= CadxParm::getInstance().getMaxCandidates()) break;
 
		long t0 = time(0);

  
		if(roiArray[k].isValid()) continue;
		
		inCan.readNextCandidate(roiArray[k]);
		
		if(roiArray[k].getProbability() < minProbability) continue;
		
		// If this candidate was already processed in a previous call skip it.
 		if(wasProcessed(roiArray[k].getIndex())) continue;

		long gt = Util::GROUNDTRUTH_UNKNOWN;	 

		if(hit.isValid()) gt = hit.getGroundtruth(roiArray[k].getAbsCandidateCol(), roiArray[k].getAbsCandidateRow());
		
 		if(hitsOnly && !(gt == Util.GROUNDTRUTH_DEFINITE || gt == Util.GROUNDTRUTH_PROBABLE)) continue;
		
		roiArray[k].setValid(true); 
		roiArray[k].setGroundtruth(gt);
		roiArray[k].setSource(imgFileName);
 
		sprintf(name, "%s_can%d_%s", baseName, roiArray[k].getIndex(), Util::getGtDescription(gt));
		userAgent.getLogFile() << "\n\n***Processing " << name << flush;
		cout << "\nProcessing " << name << flush;

		if(onlyCandidate == -1) {
		     roiArray[k].setName(name);
			roiArray[k].cropImage(img, imgAnatomicalMap);
			process(roiArray[k]);
		}
		else if(roiArray[k].getIndex() == onlyCandidate) {
		     roiArray[k].setName(name);
			roiArray[k].cropImage(img, imgAnatomicalMap);
			process(roiArray[k]);
		}
		
		userAgent.writeDebugImage(roiArray[k].getCroppedImage(), "ROI");

		if(hit.isValid() && (gt == Util.GROUNDTRUTH_DEFINITE || gt == Util.GROUNDTRUTH_PROBABLE || gt == Util.GROUNDTRUTH_POSSIBLE)) {
			roiArray[k].compareRegionToGroundtruth(hit);
		}
		
		roiArray[k].write(userAgent.getLogFile());
		
		// Keep track of the index of processed candidates.
		processedCanList[k] = roiArray[k].getIndex();

		long t1 = time(0);

		cout << "\n" << name << ", processing time " << t1 - t0 << flush;

		nCandidates++;
	}   		                        				  	       

} 

 
void CalcFeatures::process(Roi& roi) {
	UserAgent roiUserAgent = userAgent;
	roiUserAgent.setTag(roi.getName());

	short minCV = CadxParm::getInstance().getMinCodeValue();
	short maxCV = CadxParm::getInstance().getMaxCodeValue(); 
	bool invert = CadxParm::getInstance().doInversion();

	Preprocessor preprocessor(minCV, maxCV, invert); 
	preprocessor.setUserAgent(roiUserAgent);
	preprocessor.apply(roi);

	MorphSegmenter segmenter;
	segmenter.setUserAgent(roiUserAgent);
	segmenter.segment(roi);
}

 
long CalcFeatures::wasProcessed(long i) {

	for(long k = 0; k < nCandidates; k++){
		if(processedCanList[k] == i) return 1;
	}

	return 0;
}



