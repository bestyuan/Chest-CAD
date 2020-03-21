//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "SurfaceDecomposition.h"
#include "PolyFit.h" 
#include "RegionLabel.h"
#include "ConvexHull.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemMorphological.h"
#include "Iem/IemKernelPlane.h"


using namespace CADX_SEG;

 
CADX_SEG::SurfaceDecomposition::SurfaceDecomposition() {
	initialize();
}


void CADX_SEG::SurfaceDecomposition::initialize() {
	colSeed = 0;
	rowSeed = 0;
	debug = 1;
	strcpy(outputDir, "./");
	strcpy(ID, "SurfaceDecomposition");
}

  
IemTImage<unsigned char> CADX_SEG::SurfaceDecomposition::decompose(IemTImage<short>& _img, 
 IemTImage<unsigned char>& _imgAnatomyMap, IemTImage<unsigned char>& _imgSupportMap,
 long _colSeed, long _rowSeed) {

	char outFileName[1024];

	img = _img;
	imgAnatomyMap = _imgAnatomyMap;
	imgSupportMap = _imgSupportMap;

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceDecomposition.tif", outputDir, ID);
		iemWrite(img, outFileName);
		sprintf(outFileName, "%s/%s_SurfaceDecomposition_AnatomyMap.tif", outputDir, ID);
		iemWrite(imgAnatomyMap, outFileName);
		sprintf(outFileName, "%s/%s_SurfaceDecomposition_SupportMap.tif", outputDir, ID);
		iemWrite(imgSupportMap, outFileName);
	} 
	
	colSeed = _colSeed;
	rowSeed = _rowSeed;
 
	imgSegMap = IemTImage<unsigned char>(1, img.rows(), img.cols());
	imgSegMap = 0;

	smoothImage();
	
	calcDerivatives();

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceDecomposition_SegMap.tif", outputDir, ID);
		iemWrite(imgSegMap, outFileName);
	}

	return imgSegMap;
}


void CADX_SEG::SurfaceDecomposition::smoothImage() {

	char outFileName[1024];  
	
	double skernel[7] = {1.0/64.0, 6.0/64.0, 15.0/64.0, 20.0/64.0, 15.0/64.0, 6.0/64.0, 1.0/64.0};

	IemKernelPlane s(7, 1, skernel);
	IemKernelPlane st(1, 7, skernel);

	imgSmooth = st.apply(img);
	imgSmooth = s.apply(imgSmooth);
		 
	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceDecomposition_smooth.tif", outputDir, ID);
		iemWrite(imgSmooth, outFileName);
	}
}
 
 
void CADX_SEG::SurfaceDecomposition::calcDerivatives() {

	char outFileName[1024];
	
	double c0 = 7.0, c1 = 1.0, c2 = 1.0;

	double d0kernel[7] = {1.0/c0, 1.0/c0, 1.0/c0, 1.0/c0, 1.0/c0, 1.0/c0, 1.0/c0};
	double d1kernel[7] = {-3.0/c1, -2.0/c1, -1.0/c1, 0.0/c1, 1.0/c1, 2.0/c1, 3.0/c1};
	double d2kernel[7] = {5.0/c2, 0.0/c2, -3.0/c2, -4.0/c2, -3.0/c2, 0.0/c2, 5.0/c2};

	IemKernelPlane d0(7, 1, d0kernel);
	IemKernelPlane d0t(1, 7, d0kernel);
	
	IemKernelPlane d1(7, 1, d1kernel);
	IemKernelPlane d1t(1, 7, d1kernel); 
	
	IemKernelPlane d2(7, 1, d2kernel);
	IemKernelPlane d2t(1, 7, d2kernel);

    /*
	imgGx = d1t.apply(imgSmooth);
	imgGx = d0.apply(imgGx);
	
	imgGy = d0t.apply(imgSmooth);
	imgGy = d1.apply(imgGy);
    */
    
    	imgGx = d1t.apply(imgSmooth);
	imgGy = d1.apply(imgSmooth);

	if(debug) {
		sprintf(outFileName, "%s/%s_SurfaceDecomposition_Gx.tif", outputDir, ID);
		iemWrite(imgGx, outFileName);
		sprintf(outFileName, "%s/%s_SurfaceDecomposition_Gy.tif", outputDir, ID);
		iemWrite(imgGy, outFileName);
	}

}


