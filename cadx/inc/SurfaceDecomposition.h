//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef SURFACEDECOMPOSITION_H
#define SURFACEDECOMPOSITION_H


#include "Iem/Iem.h"    
#include "Util.h" 




namespace CADX_SEG {

  
class SurfaceDecomposition {

	protected:

	// The image to be segmented.
	IemTImage<short> img;
	
	IemTImage<short> imgSmooth;
	
	IemTImage<short> imgGx;
	IemTImage<short> imgGy;
	IemTImage<short> imgGxx;
	IemTImage<short> imgGyy;
	IemTImage<short> imgGxy;

	// A map of anatomy.
	IemTImage<unsigned char> imgAnatomyMap;

	// The segmented region.
	IemTImage<unsigned char> imgSegMap;

	// The region of support for the surface fit.
	IemTImage<unsigned char> imgSupportMap;
	
	// Location of seed pixel.
	long colSeed, rowSeed;

	long debug;
	char ID[1024];
	char outputDir[1024];
	ofstream *logFile;

	public:
	
	SurfaceDecomposition();
	
	IemTImage<unsigned char> decompose(IemTImage<short>& _img, IemTImage<unsigned char>& _imgAnatomyMap, 
	 IemTImage<unsigned char>& _imgSupportMap, long _colSeed, long _rowSeed);

	void setOutputDir(char* _outputDir) {strcpy(outputDir, _outputDir);}
	void setID(char* _ID) {strcpy(ID, _ID);}
	void setDebug(short _debug) {debug = _debug;}
	void setLogFile(ofstream* _logFile) {logFile = _logFile;}
	
	protected:
	
	void initialize();
	
	void smoothImage();
	
	void calcDerivatives();


}; 


} // Namespace CADX_SEG


#endif
