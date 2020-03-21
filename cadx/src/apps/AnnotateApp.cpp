//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


// True Positive- white
// False Positive- cyan
// True Negative- yellow
// False Negative- magenta
// Groundtruth definite- red
// Groundtruth probable- green
// Groundtruth definite- blue

#include "Can.h" 
#include "Hit.h"
#include "Roi.h"
#include "CadxParm.h"
#include "Annotator.h"
#include "Tokenizer.h"  
#include "ImageReader.h"
#include "Iem/Iem.h"
#include "Iem/IemImageIO.h"


using namespace CADX_SEG;


int main(int argc, char* argv[])
{                    
	short narg = 0, candidatesFromHitFile = 0, hitFile = 0, hitsFirst = 0, onlyHits = 0;
	char imgFileName[1024], maskFileName[1024], parmFileName[1024]; 
	char inCanFileName[1024], hitFileName[1024], cnlFileName[1024], name[1024]; 
	char outCanFileName[1024], imgName[1024], logFileName[1024], summaryFileName[1024];
	char imgDir[1024], inCanDir[1024], outDir[1024], hitDir[1024], maskDir[1024];
	char token[1024], listFileName[1024], outFileName[1024], label[1024];
	double minProbability = 0.5, value;
	long desciption;
	long nTrueNeg = 0, nTruePos = 0, nFalseNeg = 0, nFalsePos = 0, nImages = 0, nNodules = 0;

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
		else if(strcmp(argv[narg], "-imgDir") == 0) {
			narg++; strcpy(imgDir, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-canDir") == 0) {
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
	}  
	
	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);

	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s); 
	
	sprintf(summaryFileName, "%s/summary.txt", outDir);
	ofstream summaryFile(summaryFileName);

	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(imgName, tokenizer.NextToken());   
		
		if(strcmp(imgName, "") == 0) break;
		
		sprintf(imgFileName, "%s/%s", imgDir, imgName);
		cout << "\n\nProcessing image: " << imgFileName;
		
		Util::stripExtension(imgName);

		sprintf(hitFileName, "%s/%s.hit", hitDir, imgName);
//		cout << "\nHit file: " << hitFileName;
				
		sprintf(inCanFileName, "%s/%s.can", inCanDir, imgName);
//		cout << "\nInput Can File: " << inCanFileName;


//		cout << "\nMask file: " << maskFileName;
		
		sprintf(outFileName, "%s/%s_annotated.tif", outDir, imgName);
//		cout << "\nOutput file: " <<outFileName;
		
		sprintf(logFileName, "%s/%s.log", outDir, imgName);
		ofstream outFile(logFileName);
		
		ImageReader reader;
		IemTImage<short> img = reader.read(imgFileName);
		

		Can can(inCanFileName);

		Hit hit(hitFileName);  
		
		nImages++;
		
		nNodules += hit.getNumWithGroundtruth(Util.GROUNDTRUTH_DEFINITE) 
		 + hit.getNumWithGroundtruth(Util.GROUNDTRUTH_PROBABLE);

		short minCV = CadxParm::getInstance().getMinCodeValue();
		short maxCV = CadxParm::getInstance().getMaxCodeValue();
		bool invert = CadxParm::getInstance().doInversion();

		Annotator annotator(img, minCV, maxCV, invert);

		IemTImage<unsigned char> imgAnatomicalMap;

		if(strcmp(maskDir, "") != 0) {
			sprintf(maskFileName, "%s/%s.tif", maskDir, imgName);
			imgAnatomicalMap << maskFileName;
			annotator.addMask(imgAnatomicalMap);
		}
		else {
			imgAnatomicalMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
			imgAnatomicalMap = 255;
		}

		annotator.addGroundTruth(hit); 
		
		for(long k = 0; k < can.getNCandidates(); k++) {
			Roi roi;
			can.readNextCandidate(roi);
			long gt = hit.getGroundtruth(roi.getAbsCandidateCol(), roi.getAbsCandidateRow());

			roi.setName(imgName);
			roi.setSourceImage(img, imgAnatomicalMap);

			if(roi.getProbability() >= minProbability) {
				value = roi.getFeatureValue("Shape.aspectRatio");
			//	if(value > 3.0) continue;
				value = roi.getFeatureValue("Shape.areaToHullRatio");
			//	if(value <  0.75) continue;

				if(gt == Util.GROUNDTRUTH_NEGATIVE) {
				     nFalsePos++;
					strcpy(label, "FalsePos");
					desciption = Util::FALSE_POSITIVE;
				}
				else if(gt == Util.GROUNDTRUTH_DEFINITE) {
				     nTruePos++;
					strcpy(label, "TruePos");
					desciption = Util::TRUE_POSITIVE;
				}
				else if(gt == Util.GROUNDTRUTH_PROBABLE) {
					nTruePos++;
					strcpy(label, "TruePos");
					desciption = Util::TRUE_POSITIVE;
				}

			} // Positives
			else {
				if(gt == Util.GROUNDTRUTH_NEGATIVE) {
				     nTrueNeg++;
					strcpy(label, "TrueNeg");
					desciption = Util::TRUE_NEGATIVE;
				}
				else if(gt == Util.GROUNDTRUTH_DEFINITE) {
				     nFalseNeg++;
					strcpy(label, "FalseNeg");
					desciption = Util::FALSE_NEGATIVE;
				}
				else if(gt == Util.GROUNDTRUTH_PROBABLE) {
					nFalseNeg++;
					strcpy(label, "FalseNeg");
					desciption = Util::FALSE_NEGATIVE;
				}

			} // Negatives

			if(desciption == Util::FALSE_NEGATIVE 
			 || desciption == Util::FALSE_POSITIVE
			 || desciption == Util::TRUE_POSITIVE) {
				
				outFile << "\n\n";
				roi.write(outFile);
				annotator.addRoiWithClassAndTruth(roi, desciption);
				sprintf(name, "%s/%s_can%d_%s.tif", outDir, roi.getName(), k, label);
				//	name << roi.getAnnotated();
			}
		}   		    				

		outFileName << annotator.getAnnotatedImage();
	}
	
	summaryFile << "\n\nminProbability= " << minProbability << ", nImages= " << nImages
	 << ", nNodules= " << nNodules 
	 << ", nTruePos= " << nTruePos << ", nFalsePos= " << nFalsePos
	 << ", nTrueNeg= " << nTrueNeg << ", nFalseNeg= " << nFalseNeg
	 << "\nFalse positive rate= " << (double)nFalsePos / (double)nImages
	 << "\nDetection rate= " << (double)nTruePos / (double)nNodules << endl;

	return 0;
}
