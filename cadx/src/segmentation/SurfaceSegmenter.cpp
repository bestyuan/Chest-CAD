//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "SurfaceSegmenter.h"
#include "PolyFit.h" 
#include "PeakDetector.h" 
#include "RegionLabel.h"
#include "ConvexHull.h"
#include "BlurFilter.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemMorphological.h"


using namespace CADX_SEG;

 
CADX_SEG::SurfaceSegmenter::SurfaceSegmenter() {
	initialize();
	polyFit.initialize(4);
}


void CADX_SEG::SurfaceSegmenter::initialize() {
	colSeed = 0;
	rowSeed = 0;
	debug = 1;
	strcpy(outputDir, "./");
	strcpy(ID, "SurfaceSegmenter");
	initialRadius = 10; 
	iteration = 0;
	logFile = NULL;
	area = 0;  
	accumError = 0;
	maxError = 100;
	maxPointError = 300;
	minCodeValue = 1;
	maxIterations = 5;
	dilateKernelSize = 11;
	erosionKernelSize = 15;
	newArea = 0;
	newError = 0;
	maxNewError = 30;
}

  
IemTImage<unsigned char> CADX_SEG::SurfaceSegmenter::segment(IemTImage<short>& _img, IemTImage<unsigned char>& _imgAnatomyMap,
 long _colSeed, long _rowSeed) {

	char outFileName[1024];

	img = _img;
	imgAnatomyMap = _imgAnatomyMap;
	
	colSeed = _colSeed;
	rowSeed = _rowSeed;
	

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_img.tif", outputDir, ID);
		iemWrite(img, outFileName);
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_imgAnatomyMap.tif", outputDir, ID);
		iemWrite(imgAnatomyMap, outFileName);
	}

	imgSegMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgSegMap = 0;
	
	imgSupportMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgSupportMap = 0;

	imgFit = IemTImage<short>(1, img.rows(), img.cols());
	imgFit = 0;
	
	imgError = IemTImage<short>(3, img.rows(), img.cols());
	imgError = 0;

	if(img[0][rowSeed][colSeed] < minCodeValue) return imgSegMap;

	if(logFile != NULL) *logFile << "\nSufaceSegmenter: Initialize fit";      

	initializeSupportMap();

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_Support.tif", outputDir, ID);
		iemWrite(imgSupportMap, outFileName);
	}

	BlurFilter blurFilter(11, 11, 3.0);	
	IemTImage<short> imgBlurred = blurFilter.filter(img);
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_blurred.tif", outputDir, ID);
		iemWrite(imgBlurred, outFileName);
	}
	
	PeakDetector peakDetector;
	peakDetector.detect(imgBlurred, imgSupportMap);
     
     if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_Peaks.tif", outputDir, ID);
		iemWrite(peakDetector.getAnnotatedImage(), outFileName);
	}

//	newFit();
	

	
//	iteration++;
//	decomposeSegMap2();
	


/*
	// Stop if the fit error is too high.
     if(getStdError() > maxError) {
		if(logFile != NULL) *logFile << "\nSufaceSegmenter: End because fit error= "
		 << getStdError() << " too high.";
		 
		imgSegMap = 0;
          return imgSegMap;
     }

	for(long i = 1; i <= maxIterations; i++) {
		iteration++;
		
		*logFile << "\nSufaceSegmenter: " << "@Iteration " << iteration;

	//	erosionKernelSize *= 1.5;
		decomposeSegMap();
		
		if(debug) {
			sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_%d.tif", outputDir, ID, iteration);
			iemWrite(imgSegMap, outFileName);
		}
		
		newFit();

		
		// Create dilated version of seg map..
		dilateSegMap(2, 7);

		// Grow region and stop if no new pixels were added.
		*logFile << "\nSufaceSegmenter: Grow Region";
		if(growRegion() == 0) {
		     if(logFile != NULL) *logFile << "\nSufaceSegmenter: End growing no new pixels";
		     break;
		}

		// Perform a new fit to lower error.
		if(getStdError() > maxError) {
		     if(logFile != NULL) *logFile << "\nSufaceSegmenter: New fit";
		     newFit();
		}

          // Stop if the new fit doesn't lower the average error to max value or less.
          if(getStdError() > maxError) {
 		     if(logFile != NULL) *logFile << "\nSufaceSegmenter: End because fit error= "
			  << getStdError() << " too high.";
          	break;
          }

	}
	
	
	IemTImage<short> imgFitFirstDer = polyFit.getFitFirstDer();
	sprintf(outFileName, "%s/%s_SurfaceSegmenter_first_der.tif", outputDir, ID);
	iemWrite(imgFitFirstDer, outFileName);
*/
	return imgSegMap;
}

 
void CADX_SEG::SurfaceSegmenter::initializeSupportMap() {

	char outFileName[1024];

	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {
			if(img[0][y][x] >= minCodeValue && imgAnatomyMap[0][y][x] == 255) {
			     imgSupportMap[0][y][x] = 255;
			}
	}}
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_Support_initial.tif", outputDir, ID);
		iemWrite(imgSupportMap, outFileName);
	}

	short d = erosionKernelSize;
  
	// Erode bridges between regions.
	imgSupportMap = iemErode(imgSupportMap, d, d);
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_Support_eroded.tif", outputDir, ID);
		iemWrite(imgSupportMap, outFileName);
	}

	// Label connected regions.
	CADX_SEG::RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgSupportMap, 0, 255, 10000);
	 
	IemTImage<unsigned char> imgLabeled = regionLabel.getLabeledImage();
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SupportMap_labeled.tif", outputDir, ID);
		iemWrite(imgLabeled, outFileName);
	}

	// Retain only the connected region that is closest to the seed.
	long label = 0;
	double d2min = DBL_MAX;

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgSupportMap[0][r][c] != 0) {
				double delCol = c - colSeed;
				double delRow = r - rowSeed;
				double d2 = delCol * delCol + delRow * delRow;
				if(d2 < d2min) {d2min = d2; label = imgLabeled[0][r][c];}
			}
	}} 
	
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgLabeled[0][r][c] == label) imgSupportMap[0][r][c] = 255;
			else imgSupportMap[0][r][c] = 0;
	}}
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_Support_selected.tif", outputDir, ID);
		iemWrite(imgSupportMap, outFileName);
	}

	// Undo the affect of erosion.
	imgSupportMap = iemDilate(imgSupportMap, d, d);
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_Support_dilated.tif", outputDir, ID);
		iemWrite(imgSupportMap, outFileName);
	}
}

void CADX_SEG::SurfaceSegmenter::initialFit() {

	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {
		                                            	
		     double dx = x - colSeed;
		     double dy = y - rowSeed;

			double d = sqrt(dx * dx + dy * dy);
			
			if(imgSupportMap[0][y][x] == 255 && d <= initialRadius) {
			     imgSegMap[0][y][x] = 255;
			}

	}}

	polyFit.calcFit(img, imgSegMap, 1);
	
	area = 0;
	accumError = 0;
	accumError2 = 0;

	for(x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {
		     if(imgSegMap[0][y][x] != 0) {
				short fit = polyFit.getValueAt(x, y);
				short error = img[0][y][x] - fit;
				if(debug) {
				     imgFit[0][y][x] = fit;
					if(error >= 0) imgError[0][y][x] = error;
					else imgError[1][y][x] = -error;
				}
				area++; 
				accumError += error;
				accumError2 += error * error;
			}
	}}
}

void CADX_SEG::SurfaceSegmenter::dilateSegMap(short aggregate, short kernelSize) {

	char outFileName[1024];

	imgSegMapDilated = iemAggregateDown(imgSegMap, aggregate, 0);
	imgSegMapDilated = iemDilate(imgSegMapDilated, kernelSize, kernelSize);
	imgSegMapDilated = iemInterpolate(imgSegMapDilated, img.rows(), img.cols());

	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {
		 	if(imgSupportMap[0][y][x] != 255) imgSegMapDilated[0][y][x] = 0;
	}}

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_dilated_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegMapDilated, outFileName);
	}
}

long CADX_SEG::SurfaceSegmenter::growRegion() {

	char outFileName[1024];

	long d = dilateKernelSize;
	newArea = 0;
	newError = 0;
	newError2 = 0;

	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {

		     if(imgSupportMap[0][y][x] == 255 && imgSegMap[0][y][x] != 255
			 && imgSegMapDilated[0][y][x] != 0) {
		                                                                   	
				short fit = polyFit.getValueAt(x, y);
				short error = img[0][y][x] - fit;

				if(debug) {
	 			     imgFit[0][y][x] = fit;
					if(error >= 0) imgError[0][y][x] = error;
					else imgError[1][y][x] = -error;
				}

				if(abs(error) <= maxPointError) {
					imgSegMap[0][y][x] = 255;
					newArea++;
					newError += error;
					newError2 += error * error;
				}
			} 
	}}
	
	area += newArea;
	accumError += newError;
	accumError2 += newError2;

	writeInfo();

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegMap, outFileName);
	}
	
//	*logFile << "\nSufaceSegmenter: area= " << area << ", avgError= " << getError() << ", newArea= " << newArea << ", avgNewError= " << getNewError() << endl;

     return newArea;
}

 
void CADX_SEG::SurfaceSegmenter::newFit() {

	char outFileName[1024];
/*
	ConvexHull convexHull(40);
	IemTImage<unsigned char> imgSegConvexHull = convexHull.calculate(imgSegMap);
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_hull_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegConvexHull, outFileName);
	}
*/
	polyFit.calcFit(img, imgSegMap, colSeed, rowSeed, 1);

	if(debug) { 
		imgFit = 0;  
		imgError = 0;
	} 
	
	area = 0;
	accumError = 0;
	accumError2 = 0;
	newArea = 0;
	newError = 0;
	newError2 = 0;
	
	for(long x = 0; x < img.cols(); x++) {
		for(long y = 0; y < img.rows(); y++) {

		     if(imgSegMap[0][y][x] != 0) {
				short fit = polyFit.getValueAt(x, y);
				short error = img[0][y][x] - fit;

				area++; 
				accumError += error;
				accumError2 += error * error;
				
				if(debug) {
				     imgFit[0][y][x] = fit;
					if(error >= 0) imgError[0][y][x] = error;
					else imgError[1][y][x] = -error;
				}
			}
	}}
	
	writeInfo();
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_fit_%d.tif", outputDir, ID, iteration);
		iemWrite(imgFit, outFileName);
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_error_%d.tif", outputDir, ID, iteration);
		iemWrite(imgError, outFileName);
	}
}
     
void CADX_SEG::SurfaceSegmenter::writeImages() {

	char outFileName[1024];

	sprintf(outFileName, "%s/%s_surface_seg_%d.tif", outputDir, ID, iteration);
	iemWrite(imgSegMap, outFileName);
	
 	IemTImage<short> imgWholeFit = polyFit.getFitImage();
	sprintf(outFileName, "%s/%s_surface_fit_%d.tif", outputDir, ID, iteration);
	iemWrite(imgWholeFit, outFileName);

	sprintf(outFileName, "%s/%s_surface_error_%d.tif", outputDir, ID, iteration);
	iemWrite(imgError, outFileName);

	IemTImage<short> imgFirstDer = polyFit.getFitFirstDer();
	sprintf(outFileName, "%s/%s_surface_first_der_%d.tif", outputDir, ID, iteration);
	iemWrite(imgFirstDer, outFileName);
}


void CADX_SEG::SurfaceSegmenter::writeInfo() {

	if(logFile == NULL) return;

	polyFit.write(*logFile);

	*logFile << "\nSufaceSegmenter: area= " << area << ", avgError= " << getMeanError()
	 << ", stdError= " << getStdError() << "\n, areaNew= " << newArea
	 << ", avgNewError= " << getNewMeanError() << ", stdNewError= " 
	 << getNewStdError() << endl;
}
 

double CADX_SEG::SurfaceSegmenter::getMeanError() {
	if(area <= 0) return 0.0;
	return (double)accumError / (double)area;
}
	
double CADX_SEG::SurfaceSegmenter::getStdError() {
	if(area <= 0) return 0.0;
	double meanError = getMeanError();
	return sqrt((double)accumError2 / (double)area - meanError * meanError);
}

double CADX_SEG::SurfaceSegmenter::getNewMeanError() {
	if(newArea <= 0) return 0.0;
	return (double)newError / (double)newArea;
}
	
double CADX_SEG::SurfaceSegmenter::getNewStdError() {
	if(newArea <= 0) return 0.0;
	double meanError = getNewMeanError();
	return sqrt((double)newError2 / (double)newArea - meanError * meanError);
}


void CADX_SEG::SurfaceSegmenter::decomposeSegMap() {

	char outFileName[1024];

	short d = erosionKernelSize;
  
	// Erode bridges between regions.
	imgSegMap = iemErode(imgSegMap, d, d);
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_eroded_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegMap, outFileName);
	}

	// Label connected regions.
	CADX_SEG::RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgSegMap, 0, 255, 10000);
	 
	IemTImage<unsigned char> imgLabeled = regionLabel.getLabeledImage();
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_labeled_%d.tif", outputDir, ID, iteration);
		iemWrite(imgLabeled, outFileName);
	}

	// Retain only the connected region that is closest to the seed.
	long label = 0;
	double d2min = DBL_MAX;

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgSegMap[0][r][c] != 0) {
				double delCol = c - colSeed;
				double delRow = r - rowSeed;
				double d2 = delCol * delCol + delRow * delRow;
				if(d2 < d2min) {d2min = d2; label = imgLabeled[0][r][c];}
			}
	}} 
	
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgLabeled[0][r][c] == label) imgSegMap[0][r][c] = 255;
			else imgSegMap[0][r][c] = 0;
	}}
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_selected_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegMap, outFileName);
	}
/*
	// Undo the affect of erosion.
	imgSegMap = iemDilate(imgSegMap, d, d);
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_dilated_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegMap, outFileName);
	}
*/
}

 
void CADX_SEG::SurfaceSegmenter::decomposeSegMap2() {

	char outFileName[1024];  

	for(long c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgError[1][r][c] >= 1) {
				imgSegMap[0][r][c] = 0;
			}
	}}
  
	// Label connected regions.
	CADX_SEG::RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgSegMap, 0, 255, 10000);
	 
	IemTImage<unsigned char> imgLabeled = regionLabel.getLabeledImage();
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_labeled_%d.tif", outputDir, ID, iteration);
		iemWrite(imgLabeled, outFileName);
	}  
 
	// Retain only the connected region that is closest to the seed.
	long label = 0;
	double d2min = DBL_MAX;
 
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgSegMap[0][r][c] != 0) {
				double delCol = c - colSeed;
				double delRow = r - rowSeed;
				double d2 = delCol * delCol + delRow * delRow;
				if(d2 < d2min) {d2min = d2; label = imgLabeled[0][r][c];}
			}
	}} 
	
	for(c = 0; c < img.cols(); c++) {
		for(long r = 0; r < img.rows(); r++) {
			if(imgLabeled[0][r][c] == label) imgSegMap[0][r][c] = 255;
			else imgSegMap[0][r][c] = 0;
	}}
	
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceSegmenter_SegMap_selected_%d.tif", outputDir, ID, iteration);
		iemWrite(imgSegMap, outFileName);
	}

}





 

	

