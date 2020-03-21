//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//





#include "MorphSegmenter.h"
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
 


using namespace CADX_SEG;


MorphSegmenter::MorphSegmenter() {

	initialize();
}


void MorphSegmenter::initialize() {   
 
              
}   


void MorphSegmenter::segment(Roi& roi) {

	char outputDir[1024], outFileName[1024], tag[1024];
	long bestScale = -1;
	strcpy(outputDir, userAgent.getOutputDir());


	IemTImage<unsigned char> bestMap;


	double maxScore = -DBL_MAX, score;

	img = roi.getPreprocessedImage();

	// The ROI map when segmentation fails.
	IemTImage<unsigned char> imgBlankMap(1, img.rows(), img.cols());
	imgBlankMap = 0;

	imgLungMask = ImageUtil::erodeMap(roi.getAnatomicalMap(), 9);
	userAgent.writeDebugImage(imgLungMask, "LungMask");

	IemTImage<unsigned char> imgLungMaskDilated = ImageUtil::dilateMap(roi.getAnatomicalMap(), 9);
	userAgent.writeDebugImage(imgLungMaskDilated, "LungMaskDilated");

//	IemTImage<short> imgLungOnly = excludeNonLung(img, imgLungMaskDilated);
	IemTImage<short> imgLungOnly = excludeNonLung(img, roi.getAnatomicalMap());
	userAgent.writeDebugImage(imgLungOnly, "LungOnly");

	IemTImage<short> imgRibs = ribFilter(imgLungOnly);
	userAgent.writeDebugImage(imgRibs, "Ribs");


	for(long k = 0; k < CadxParm::getInstance().getNTemplates(); k++) {   
		
		char templateName[1024];

		sprintf(tag, "%s_res%d", roi.getName(), k);
		userAgent.setTag(tag);

		strcpy(templateName, CadxParm::getInstance().getNoduleTemplateFileName(k));  
		
		userAgent.getLogFile() << "\n\nScale " << k << ". Using template " << templateName << endl;
		
		Util::stripDirectory(templateName);     
		Util::stripExtension(templateName);     
		  
		IemTImage<short> imgTemplate = CadxParm::getInstance().getNoduleTemplate(k); 
		long nNoduleAggregations = CadxParm::getInstance().getNNoduleAggregations(k);
			 
		MorphFilter morphFilter(imgTemplate, nNoduleAggregations, 512.0 / 1024.0);
		IemTImage<short> imgNodule = morphFilter.filter(img, MorphFilter::OPEN);
		userAgent.writeDebugImage(imgNodule, "Nodule");

		IemTImage<short> imgNoduleRibDifX = imgNodule - imgRibs;
		imgNoduleRibDifX = excludeNonLung(imgNoduleRibDifX, imgLungMask);
 		userAgent.writeDebugImage(imgNoduleRibDifX, "Nodule-Rib");

		long colSeed = roi.getCandidateCol();
		long rowSeed = roi.getCandidateRow(); 

		SegmenterA segmenter;
		segmenter.setUserAgent(userAgent);
		IemTImage<unsigned char> imgRegionMapX = segmenter.segment(imgNoduleRibDifX, imgLungMask, colSeed, rowSeed);

		if(segmenter.hasFailed()) {
		    	userAgent.getLogFile() << "\nSegmenter failed!";
		    	continue;
		}

		Boundary boundary;
		boundary.mapToBoundary(imgRegionMapX);
//		userAgent.writeDebugImage(boundary.getBoundaryImage(), "RegionBoundary");
		imgRegionMapX = boundary.boundaryToMap();
		userAgent.writeDebugImage(imgRegionMapX, "RegionMap");
		

		// Calculate features based on region.
          ScaleSelector scaleSelector;
		score = scaleSelector.getScore(imgNoduleRibDifX, imgRegionMapX, imgLungMask);
		userAgent.getLogFile() << "\n\nScale " << k << " score: " << score << endl;
		
		if(score > maxScore) {
		     bestScale = k;
			maxScore = score;
			imgNoduleRibDif = imgNoduleRibDifX;
			imgRegionMap = imgRegionMapX;
		}
		
	}  // Segmentation loop at particular scale

	// Region segmentation and feature extraction failed at all scales.
	if(maxScore == -DBL_MAX) {
		useDefaultFeatures(roi);
		roi.setNoduleMap(imgBlankMap);
		userAgent.getLogFile() << "\n\nSegmention failed at all scales." << endl;
		return;
	}
	
	userAgent.getLogFile() << "\n\nUsing Scale " << bestScale << endl;

	sprintf(tag, "%s_res%d", roi.getName(), bestScale);
	userAgent.setTag(tag);
	calculateFeatures(roi);
	
	roi.setNoduleMap(imgRegionMap);
//	if(userAgent.getDebug()) userAgent.writeDebugImage(roi.getAnnotated(), "Annotated");
}



IemTImage<short> MorphSegmenter::ribFilter(IemTImage<short>& img) {
	short nTemplates = CadxParm::getInstance().getNRibTemplates();
	IemTImage<short>* pRibTemplates = CadxParm::getInstance().getRibTemplatesPtr();
	long nRibAggregations = CadxParm::getInstance().getNRibAggregations();
	MultiMorphFilter multiMorphFilter(pRibTemplates, nTemplates, nRibAggregations, 512.0 / 1024.0);

	return multiMorphFilter.filter(img, 0, MorphFilter::OPEN);
}


IemTImage<short> MorphSegmenter::excludeNonLung(IemTImage<short>& img, IemTImage<unsigned char>& mask) {
 
	IemTImage<short> imgLung = img.copy();
	
	short v = iemMinimum(img);

	for(long r = 0; r < img.rows(); r++) {
		for(long c = 0; c < img.cols(); c++) {
			
			if(mask[0][r][c] == 0) imgLung[0][r][c] = v;
	}}
  
	return imgLung;
} 

  
void MorphSegmenter::calculateFeatures(Roi& roi) {

	 	ShapeFeature shapeFeature("Shape", imgRegionMap);
		shapeFeature.calculate();
		shapeFeature.addToRoi(roi);   
		shapeFeature.write(userAgent.getLogFile());
		userAgent.writeDebugImage(shapeFeature.getConvexHull(), "ShapeFeature_ConvexHull");
/*
		RegionFeature regionFeature("Density", img, imgRegionMap, imgLungMask);
		regionFeature.setUserAgent(userAgent);
 		regionFeature.calculate();
		regionFeature.addToRoi(roi);
		regionFeature.write(userAgent.getLogFile());
		userAgent.writeDebugImage(regionFeature.getBkMap(), "RegionFeature_Surround");
*/
		RegionFeature noduleRibDifFeature("Nodule-Rib", imgNoduleRibDif, imgRegionMap, imgLungMask);
		noduleRibDifFeature.setUserAgent(userAgent);
 		noduleRibDifFeature.calculate();
 		noduleRibDifFeature.addToRoi(roi);
		noduleRibDifFeature.write(userAgent.getLogFile());
		
		long maxCol = noduleRibDifFeature.getMaxCol();
		long maxRow = noduleRibDifFeature.getMaxRow();
		userAgent.getLogFile() << "\n\nNodule-Rib max point: col=" << maxCol << ", row=" << maxRow;

 		GradientFeature gradientFeature("Gradient", imgNoduleRibDif, imgRegionMap);
		gradientFeature.setUserAgent(userAgent);
 		gradientFeature.calculate();
 		gradientFeature.addToRoi(roi);
		gradientFeature.write(userAgent.getLogFile());
//		userAgent.writeDebugImage(gradientFeature.getEdgeImage(), "GradientFeature_Edge");
//		userAgent.writeDebugImage(gradientFeature.getCosineImage(), "GradientFeature_Cosine");

		TextureFeature textureFeature("Texture", img, imgRegionMap);
 		textureFeature.calculate();
 		textureFeature.addToRoi(roi);
		textureFeature.write(userAgent.getLogFile());
//		userAgent.writeDebugImage(textureFeature.getImage(), "Texture_Input");
//		userAgent.writeDebugImage(textureFeature.getImageMap(), "Texture_Map");
//		userAgent.writeDebugImage(textureFeature.getEdgeImage(), "Texture_Edge");
		
		AfumFeature afumFeature("Nodule-Rib", imgNoduleRibDif, imgRegionMap, imgLungMask);
		afumFeature.setUserAgent(userAgent);
 		afumFeature.calculate();
 		afumFeature.addToRoi(roi);
		afumFeature.write(userAgent.getLogFile());
		
		SurfaceFeature surfaceFeature("Surface", imgNoduleRibDif, imgRegionMap, maxCol, maxRow);
		surfaceFeature.setUserAgent(userAgent);
 		surfaceFeature.calculate();
 		surfaceFeature.addToRoi(roi);
 		surfaceFeature.write(userAgent.getLogFile());
// 		userAgent.writeDebugImage(surfaceFeature.getFitImage(), "SurfaceFit");
 		
 		Boundary boundary;
		boundary.mapToBoundary(imgRegionMap);

 		OverlapFeature overlapFeature("Overlap", img, boundary, imgLungMask); 
		overlapFeature.setUserAgent(userAgent);
 		overlapFeature.calculate();
 		overlapFeature.addToRoi(roi);
 		overlapFeature.write(userAgent.getLogFile());
 }
 

void MorphSegmenter::useDefaultFeatures(Roi& roi) {

 	ShapeFeature shapeFeature("Shape");
//	RegionFeature regionFeature("Density");
	RegionFeature noduleRibDifFeature("Nodule-Rib");
	GradientFeature gradientFeature("Gradient");
	TextureFeature textureFeature("Texture");
	AfumFeature afumFeature("Nodule-Rib");
	SurfaceFeature surfaceFeature("Surface");
	OverlapFeature overlapFeature("Overlap");

	shapeFeature.addToRoi(roi);
//	regionFeature.addToRoi(roi);
	noduleRibDifFeature.addToRoi(roi);
	gradientFeature.addToRoi(roi);
	textureFeature.addToRoi(roi);
	afumFeature.addToRoi(roi);
	surfaceFeature.addToRoi(roi);
	overlapFeature.addToRoi(roi);
}







	
