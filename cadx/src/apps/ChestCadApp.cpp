//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//
#include <stdio.h>
#include "Tokenizer.h"  
#include "UserAgent.h"
#include "CadxParm.h"
#include "ImageReader.h"
#include "ImageUtil.h"
#include "CadxParm.h"
#include "GmlData.h"
#include "GmlClassifier.h"
#include "Hit.h"
#include "Can.h"
#include "Roi.h"
#include "Icd.h"
#include "Culler.h"
#include "Annotator.h"
#include "Report.h"
#include "SegFeatures.h"
#include "LungMaskMaker.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemInterpolate.h"


using namespace CADX_SEG;



int main(int argc, char* argv[])
{        
	
	typedef struct 
	{
	//CPoint centerPoint;
	int x;
	int y;

	} CPoint;

	HINSTANCE DLLInst;
	//////////////////////
   FILE * pFile;
   int n;
   pFile = fopen ("myfile.txt","a");
//////////////////////
	short narg = 0;
	long log = 0, debug = 0, hitFile = 0;
	char imgFileName[1024], imgFileNameribsup[1024], maskFileName[1024], parmFileName[1024];
	char classDataFileName[1024], baseName[1024], tag[256], hitFileName[1024];
	char outCanFileName[1024], logFileName[1024], outDir[1024];
	char outFileName[1024], reportFileName[1024], truthDir[1024];
	char lungParmFileName[1024], rightLungFileName[1024], leftLungFileName[1024];
	bool onlyTP = false, onlyIC = false, showAll = false;
	double minProbability = 0.50;
//////////////////////////////////////////////////////////////
	strcpy(imgFileName, "");
	strcpy(classDataFileName, "");
	strcpy(outCanFileName, "");	
	strcpy(maskFileName, "");	
	strcpy(logFileName, "");
	strcpy(hitFileName, "");
	strcpy(truthDir, "");


	while((++narg) < argc) {

		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}       
		else if(strcmp(argv[narg], "-log") == 0) {
			log = 1;
		}
		else if(strcmp(argv[narg], "-classData") == 0) {
			narg++; strcpy(classDataFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-truth") == 0) {
			narg++; strcpy(truthDir, argv[narg]);
			hitFile = 1;
		}	
		else if(strcmp(argv[narg], "-i") == 0) {
			narg++; strcpy(imgFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-iribsup") == 0) {
			narg++; strcpy(imgFileNameribsup, argv[narg]);
		}
		else if(strcmp(argv[narg], "-lungParm") == 0) {
			narg++; strcpy(lungParmFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-leftLung") == 0) {
			narg++; strcpy(leftLungFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-rightLung") == 0) {
			narg++; strcpy(rightLungFileName, argv[narg]);
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
		else if(strcmp(argv[narg], "-onlyTP") == 0) {
			onlyTP = true;
		}
		else if(strcmp(argv[narg], "-onlyIC") == 0) {
			onlyIC = true;
		}
		else if(strcmp(argv[narg], "-showAll") == 0) {
		showAll = true;
	}
		else {
			cerr << "\nBad command option: " << argv[narg] << endl;
			return 1;
		}
	}
//////////////////////////////////////////////////////////////////////
	UserAgent userAgent;
	userAgent.setOutputDir(outDir);
	userAgent.setDebug(debug);
	userAgent.setLog(log);

	strcpy(baseName, imgFileName);
	Util::stripDirectory(baseName);
	Util::stripExtension(baseName);
	userAgent.setTag(baseName);

	sprintf(logFileName, "%s/%s.log", outDir, baseName);
	ofstream logFile(logFileName);
	userAgent.setOutStream(logFile);

	ImageReader reader;
	IemTImage<short> img1 = reader.read(imgFileName);
	userAgent.writeDebugImage(img1, "Input");
	IemTImage<short> img = iemInterpolate(img1, 2048, 2048);

	ImageReader reader1;
	IemTImage<short> img1ribsup = reader1.read(imgFileNameribsup);
	userAgent.writeDebugImage(img1ribsup, "Inputribsup");
	IemTImage<short> imgribsup = iemInterpolate(img1ribsup, 2048, 2048);
//////////////////////////////////////////////////////////////////

	sprintf(hitFileName, "%s/%s.hit", truthDir, baseName);
	Hit hit(hitFileName);
	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName); 
	// Read the radiograph.
	
	// Automatically set the maximum code value.
	long maxCodeValue = pow(2, reader.getBitDepth()) - 1;
	CadxParm::getInstance().setMaxCodeValue(maxCodeValue);

	// Resize the image so that the pixel spacing is equal to the aim.
//	double resize = CadxParm::getInstance().getAimPixelSpacing() / reader.getXSpacing();
    double resize = reader.getXSpacing() / CadxParm::getInstance().getAimPixelSpacing();
	IemTImage<short> imgResized = iemInterpolate(img, (int)(resize * img.rows()), (int)(resize * img.cols()));
	IemTImage<short> imgResizedribsup = iemInterpolate(imgribsup, (int)(resize * imgribsup.rows()), (int)(resize * imgribsup.cols()));
	userAgent.writeDebugImage(imgResized, "Input_Resized");
	userAgent.writeDebugImage(imgResizedribsup, "Input_Resizedribsup");

	// Make the lung mask.
	//////////////////////////////////////////////////////////////////////////////////
	IemTImage<unsigned char> imgMask;
	LungMaskMaker lungMaskMaker(lungParmFileName, rightLungFileName, leftLungFileName);
	lungMaskMaker.setMaxCodeValue(CadxParm::getInstance().getMaxCodeValue());

	imgMask = lungMaskMaker.makeMask(imgResized);                         
	userAgent.writeDebugImage(imgMask, "Mask");


////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////    show lung segmentation on image
/*	Boundary boundary;
	boundary.mapToBoundary(imgMask);
	IemTImage<unsigned char> lungContour=boundary.getBoundaryImage();
		for(long r = 0; r < 2094; r++) {                                                 
		                                        
			for(long c = 0; c < 2094; c++) { 
			if(lungContour[0][r][c]==255)
				imgResized[0][r][c] = 1024;

		}
	}

	IemTImage<unsigned char> imgre1 = lungMaskMaker.convertTo8Bit(imgResized);
  userAgent.writeDebugImage(imgre1, "imgwithlung");*/
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
	IemTImage<short> imgNoduleTemplate0 = CadxParm::getInstance().getNoduleTemplate(0);
	IemTImage<unsigned char> imagex0 = lungMaskMaker.convertTo8Bit(imgNoduleTemplate0);
	userAgent.writeDebugImage(imagex0, "Inputtemplate0");
    IemTImage<short> imgNoduleTemplate1 = CadxParm::getInstance().getNoduleTemplate(1);
	IemTImage<unsigned char> imagex1 = lungMaskMaker.convertTo8Bit(imgNoduleTemplate1);
	userAgent.writeDebugImage(imagex1, "Inputtemplate1");

	IemTImage<short> imgRibTemplate00 = CadxParm::getInstance().getRibTemplate(0);
	IemTImage<unsigned char> imagex00 = lungMaskMaker.convertTo8Bit(imgRibTemplate00);
	userAgent.writeDebugImage(imagex00, "Inputtemplate00");

	IemTImage<short> imgRibTemplate01 = CadxParm::getInstance().getRibTemplate(1);
	IemTImage<unsigned char> imagex01 = lungMaskMaker.convertTo8Bit(imgRibTemplate01);
	userAgent.writeDebugImage(imagex01, "Inputtemplate01");

	IemTImage<short> imgRibTemplate02 = CadxParm::getInstance().getRibTemplate(2);
	IemTImage<unsigned char> imagex02 = lungMaskMaker.convertTo8Bit(imgRibTemplate02);
	userAgent.writeDebugImage(imagex02, "Inputtemplate02");

	IemTImage<short> imgRibTemplate03 = CadxParm::getInstance().getRibTemplate(3);
	IemTImage<unsigned char> imagex03 = lungMaskMaker.convertTo8Bit(imgRibTemplate03);
	userAgent.writeDebugImage(imagex03, "Inputtemplate03");

	IemTImage<short> imgRibTemplate04 = CadxParm::getInstance().getRibTemplate(4);
	IemTImage<unsigned char> imagex04 = lungMaskMaker.convertTo8Bit(imgRibTemplate04);
	userAgent.writeDebugImage(imagex04, "Inputtemplate04");

	IemTImage<short> imgRibTemplate05 = CadxParm::getInstance().getRibTemplate(5);
	IemTImage<unsigned char> imagex05 = lungMaskMaker.convertTo8Bit(imgRibTemplate05);
	userAgent.writeDebugImage(imagex05, "Inputtemplate05");
	/////////////////////////////////////////////////

	long startTime = time(0);
 	
 	long maxCandidates = CadxParm::getInstance().getMaxCandidates();
 	Roi* roiArray = new Roi[maxCandidates];

	// Initial candidate detection.
	Icd icd;
	icd.setUserAgent(userAgent);

	cout << "\n\nICD ";
	long icdStartTime = time(0);
	long nInitialCandidates = icd.run(imgResized, imgResizedribsup, imgMask, roiArray, maxCandidates);
    IemTImage<short> imgPreprocessed=icd.getPreprocessedImage();  
	IemTImage<unsigned char> imgPre = lungMaskMaker.convertTo8Bit(imgPreprocessed);
	userAgent.writeDebugImage(imgPre, "imgproprecessed");

	long icdEndTime = time(0);
	cout << ", processing time " << icdEndTime - icdStartTime << " sec";
	/////////////////////////////////////////////////////////
	fprintf (pFile, "%d\n",icdEndTime-icdStartTime);//////////
	/////////////////////////////////////////////////////////
	if(onlyIC) return 0;

     // Calculate segmentation based features.
	SegFeatures segFeatures;
    
//	long k;                                                              
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
			//////////////////////////////////////////
			long roiEndTime = time(0);                               
			cout << ", processing time " << roiEndTime - roiStartTime << " sec";         
	}}                                                                                    


	// Assigned ROI to groups.
	if(CadxParm::getInstance().doGrouping()) {
		Culler culler(roiArray, nInitialCandidates);
		culler.assignGroups();
	}

	// Classify all ROI.

	// Read the data for GML classifier.
	GmlData gmlData(classDataFileName);
	
	// Setup the classifier.
	GmlClassifier gmlClassifier(gmlData);

	// Classify all ROI.
	for(k = 0; k < nInitialCandidates; k++) {
		gmlClassifier.classify(roiArray[k]);
	}

	// If candidates belong to the same group (boundaries overlap)
	// remove all but the candidate with highest probability.
	// A candidate is removed by setting its probability to a 
	// negative number.
	Culler  culler2(roiArray, nInitialCandidates);
	culler2.removeDuplicates();

	// Write CAN file.
	Can can;
	sprintf(outCanFileName, "%s/%s.can", outDir, baseName);
 	can.open(outCanFileName, img.cols(), img.rows(), imgFileName);
//	can.open(outCanFileName, imgResized.cols(), imgResized.rows(), imgFileName);
	for(k = 0; k < nInitialCandidates; k++) {
		if(roiArray[k].isValid()) {can.writeCandidate(roiArray[k], resize);}
//		if(roiArray[k].isValid()) {can.writeCandidate(roiArray[k], 1);}
	}
	
	long endTime = time(0);
	cout << "\n\nTotal processing time " << endTime - startTime << " sec";
///////////////////////////////////////////////////////////////
            fprintf (pFile, "%d\n",endTime-startTime);////////////
            fprintf (pFile, "\n");
	   fclose (pFile);/////////////////////////////////////////
	/////////////////////////////

	Can canAnnotate(outCanFileName);
	
	short min = CadxParm::getInstance().getMinCodeValue();
	short max = CadxParm::getInstance().getMaxCodeValue();
	bool invert = CadxParm::getInstance().doInversion();
	int maxAnnotations = CadxParm::getInstance().getMaxAnnotations();
//	Annotator annotator(img, min, max, invert);
	Annotator annotator(img, min, 1024, invert);
	annotator.setUserAgent(userAgent);

	annotator.circleCandidates(canAnnotate, minProbability, maxAnnotations);
	if(showAll) {annotator.addCandidates(canAnnotate);}

	if(hit.isValid()) {
		Hit hit(hitFileName);
		annotator.addGroundTruth(hit); 
	}
//    annotator.addMask(imgMask);
	sprintf(outFileName, "%s/%s_annotated.tif", outDir, baseName);	
	outFileName << annotator.getAnnotatedImage();

	sprintf(reportFileName, "%s/%s_report.txt", outDir, baseName);
	Report report(reportFileName);
	report.makeReport(canAnnotate, minProbability);
	


	if(roiArray != NULL) delete[] roiArray;

 	return 0;

}

