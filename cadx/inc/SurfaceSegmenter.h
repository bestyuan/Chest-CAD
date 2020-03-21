//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SURFACESEGMENTER_H
#define SURFACESEGMENTER_H


#include "Iem/Iem.h"    
#include "Util.h" 
#include "PolyFit.h"


namespace CADX_SEG {

  
class SurfaceSegmenter {

	protected:

	// The image to be segmented.
	IemTImage<short> img;
	
	// The fitted image.
	IemTImage<short> imgFit;
	
	// The error image.
	IemTImage<short> imgError;

	// A map of anatomy.
	IemTImage<unsigned char> imgAnatomyMap;
	
	// The segmented region.
	IemTImage<unsigned char> imgSegMap;
	
	IemTImage<unsigned char> imgSegMapDilated;
	
	// The region of support for the surface fit.
	IemTImage<unsigned char> imgSupportMap;
	
	PolyFit polyFit;
	
	long iteration;
	
	long initialRadius;

	long colSeed, rowSeed;

	// Number of pixels in the segment.
	long area;
	
	// Accumulated fit error.
	long accumError;
	long accumError2;

	// Area added in current iteration.	
	long newArea;
	
	// Error added in current iteration.
	long newError;
	long newError2;
	
	// The maximum average fit error that triggers a new fit calculation.
	double maxError;
	
	// The maximum average fit error that triggers a new fit calculation.
	double maxNewError;

	// The maximum error between pixel value and fit allowed for the pixel to be added
	// to the segment.
	short maxPointError;
	
	long maxIterations;
	
	short dilateKernelSize;
	short erosionKernelSize;

	// Minimum code value of input image that will be included in the segment.
	short minCodeValue;
	
	long debug;
	char ID[1024];
	char outputDir[1024];
	ofstream *logFile;

	public:

	SurfaceSegmenter();
	
	void setOutputDir(char* _outputDir) {strcpy(outputDir, _outputDir);}
	void setID(char* _ID) {strcpy(ID, _ID);}
	void setDebug(short _debug) {debug = _debug;}
	void setLogFile(ofstream* _logFile) {logFile = _logFile;}

	IemTImage<unsigned char> segment(IemTImage<short>& img, IemTImage<unsigned char>& imgAnatomyMap,
	 long colSeed, long rowSeed);

	double getMeanError();
	double getStdError();

	double getNewMeanError();
	double getNewStdError();

	IemTImage<unsigned char> getSegMap() {return imgSegMap;}


	private:

	void initialize();
	
	void initializeSupportMap();
	
	void decomposeSegMap();
	
	void decomposeSegMap2();

	void initialFit();
	
	void dilateSegMap(short aggregate, short kernelSize);
	
	long growRegion();
	
	void writeImages();
	
	void newFit();
	
	void writeInfo();
	


}; 


} // Namespace CADX_SEG


#endif
