//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//





#include "AltSegmenter.h"
#include "RegionGrower.h"   
#include "CadxParm.h"  
#include "Iem/IemImageIO.h"    
#include "Iem/IemMorphological.h"
#include "Iem/maximumFxns.h"
#include "RegionLabel.h"
#include "MatchedFilter.h"    
#include "ShapeFeature.h"
#include "RegionFeature.h"
#include "GradientFeature.h"
#include "EdgeDetector.h"
#include "MorphFilter.h"
#include "MultiMorphFilter.h"
#include "BlurFilter.h"
#include "ImageUtil.h"
#include "ScalingFilter.h"
#include "PolyFit.h"

 


AltSegmenter::AltSegmenter() {

	initialize();
}   

void AltSegmenter::initialize() {

	strcpy(outputDir, "");
	strcpy(name, "");
}

IemTImage<unsigned char> AltSegmenter::segment(Roi& roi) {

	char outFileName[1024];

	strcpy(outputDir, CadxParm::getInstance().getOutputDir());
	strcpy(name, roi.getName());


	IemTImage<short> img = roi.getCroppedImage();

	IemTImage<short> imgInverted = ImageUtil::invert(img, 0, 4095);
	
	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt.tif", outputDir, roi.getName());
		iemWrite(imgInverted, outFileName);
	}
	
	IemTImage<short> imgLung = excludeNonLung(imgInverted, roi.getAnatomicalMap());
	
	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt_lung.tif", outputDir, roi.getName());
		iemWrite(imgLung, outFileName);
	}

	IemTImage<short> imgTemplate = CadxParm::getInstance().getNoduleTemplate(1);
	long nNoduleAggregations = CadxParm::getInstance().getNNoduleAggregations(1);

	MorphFilter morphFilter(imgTemplate, 4, 1.0);
	IemTImage<short> imgNodule = morphFilter.filter(imgInverted, MorphFilter::OPEN);
		
	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt_nodule.tif", outputDir, roi.getName());
		iemWrite(imgNodule, outFileName);
	}
	
	PolyFit polyFit;
	IemTImage<short> imgPolyFit = polyFit.calcFit(imgInverted);

 	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt_polyfit.tif", outputDir, roi.getName());
		iemWrite(imgPolyFit, outFileName);
	}   
	
	IemTImage<short> imgPadded = morphFilter.getPaddedImage();
	
	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt_padded.tif", outputDir, roi.getName());
		iemWrite(imgPadded, outFileName);
	}


   IemTImage<short> imgRibs = ribFilter(imgInverted);

	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt_rib.tif", outputDir, roi.getName());
		iemWrite(imgRibs, outFileName);
	}

	IemTImage<short> imgNoduleFitDif = imgNodule - imgPolyFit;
 //	IemTImage<short> imgNoduleRibDif = imgNodule - imgInverted;
 	
 	imgNoduleFitDif = excludeNonLung(imgNoduleFitDif, roi.getAnatomicalMap());
 	
 	if(CadxParm::getInstance().getDebug()) {
		sprintf(outFileName, "%s/%s_alt_nodule-fit.tif", outputDir, roi.getName());
		iemWrite(imgNoduleFitDif, outFileName);
	}


	IemTImage<unsigned char> imgRegionMap(1, img.rows(), img.cols());
	imgRegionMap = 0;

	return imgRegionMap;
}


IemTImage<short> AltSegmenter::ribFilter(IemTImage<short>& img) {

	short nTemplates = CadxParm::getInstance().getNRibTemplates();
	
//	IemTImage<short>* pRibTemplates = new IemTImage<short>[nTemplates];
	IemTImage<short>* pRibTemplates = CadxParm::getInstance().getRibTemplatesPtr();
	
//	for(short j = 0; j < nTemplates; j++) pRibTemplates[j] = CadxParm::getInstance().getRibTemplate(j);
	
	long nRibAggregations = CadxParm::getInstance().getNRibAggregations();

	MultiMorphFilter multiMorphFilter(pRibTemplates, nTemplates, nRibAggregations, 1.0);

//	multiMorphFilter.setDebug(1);
	multiMorphFilter.setOutputDir(outputDir);
	multiMorphFilter.setName(name);
	
//	return multiMorphFilter.filterDeltaOpen(img);
	return multiMorphFilter.filter(img, MorphFilter::OPEN);
}


IemTImage<short> AltSegmenter::excludeNonLung(IemTImage<short>& img, IemTImage<unsigned char>& mask) {

	IemTImage<short> imgLung = img.copy();

	for(long r = 0; r < img.rows(); r++) {
		for(long c = 0; c < img.cols(); c++) {
			
			if(mask[0][r][c] == 0) imgLung[0][r][c] = 0;
	}}

	return imgLung;
}


IemTImage<unsigned char> AltSegmenter::pruneRegions(IemTImage<unsigned char>& imgLabeled, long col, long row, IemTImage<unsigned char>& imgMap) {

	long v = 0;
	
	double d2min = imgLabeled.cols() * imgLabeled.cols() + imgLabeled.rows() * imgLabeled.rows();  
	
	IemTImage<unsigned char> imgPruned(1, imgLabeled.rows(), imgLabeled.cols());

	for(long c = 0; c < imgLabeled.cols(); c++) {
		for(long r = 0; r < imgLabeled.rows(); r++) {  
			
			if(imgMap[0][r][c] == 0) continue; 

			double d2 = (c - col) * (c - col) + (r - row) * (r - row); 
			
			if(d2 < d2min) {d2min = d2; v = imgLabeled[0][r][c];}
	}} 
	
	for(c = 0; c < imgLabeled.cols(); c++) {
		for(long r = 0; r < imgLabeled.rows(); r++) {
			if(imgLabeled[0][r][c] == v) imgPruned[0][r][c] = 255;
			else imgPruned[0][r][c] = 0;
	}} 
	
	return imgPruned; 
}
