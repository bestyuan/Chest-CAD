//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "cadx.h"       
#include "Tokenizer.h" 
#include "ImageReader.h" 
#include "Annotator.h" 
#include "Iem/Iem.h" 
#include "Iem/IemImageIO.h"  


int main(int argc, char* argv[])
{                    
	short narg = 0, debug = 1, k, positives = 1, negatives = 1;   
	char canFileName[1024], hitFileName[1024], parmFileName[1024];  
	char token[1024], listFileName[1024], outDir[1024], outCanFileName[1024]; 
	char plotFileName[1024], canDir[1024], hitDir[1024], imgName[1024];
	char featureName[1024], imgDir[1024], name[1024], testCondition[1024];
	char imgFileName[1024], annotatedImageFileName[1024];
	double featureThreshold = 0;
	

	while((++narg) < argc) {

		if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}       		
		else if(strcmp(argv[narg], "-list") == 0) {
			narg++; strcpy(listFileName, argv[narg]);
		}
 		else if(strcmp(argv[narg], "-outDir") == 0) {
			narg++; strcpy(outDir, argv[narg]);
		}
		else if(strcmp(argv[narg], "-canDir") == 0) {
			narg++; strcpy(canDir, argv[narg]);
		}   
		else if(strcmp(argv[narg], "-hitDir") == 0) {
			narg++; strcpy(hitDir, argv[narg]);
		}  
		else if(strcmp(argv[narg], "-imgDir") == 0) {
			narg++; strcpy(imgDir, argv[narg]);
		} 
		else if(strcmp(argv[narg], "-featureName") == 0) {
			narg++; strcpy(featureName, argv[narg]);
		}  
		else if(strcmp(argv[narg], "-featureThreshold") == 0) {
			narg++; featureThreshold = atof(argv[narg]);
		}  
		else if(strcmp(argv[narg], "-testCondition") == 0) {
			narg++; strcpy(testCondition, argv[narg]);
		}  
		else if(strcmp(argv[narg], "-positivesOnly") == 0) {
			narg++; positives = 1; negatives = 0;
		}  
		else if(strcmp(argv[narg], "-negativesOnly") == 0) {
			narg++; positives = 0; negatives = 1;
		}  
	}  


	// Initialize Cadx parameters.
	CadxParm::getInstance().readFile(parmFileName);
	

	ifstream s(listFileName);	    
	
	Tokenizer tokenizer(s);    
	
//	FeatureStats stats(plotFileName);
	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(imgName, tokenizer.NextToken());   
		cout << "\nProcessing image: " << imgName;

		sprintf(imgFileName, "%s/%s", imgDir, imgName);
		cout << "\nImage file: " << imgFileName;

		// basename is added to name of output files.
		char baseName[1024];	
		strcpy(baseName, imgName);  
		Util::stripExtension(baseName);    

		sprintf(annotatedImageFileName, "%s/%s.tif", outDir, baseName);
		cout << "\nAnnotated Image file: " << annotatedImageFileName;

		sprintf(hitFileName, "%s/%s.hit", hitDir, baseName);
		cout << "\nHit file: " << hitFileName;
				
		sprintf(canFileName, "%s/%s.can", canDir, baseName);
		cout << "\nCan file: " << canFileName;
			
	
		Can can(canFileName);   
			
		if(!can.isValid()) continue; 

		sprintf(outCanFileName, "%s/%s.can", outDir, baseName);
		cout << "\nOut Can file: " << canFileName;

		Can outCan(outCanFileName, can.getWidth(), can.getHeight(), imgName);   

			
		Hit hit(hitFileName);   
			
		for(long k = 0; k < can.getNCandidates(); k++) { 
			
			cout << "\n" << baseName << setw(7) << k << flush; 
			
			Roi roi; 
			can.readNextCandidate(roi); 
			
			if(!roi.hasFeature(featureName)) {cout << "\nError: Feature " << featureName << " not found"; exit(1);}
			
			long gt = Util::GROUNDTRUTH_UNKNOWN;	 

			gt = hit.getGroundtruth(roi.getAbsCandidateCol(), roi.getAbsCandidateRow()); 

			if(!positives && (gt == Util.GROUNDTRUTH_DEFINITE || gt == Util.GROUNDTRUTH_PROBABLE)) continue;
			if(!negatives && (gt == Util.GROUNDTRUTH_NEGATIVE)) continue;
			
			sprintf(name, "%s_can%d_%s", baseName, roi.getIndex(), Util::getGtDescription(gt));   
			cerr << "\n\n***Processing " << name << flush;    
			
			roi.setName(name); 

			long process = 0;
			double value = roi.getFeatureValue(featureName);

			if(!(strcmp(testCondition, "ge") == 0 || strcmp(testCondition, "lt") == 0)) {
			  cerr << "\n\nValid test conditions are ge (greater or equal to) or lt (less then)." << endl; exit(1);
			}
			
			if(strcmp(testCondition, "ge") == 0 && value >= featureThreshold) process = 1;
			else if(strcmp(testCondition, "lt") == 0 && value < featureThreshold) process = 1;
			
			if(process) {

				cerr << "\n\tWriting ROI" << flush;

				// Add directory to ROI sourceImageName.
				sprintf(name, "%s/%s", imgDir, roi.getSourceImageName());  
				roi.setSourceImageName(name);

				// Use the sourceImageName field to open the source image
				// and create the cropped image.
		//		roi.setSourceImage();

				sprintf(name, "%s/%s.tif", outDir, roi.getName());
			//	name << roi.getAnnotated();

				outCan.writeCandidate(roi);
																
				cout << "\n" << roi << endl;  
			}
				 			
		}  // Next ROI	


		outCan.close();


		Can annotateCan(outCanFileName);   

		IemTImage<short> img;
		ImageReader reader;
		img = reader.read(imgFileName);

		Annotator annotator(img);
		annotator.addGroundTruth(hit);
		annotator.addCandidates(annotateCan);


		IemTImage<unsigned char> imgAnnotated = annotator.getAnnotatedImage();
		
		imgAnnotated >> annotatedImageFileName;
			
		

	}  // Next image

 
     

	return 0;
} 

