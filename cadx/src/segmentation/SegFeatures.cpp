//
//  (c) Copyright Eastman Kodak Company
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//





#include "SegFeatures.h"
#include "Boundary.h"
#include "CadxParm.h"  
#include "Iem/IemImageIO.h"    
#include "Iem/IemMorphological.h"
#include "Iem/maximumFxns.h"
#include "Iem/minimumFxns.h"
#include "RegionLabel.h"
#include "ShapeFeature.h"
#include "RegionFeature.h"
#include "GradientFeature.h"
#include "TextureFeature.h"
#include "OverlapFeature.h"
#include "AfumFeature.h"
#include "EdgeDetector.h"
#include "MorphFilter.h"
#include "MultiMorphFilter.h"
#include "BlurFilter.h"
#include "ImageUtil.h"
#include "ScalingFilter.h"
#include "Decomposition.h"
#include "SurfaceFeature.h"
#include "SegmenterA.h"
#include "ScaleSelector.h"
#include "LungMaskMaker.h"
 


using namespace CADX_SEG;


SegFeatures::SegFeatures() {

	initialize();
}


void SegFeatures::initialize() {   
 
              
}   


void SegFeatures::calculate(Roi& roi) {


    LungMaskMaker lungMaskMaker;
	char outputDir[1024], outFileName[1024], tag[1024];
	strcpy(outputDir, userAgent.getOutputDir());

	img = roi.getPreprocessedImage();
//	imgribsup = roi.getPreprocessedImageribsup();

	IemTImage<unsigned char> imagex02 = lungMaskMaker.convertTo8Bit(img);
	userAgent.writeDebugImage(imagex02, "Preprocessed");

//	IemTImage<unsigned char> imagex02ribsup = lungMaskMaker.convertTo8Bit(imgribsup);
//	userAgent.writeDebugImage(imagex02ribsup, "Preprocessedribsup");
//	userAgent.writeDebugImage(img, "Preprocessed");

	imgLungMask = roi.getAnatomicalMap();
	userAgent.writeDebugImage(imgLungMask, "AnatomicalMap");

	imgNoduleRibDif =  roi.getNoduleRibDifImage();
//	imgNoduleRibDifribsup =  roi.getNoduleRibDifImageribsup();
	userAgent.writeDebugImage(imgNoduleRibDif, "NoduleRibDif");
//	userAgent.writeDebugImage(imgNoduleRibDifribsup, "NoduleRibDifribsup");

	long colSeed = roi.getCandidateCol();
	long rowSeed = roi.getCandidateRow();
////////////////////////////////////////////
	SegmenterA segmenter;
	segmenter.setUserAgent(userAgent);
	imgRegionMap = segmenter.segment(imgNoduleRibDif, imgLungMask, colSeed, rowSeed);

	if(segmenter.hasFailed()) {
		useDefaultFeatures(roi);
		roi.setNoduleMap(imgRegionMap);
		userAgent.getLogFile() << "\n\nSegmention failed." << endl;
		return;
	}

	Boundary boundary;
	boundary.mapToBoundary(imgRegionMap);
	userAgent.writeDebugImage(boundary.getBoundaryImage(), "RegionBoundary");
//////////////////////////////////////////////////
	////////////////////////////////////////////  nodule segmentation

/*	IemTImage<unsigned char> imageContour = boundary.getBoundaryImage();
	for(long r = 0; r < 512; r++) {
		for(long c = 0, k = 0; c < 512; c++, k++) { 
		if(imageContour[0][r][c]==255)
			img[0][r][c] = 4095;
		}
	}
	IemTImage<unsigned char> imagex06 = lungMaskMaker.convertTo8Bit(img);
	userAgent.writeDebugImage(imagex06, "Nodulesegmentation");*/

///////////////////////////////////////////////
//////////////////////////////////////////////
	imgRegionMap = boundary.boundaryToMap();
	userAgent.writeDebugImage(imgRegionMap, "NoduleMap");
//////////////////////////////////////////////////
//	SegmenterA segmenterribsup;                                                                               
//	segmenterribsup.setUserAgent(userAgent);
//	imgRegionMapribsup = segmenterribsup.segment(imgNoduleRibDifribsup, imgLungMask, colSeed, rowSeed);          

//	if(segmenterribsup.hasFailed()) {
//		imgRegionMapribsup = imgRegionMap;

//	}

//	Boundary boundaryribsup;
//	boundaryribsup.mapToBoundary(imgRegionMapribsup);
//	userAgent.writeDebugImage(boundaryribsup.getBoundaryImage(), "RegionBoundaryribsup");                     
//	imgRegionMapribsup = boundaryribsup.boundaryToMap();
//	userAgent.writeDebugImage(imgRegionMapribsup, "NoduleMapribsup");                                        
//////////////////////////////////////////////////////
	calculateFeatures(roi);
	roi.setNoduleMap(imgRegionMap);
//	roi.setNoduleMapribsup(imgRegionMapribsup);                                                                 
//	if(userAgent.getDebug()) userAgent.writeDebugImage(roi.getAnnotated(), "Annotated");
}



  
void SegFeatures::calculateFeatures(Roi& roi) {

	 	ShapeFeature shapeFeature("Shape", imgRegionMap);
		shapeFeature.calculate();
		shapeFeature.addToRoi(roi);
		shapeFeature.write(userAgent.getLogFile());

//		ShapeFeature shapeFeatureribsup("Sr", imgRegionMapribsup);                                  
//		shapeFeatureribsup.calculate();
//		shapeFeatureribsup.addToRoi(roi);
//		shapeFeatureribsup.write(userAgent.getLogFile());
//		userAgent.writeDebugImage(shapeFeature.getConvexHull(), "ShapeFeature_ConvexHull");

		RegionFeature regionFeature("D", img, imgRegionMap, imgLungMask);
		regionFeature.setUserAgent(userAgent);
 		regionFeature.calculate();
		regionFeature.addToRoi(roi);
		regionFeature.write(userAgent.getLogFile());

//		RegionFeature regionFeatureribsup("Dr", imgribsup, imgRegionMapribsup, imgLungMask);            
//		regionFeatureribsup.setUserAgent(userAgent);
//		regionFeatureribsup.calculate();
//		regionFeatureribsup.addToRoi(roi);
//		regionFeatureribsup.write(userAgent.getLogFile());
//		userAgent.writeDebugImage(regionFeature.getBkMap(), "RegionFeature_Surround");

		RegionFeature noduleRibDifFeature("NR", imgNoduleRibDif, imgRegionMap, imgLungMask);
		noduleRibDifFeature.setUserAgent(userAgent);
 		noduleRibDifFeature.calculate();
 		noduleRibDifFeature.addToRoi(roi);
		noduleRibDifFeature.write(userAgent.getLogFile());

//		RegionFeature noduleRibDifFeatureribsup("NRr", imgNoduleRibDifribsup, imgRegionMapribsup, imgLungMask);    
//		noduleRibDifFeatureribsup.setUserAgent(userAgent);
//		noduleRibDifFeatureribsup.calculate();
//		noduleRibDifFeatureribsup.addToRoi(roi);
//		noduleRibDifFeatureribsup.write(userAgent.getLogFile());
		
		long maxCol = noduleRibDifFeature.getMaxCol();
		long maxRow = noduleRibDifFeature.getMaxRow();
		userAgent.getLogFile() << "\n\nNodule-Rib max point: col=" << maxCol << ", row=" << maxRow;

 		GradientFeature gradientFeature("G", imgNoduleRibDif, imgRegionMap);
		gradientFeature.setMinGradientMag(0);
		gradientFeature.setMaxGradientMag(SHRT_MAX);
		gradientFeature.setUserAgent(userAgent);
 		gradientFeature.calculate();
 		gradientFeature.addToRoi(roi);
		gradientFeature.write(userAgent.getLogFile());

//		GradientFeature gradientFeatureribsup("Gr", imgNoduleRibDifribsup, imgRegionMapribsup);      
//		gradientFeatureribsup.setMinGradientMag(0);
//		gradientFeatureribsup.setMaxGradientMag(SHRT_MAX);
//		gradientFeatureribsup.setUserAgent(userAgent);
//		gradientFeatureribsup.calculate();
//		gradientFeatureribsup.addToRoi(roi);
//		gradientFeatureribsup.write(userAgent.getLogFile());                                          
//		userAgent.writeDebugImage(gradientFeature.getEdgeImage(), "GradientFeature_Edge");
//		userAgent.writeDebugImage(gradientFeature.getCosineImage(), "GradientFeature_Cosine");

		TextureFeature textureFeature("T", img, imgRegionMap);
 		textureFeature.calculate();
 		textureFeature.addToRoi(roi);
		textureFeature.write(userAgent.getLogFile());

//		TextureFeature textureFeatureribsup("Tr", imgribsup, imgRegionMapribsup);                   
 //   	textureFeatureribsup.calculate();
//		textureFeatureribsup.addToRoi(roi);
//		textureFeatureribsup.write(userAgent.getLogFile());                                             


//		userAgent.writeDebugImage(textureFeature.getImage(), "Texture_Input");
//		userAgent.writeDebugImage(textureFeature.getImageMap(), "Texture_Map");
//		userAgent.writeDebugImage(textureFeature.getEdgeImage(), "Texture_Edge");
		
		AfumFeature afumFeature("N-R", imgNoduleRibDif, imgRegionMap, imgLungMask);
		afumFeature.setUserAgent(userAgent);
 		afumFeature.calculate();
 		afumFeature.addToRoi(roi);
		afumFeature.write(userAgent.getLogFile());

//		AfumFeature afumFeatureribsup("N-Rr", imgNoduleRibDifribsup, imgRegionMapribsup, imgLungMask);          
//		afumFeatureribsup.setUserAgent(userAgent);
//		afumFeatureribsup.calculate();
//		afumFeatureribsup.addToRoi(roi);
//		afumFeatureribsup.write(userAgent.getLogFile());
		
		SurfaceFeature surfaceFeature("Sur", imgNoduleRibDif, imgRegionMap, maxCol, maxRow);
		surfaceFeature.setUserAgent(userAgent);
 		surfaceFeature.calculate();
 		surfaceFeature.addToRoi(roi);
 		surfaceFeature.write(userAgent.getLogFile());

//		SurfaceFeature surfaceFeatureribsup("Surr", imgNoduleRibDifribsup, imgRegionMapribsup, maxCol, maxRow);   
//		surfaceFeatureribsup.setUserAgent(userAgent); 
//		surfaceFeatureribsup.calculate();
//		surfaceFeatureribsup.addToRoi(roi);
//		surfaceFeatureribsup.write(userAgent.getLogFile());
 //		userAgent.writeDebugImage(surfaceFeature.getFitImage(), "SurfaceFit");
 		
 		Boundary boundary;
		boundary.mapToBoundary(imgRegionMap);

		OverlapFeature overlapFeature("Over", img, boundary, imgLungMask); 
		overlapFeature.setUserAgent(userAgent);
 		overlapFeature.calculate();
 		overlapFeature.addToRoi(roi);
 		overlapFeature.write(userAgent.getLogFile());

//		Boundary boundaryribsup;
//		boundaryribsup.mapToBoundary(imgRegionMapribsup);                                                       

//		OverlapFeature overlapFeatureribsup("Overr", imgribsup, boundaryribsup, imgLungMask); 
//		overlapFeatureribsup.setUserAgent(userAgent);
//		overlapFeatureribsup.calculate();
//		overlapFeatureribsup.addToRoi(roi);
//		overlapFeatureribsup.write(userAgent.getLogFile());
}
 

void SegFeatures::useDefaultFeatures(Roi& roi) {

 	ShapeFeature shapeFeature("Shape");
//	ShapeFeature shapeFeatureribsup("Sr");                    

	RegionFeature regionFeature("D");
//	RegionFeature regionFeatureribsup("Dr");

	RegionFeature noduleRibDifFeature("NR");
//	RegionFeature noduleRibDifFeatureribsup("NRr");
	GradientFeature gradientFeature("G");
//	GradientFeature gradientFeatureribsup("Gr");
	TextureFeature textureFeature("T");
//	TextureFeature textureFeatureribsup("Tr");

	AfumFeature afumFeature("N-R");
//	AfumFeature afumFeatureribsup("N-Rr");
	SurfaceFeature surfaceFeature("Sur");
//	SurfaceFeature surfaceFeatureribsup("Surr");
	OverlapFeature overlapFeature("Over");
//	OverlapFeature overlapFeatureribsup("Overr");

	shapeFeature.addToRoi(roi);
//	shapeFeatureribsup.addToRoi(roi);                

	regionFeature.addToRoi(roi);
//	regionFeatureribsup.addToRoi(roi);               

	noduleRibDifFeature.addToRoi(roi);
//	noduleRibDifFeatureribsup.addToRoi(roi);         
	gradientFeature.addToRoi(roi);
//	gradientFeatureribsup.addToRoi(roi);           
	textureFeature.addToRoi(roi);
//	textureFeatureribsup.addToRoi(roi);          

	afumFeature.addToRoi(roi);
//	afumFeatureribsup.addToRoi(roi);              
	surfaceFeature.addToRoi(roi);
//	surfaceFeatureribsup.addToRoi(roi);          
	overlapFeature.addToRoi(roi);
//	overlapFeatureribsup.addToRoi(roi);          
}







	
