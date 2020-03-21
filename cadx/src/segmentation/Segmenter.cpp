//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//





#include "Segmenter.h"     
#include "RegionGrower.h"   
#include "CadxParm.h"  
#include "Iem/IemImageIO.h"   
#include "Iem/IemMorphological.h"   
#include "RegionLabel.h"
#include "MatchedFilter.h"    
#include "ShapeFeature.h"
#include "RegionFeature.h"
#include "EdgeDetector.h"
#include "MorphFilter.h"
#include "BlurFilter.h"
#include "ImageUtil.h"
 


Segmenter::Segmenter() {

	initialize();
}   


void Segmenter::initialize() {   


}

 
IemTImage<unsigned char> Segmenter::segment(Roi& roi) {     
		
	IemTImage<unsigned char> imgGrown = growRegion(roi);  
	
	return imgGrown;	
}


IemTImage<unsigned char> Segmenter::growRegion(Roi& roi) {     
	
	char outputDir[1024], outFileName[1024];
	strcpy(outputDir, CadxParm::getInstance().getOutputDir());	
	
	
	IemTImage<unsigned char> imgBestMap;
	ShapeFeature bestShape;   
	RegionFeature bestRegion;
	RegionFeature bestRegionMatch; 
	RegionFeature bestRegionDirMatch;
	
	double maxScore = 0, score;    
	
	IemTImage<short> img = roi.getCroppedImage();
	short colSeed = roi.getCandidateCol(); 
	short rowSeed = roi.getCandidateRow();  
	
//	BlurFilter blurFilter(5,5);
//	IemTImage<short> imgBlurred = blurFilter.filter(img);
	
//	EdgeDetector edgeDetector(imgBlurred);  
//	edgeDetector.setUpperHistPen(1.0);   
//	edgeDetector.calcSobelGradient();	 
	
/*		
	if(CadxParm::getInstance().getDebug()) {				
		sprintf(outFileName, "%s/%s_edgeMag.tif", outputDir, roi.getName());
		iemWrite(edgeDetector.getEdgeMag(), outFileName);   
		sprintf(outFileName, "%s/%s_edgeDir.tif", outputDir, roi.getName());
		iemWrite(edgeDetector.getEdgeDir(), outFileName);     
	}  			    
*/

	for(long k = 0; k < CadxParm::getInstance().getNTemplates(); k++) {   
		
		char templateName[1024];
		
		strcpy(templateName, CadxParm::getInstance().getNoduleTemplateFileName(k));  
		
		CadxParm::getInstance().getLogFile() << "\n\nUsing template " 
		 << CadxParm::getInstance().getNoduleTemplateFileName(k);
		
		Util::stripDirectory(templateName);     
		Util::stripExtension(templateName);     
		  
		IemTImage<short> imgTemplate = iemRead(CadxParm::getInstance().getNoduleTemplateFileName(k)); 
		
		short decimation = Util::max(log10(imgTemplate.cols()) / log10(2) - 5, 0);
		
				
//		EdgeDetector edgeDetector2(imgTemplate);  
//		edgeDetector2.setUpperHistPen(1.0);   
//		edgeDetector2.calcSobelGradient();	     
//		IemTImage<short> imgDirTemplate = edgeDetector2.getEdgeDir();

		MatchedFilter matchedFilter(imgTemplate, decimation);
		IemTImage<short> imgMatch = matchedFilter.filter(img);
		
		if(CadxParm::getInstance().getDebug()) {				
			sprintf(outFileName, "%s/%s_matched_%s.tif", outputDir, roi.getName(), templateName);
			iemWrite(imgMatch, outFileName);  
		}  

//		MatchedFilter matchFilterDir(imgDirTemplate, decimation);
//		IemTImage<short> imgDirMatch = matchFilterDir.filter(edgeDetector.getEdgeDir());  
		
//		if(CadxParm::getInstance().getDebug()) {	
//			sprintf(outFileName, "%s/%s_match_dir_%s.tif", outputDir, roi.getName(), templateName);
//			iemWrite(imgDirMatch, outFileName);  
//		} 	
     
		RegionGrower regionGrower(imgMatch, colSeed, rowSeed); 
		regionGrower.start();
		   
		regionGrower.write(CadxParm::getInstance().getLogFile());  
		
		if(CadxParm::getInstance().getDebug()) {														                                    
			sprintf(outFileName, "%s/%s_grown_%s.tif", outputDir, roi.getName(), templateName);
			iemWrite(regionGrower.getRegionView(), outFileName); 
		}  
		
		IemTImage<unsigned char> imgDecomposedMap = decomposeRegionMap(regionGrower.getMask(), roi);  
		
		if(CadxParm::getInstance().getDebug()) {														                                    
			sprintf(outFileName, "%s/%s_map_%s.tif", outputDir, roi.getName(), templateName);
			iemWrite(imgDecomposedMap, outFileName); 
		}      
		
 		ShapeFeature shape("Shape", imgDecomposedMap);
 		shape.calculate();	
		shape.write(CadxParm::getInstance().getLogFile());
		
		if(CadxParm::getInstance().getDebug()) {														                                    
			sprintf(outFileName, "%s/%s_convexhull_map_%s.tif", outputDir, roi.getName(), templateName);
			iemWrite(shape.getConvexHull(), outFileName); 
		}      
		
		RegionFeature region("Image", img, shape.getConvexHull());
 		region.calculate();
		region.write(CadxParm::getInstance().getLogFile());
		
		RegionFeature regionMatch("Matched", imgMatch, shape.getConvexHull());
 		regionMatch.calculate();
		regionMatch.write(CadxParm::getInstance().getLogFile());
		
//		RegionFeature regionDirMatch("DirMatched", imgDirMatch, shape.getConvexHull());
// 		regionDirMatch.calculate();	 
//		regionDirMatch.write(CadxParm::getInstance().getLogFile());
 		
 		score = shape.getScore() * regionMatch.getScore(); // * regionDirMatch.getScore();
		CadxParm::getInstance().getLogFile() << "\n\nscore: " << score << flush;
		
		if(score > maxScore) {   
						
			maxScore = score;
		//	imgBestMap = imgDecomposedMap;
			imgBestMap = shape.getConvexHull();
			bestShape = shape;
			bestRegion = region;	
			bestRegionMatch = regionMatch;	
//			bestRegionDirMatch = regionDirMatch;		
		}  
		
	}
		
		
	bestShape.addToRoi(roi);   
	bestRegion.addToRoi(roi);  
	bestRegionMatch.addToRoi(roi);  
//	bestRegionDirMatch.addToRoi(roi);  
	
	return imgBestMap;   
}


IemTImage<unsigned char> Segmenter::decomposeRegionMap(IemTImage<unsigned char>& imgMap, Roi& roi) { 
	
	char outputDir[1024], outFileName[1024];
	strcpy(outputDir, CadxParm::getInstance().getOutputDir());	    
			
	IemTImage<unsigned char> imgEroded = iemErode(imgMap, 5, 5);  	
	
	if(CadxParm::getInstance().getDebug() > 1) {	   	
		sprintf(outFileName, "%s/%s_segmenter_eroded.tif", outputDir, roi.getName());	
		iemWrite(imgEroded, outFileName);
	}  	
	  
	RegionLabel regionLabel;
	regionLabel.Label_Connected_Regions(imgEroded, 0, 255, 10000);
	 
	IemTImage<unsigned char> imgLabeled = regionLabel.getLabeledImage(); 
	
	if(CadxParm::getInstance().getDebug() > 1) {	
		sprintf(outFileName, "%s/%s_segmenter_labeled.tif", outputDir, roi.getName());	   	
		iemWrite(imgLabeled, outFileName);   
	}   	 
	
	short colSeed = roi.getCandidateCol(); 
	short rowSeed = roi.getCandidateRow();
	
	IemTImage<unsigned char> imgPruned = pruneRegions(imgLabeled, colSeed, rowSeed, imgEroded);   
	
	if(CadxParm::getInstance().getDebug() > 1) {	
		sprintf(outFileName, "%s/%s_segmenter_pruned.tif", outputDir, roi.getName());	   	
		iemWrite(imgPruned, outFileName);   
	}      
	  	
	IemTImage<unsigned char> imgDecomposed = iemDilate(imgPruned, 5, 5); 

	if(CadxParm::getInstance().getDebug() > 1) {		
		sprintf(outFileName, "%s/%s_segmenter_dilated.tif", outputDir, roi.getName());	 
		iemWrite(imgMap, outFileName);  
	} 
		 
	return imgDecomposed;
}


IemTImage<unsigned char> Segmenter::pruneRegions(IemTImage<unsigned char>& imgLabeled, long col, long row, IemTImage<unsigned char>& imgMap) {  
	
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



	