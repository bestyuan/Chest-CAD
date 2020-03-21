//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "MatchedFilter.h"   
#include "util.h"  
#include "ImageUtil.h" 
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemAggregate.h"
#include "Iem/IemImageIO.h" 



MatchedFilter::MatchedFilter(IemTImage<short>& _imgTemplate, short _aggregation) {

	initialize();
	
	aggregation = _aggregation;
	
//	imgTemplate = iemAggregateDown(_imgTemplate, decimation, 0);
//	"h:/schildkr/template.tif" << imgTemplate;

	imgTemplate = _imgTemplate;
	
	calcTemplateStats();
}


void MatchedFilter::initialize() {

	templateN = 0.0;
	templateMean = 0.0; 
	templateSigma = 0.0;
	aggregation = 0;
}


void MatchedFilter::calcTemplateStats() {

	double sum1 = 0, sum2 = 0;  
	long n = 0;
	
	for(long c = 0; c < imgTemplate.cols(); c++) {
		for(long r = 0; r < imgTemplate.rows(); r++) {

			short v = imgTemplate[0][r][c];
	                
			n++;	    
			sum1 += v;
			sum2 += v * v;			
	}}  
	        
	templateN = (double)n;
	templateMean = (double)sum1 / templateN;   
	templateSigma = sqrt((double)sum2 / templateN - Util::square(templateMean)); 
}
     


IemTImage<short> MatchedFilter::filter(IemTImage<short>& img) {   

	imgAggregated = iemAggregateDown(img, aggregation, 0);
		
	long tpad = imgTemplate.cols() / 2;
	long lpad = imgTemplate.rows() / 2;
	long rpad = imgTemplate.rows() / 2;
	long bpad = imgTemplate.cols() / 2;
		
	imgPadded = ImageUtil::pad(imgAggregated, tpad, lpad, rpad, bpad);
	
	IemTImage<short> imgFiltered(1, imgAggregated.rows(), imgAggregated.cols());
	
	// Calculate correlation.
	for(long c = 0; c < imgAggregated.cols(); c++) {
		for(long r = 0; r < imgAggregated.rows(); r++) {
			
			imgFiltered[0][r][c] = (short)filter(imgPadded, c, r);
	}}  
	
	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}  



double MatchedFilter::filter(IemTImage<short>& img, long c, long r) {
	
	double sum1 = 0, sum2 = 0, sum3 = 0; 
			
	for(long cc = 0; cc < imgTemplate.cols(); cc++) {  
		
		long lineSum1 = 0;
		double lineSum2 = 0, lineSum3 = 0; 		
		
		for(long rr = 0; rr < imgTemplate.rows(); rr++) {
				
			short v = img[0][r + rr][c + cc]; 
			short vTemplate = imgTemplate[0][rr][cc];
	
			lineSum1 += v;
			lineSum2 += v * v;	  
			lineSum3 += v * vTemplate; 
		}  
		
		sum1 += lineSum1;
		sum2 += lineSum2;
		sum3 += lineSum3;		
	}
			
	double mean = sum1 / templateN;   
	double sigma = sqrt(sum2 / templateN - Util::square(mean));    
	double product = sum3 / templateN;
			
	double corr = (product - mean * templateMean) / (sigma * templateSigma);
	
//	return 4095.0 * (corr + 1.0) / 2.0;
	
	return 1000.0 * corr;
}
