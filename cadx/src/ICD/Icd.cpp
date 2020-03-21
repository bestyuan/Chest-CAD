//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Icd.h"
#include "Preprocessor2.h"
#include "ImageUtil.h"
#include "PeakDetector.h"
#include "MorphPreprocessor.h"
#include "MhacFeature.h"
#include "CadxParm.h"
#include "Iem/IemAggregate.h"


 
using namespace CADX_SEG;
 


Icd::Icd() {
	initialize();
}


void Icd::initialize() {
	nCandidates = 0;
	maxCandidates = 0;
	maxScale0Candidates = 40;
	maxScale1Candidates = 30;
	noduleTemplateHeight = 1.0; //0.25;
	ribTemplateHeight = 1.0; // 0.25;
}
  
 
long Icd::run(IemTImage<short>& _img, IemTImage<short>& _imgribsup, IemTImage<unsigned char>& _imgMask, Roi* _roiArray,
 long _maxCandidates) {

	roiArray = _roiArray;
	img = _img;
	imgribsup=_imgribsup;
	imgMask = _imgMask;
   	maxCandidates = _maxCandidates;

//	userAgent.writeDebugImage(img, "ICD_Input");
//	userAgent.writeDebugImage(imgMask, "ICD_InputMask");

	short minCV = CadxParm::getInstance().getMinCodeValue();
	short maxCV = CadxParm::getInstance().getMaxCodeValue();
	bool invert = CadxParm::getInstance().doInversion();

	Preprocessor2 preprocessor(minCV, maxCV, invert);
	preprocessor.setGain(1.0);
	preprocessor.setUserAgent(userAgent);
	imgPreprocessed = preprocessor.apply(img, imgMask); 

	clfCenter = preprocessor.getClfCenter();
//	userAgent.writeDebugImage(imgPreprocessed, "ICD_Preprocessed");
	
	short nRibTemplates = CadxParm::getInstance().getNRibTemplates();
	IemTImage<short>* pRibTemplates = CadxParm::getInstance().getRibTemplatesPtr();
	IemTImage<short> imgNoduleTemplate = CadxParm::getInstance().getNoduleTemplate(0);
	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	char baseTag[1024], tag[1024];
	strcpy(baseTag, userAgent.getTag());

	// Scale 0
	userAgent.getLogFile() << "\n\nScale 0 initial candidates.";
	sprintf(tag, "%s_ascale0", baseTag);
	userAgent.setTag(tag);
	
	MorphPreprocessor morphPreprocessor0(imgNoduleTemplate, 2, noduleTemplateHeight,
	 pRibTemplates, nRibTemplates, 3, ribTemplateHeight);
	morphPreprocessor0.setUserAgent(userAgent);
	morphPreprocessor0.setRibTemplateScale(1.0);
	imgDif0 = morphPreprocessor0.apply(preprocessor.getTrendCorrectedImage());

	// Get the rib image for use at scale 1.
	IemTImage<short> imgRib = morphPreprocessor0.getRibImage();

	icdFeatures0.setUserAgent(userAgent);
	icdFeatures0.setNMhacAggregations(2);
	icdFeatures0.setNMhacRadius(6);
	icdFeatures0.setMinInterPeakDistance(10);
	icdFeatures0.calculate(imgPreprocessed, imgDif0, imgMask);
	list<Point> peakList0 = icdFeatures0.getPeakList();
	peakList0.sort();
	peakList0.reverse();

  
     // Scale 1
	userAgent.getLogFile() << "\n\nScale 1 initial candidates.";
	sprintf(tag, "%s_ascale1", baseTag);
	userAgent.setTag(tag);
	
	MorphPreprocessor morphPreprocessor1(imgNoduleTemplate, 3, noduleTemplateHeight, 
	 pRibTemplates, nRibTemplates, 3, ribTemplateHeight);
	morphPreprocessor1.setUserAgent(userAgent);
	morphPreprocessor1.setRibTemplateScale(1.0);
	imgDif1 = morphPreprocessor1.apply(preprocessor.getTrendCorrectedImage(), imgRib);


	icdFeatures1.setUserAgent(userAgent);
	icdFeatures1.setNMhacAggregations(2);
	icdFeatures1.setNMhacRadius(10);
	icdFeatures1.setMinInterPeakDistance(20);
	icdFeatures1.calculate(imgPreprocessed, imgDif1, imgMask);
	list<Point> peakList1 = icdFeatures1.getPeakList();
	peakList1.sort();
	peakList1.reverse();

	userAgent.setTag(baseTag);

	makePeakList(peakList0, peakList1);

	peaksToRoi();

/*
*/
/*
*/

	if(userAgent.getDebug()) {
		IemTImage<short> imgIcd(3, img.rows(), img.cols());

		IemTImage<short> imgTmp = preprocessor.getTrendCorrectedImage();
		
		imgIcd[0] = imgTmp[0].copy();
		imgIcd[1] = imgTmp[0].copy();
		imgIcd[2] = imgTmp[0].copy(); 

		for(long i = 0; i < nCandidates; i++) {
			long col = roiArray[i].getAbsCandidateCol();
			long row = roiArray[i].getAbsCandidateRow();

			ImageUtil::makeMark(imgIcd, col, row, 4095, 0, 0, 4);
		}

//		userAgent.writeDebugImage(imgIcd, "ICD");
	}


	return nCandidates;
}


void Icd::peaksToRoi() {

	IemTImage<short> imgTmp;

	// Peaks are sorted in order of increasing probability.
	peakList.sort();
	
	// Reverse list order so ordered in decreasing probability.
	peakList.reverse();
	
	userAgent.getLogFile() << "\n\n\nSorted Peaks";

   
	long i = 0;

	for(list<Point>::iterator iter = peakList.begin(); iter != peakList.end(); iter++) {
	     userAgent.getLogFile() << "\n #" << i++ << " "; (*iter).write(userAgent.getLogFile());

		if(nCandidates < maxCandidates) {
			roiArray[nCandidates].setLocation((*iter).x, (*iter).y);
			roiArray[nCandidates].setProbability((*iter).z);
			roiArray[nCandidates].setIndex(nCandidates);
			roiArray[nCandidates].setValid(true);
			roiArray[nCandidates].setScaleNum((*iter).source);
			roiArray[nCandidates].cropImage(img, imgMask); 
			roiArray[nCandidates].cropImageribsup(imgribsup, imgMask); 

			
			double dx = (double)abs((*iter).x - clfCenter.x) / (double)(img.cols() / 2);
			double dy = -(double)((*iter).y - clfCenter.y) / (double)(img.rows() / 2);
			
			roiArray[nCandidates].addFeature("Icd.dx", dx);
			roiArray[nCandidates].addFeature("Icd.dy", dy);

			imgTmp = roiArray[nCandidates].cropRoiFromImage(imgPreprocessed);
			roiArray[nCandidates].setPreprocessedImage(imgTmp);


			if((*iter).source == 0) {
			     imgTmp = roiArray[nCandidates].cropRoiFromImage(imgDif0);
			     icdFeatures0.addToRoi(roiArray[nCandidates]);
			}
			else {
			     imgTmp = roiArray[nCandidates].cropRoiFromImage(imgDif1);
			     icdFeatures1.addToRoi(roiArray[nCandidates]);
			}
			
			roiArray[nCandidates].write(userAgent.getLogFile());

			roiArray[nCandidates++].setNoduleRibDifImage(imgTmp);
		}
	}
}


void Icd::makePeakList(list<Point>& peakList0, list<Point>& peakList1) {

	// Combine both lists.
	list<Point>::iterator iter;    

	long i = 0;
	for(iter = peakList0.begin(); iter != peakList0.end(); iter++) {
		if(i >= maxScale0Candidates) break;
		(*iter).source = 0;
		peakList.push_back(*iter);
		userAgent.getLogFile() << "\nScale0 peak #" << i++ << " "; (*iter).write(userAgent.getLogFile());
	}

	userAgent.getLogFile() << "\n";

	i = 0;
	for(iter = peakList1.begin(); iter != peakList1.end(); iter++) {
		if(i >= maxScale1Candidates) break;
		(*iter).source = 1;
		peakList.push_back(*iter);
		userAgent.getLogFile() << "\nScale1 peak #" << i++ << " "; (*iter).write(userAgent.getLogFile());
	}


}















