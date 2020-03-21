//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "MorphFilter.h"
#include "ImageUtil.h"
#include "CadxParm.h" 
#include "Histogram.h" 
#include "Iem/IemImageIO.h"
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  
#include "Iem/IemAggregate.h"
#include "Iem/IemTImageIter.h"


using namespace CADX_SEG;


const short MorphFilter::ERODE = 0;
const short MorphFilter::DILATE = 1;
const short MorphFilter::OPEN = 2;
const short MorphFilter::CLOSE = 3;






MorphFilter::MorphFilter(IemTImage<short>& _imgTemplate, short _aggregation, double templateHeightScale) {

	initialize();
	
	aggregation = _aggregation;
	imgTemplate = _imgTemplate.copy();
	
	if(templateHeightScale != 1.0)  {
		imgTemplate = imgTemplate * templateHeightScale;
	}
}


void MorphFilter::initialize() {
	aggregation = 0;
}
 

IemTImage<short> MorphFilter::filter(IemTImage<short>& img, short type) {

	IemTImage<short> imgFiltered;

	userAgent.writeDebugImage(img, "MorphFilter_Input");
	userAgent.writeDebugImage(imgTemplate, "MorphFilter_Template");

	if(type == DILATE) imgFiltered = basicFilter(img, DILATE);
	else if(type == ERODE) imgFiltered = basicFilter(img, ERODE);
	else if(type == OPEN) {
		IemTImage<short> imgTmp = basicFilter(img, ERODE);  
		userAgent.writeDebugImage(imgTmp, "MorphFilter_Eroded");
		imgFiltered = basicFilter(imgTmp, DILATE);
		userAgent.writeDebugImage(imgFiltered, "MorphFilter_Opened");
	}
	else if(type == CLOSE) {
		IemTImage<short> imgTmp = basicFilter(img, DILATE);
		userAgent.writeDebugImage(imgTmp, "MorphFilter_Dilated");
		imgFiltered = basicFilter(imgTmp, ERODE);
		userAgent.writeDebugImage(imgFiltered, "MorphFilter_Closed");
	}

	return imgFiltered;
}




IemTImage<short> MorphFilter::basicFilter(IemTImage<short>& img, short type) {
	if(type == DILATE) return dilation(img);
	else if(type == ERODE) return erosion(img);
	else return img.copy();
}


short MorphFilter::dilatePixel(IemTImage<short>& img, long c, long r) {
	
	short max = SHRT_MIN;

	for(long cc = 0; cc < imgTemplate.cols(); cc++) {
		for(long rr = 0; rr < imgTemplate.rows(); rr++) {
				
			short v = img[0][r + rr][c + cc] + imgTemplate[0][rr][cc];

			if(v > max) max = v;
	}}
				
	return max;
}

short MorphFilter::erodePixel(IemTImage<short>& img, long c, long r) {
	
	short min = SHRT_MAX; 

	for(long cc = 0; cc < imgTemplate.cols(); cc++) {  
		for(long rr = 0; rr < imgTemplate.rows(); rr++) {
				
			short v = img[0][r + rr][c + cc] - imgTemplate[0][rr][cc];
	
			if(v < min) min = v;
	}}
				
	return min;
}


IemTImage<short> MorphFilter::dilation(IemTImage<short>& img) {

	imgAggregated = iemAggregateDown(img, aggregation, 0);

	long tpad = imgTemplate.rows() / 2;
	long lpad = imgTemplate.cols() / 2;
	long rpad = imgTemplate.cols() / 2;
	long bpad = imgTemplate.rows() / 2;

	imgPadded = ImageUtil::pad(imgAggregated, tpad, lpad, rpad, bpad);
//	userAgent.writeDebugImage(imgPadded, "MorphFilter_DilationPadded");

	IemTImage<short> imgFiltered(1, imgAggregated.rows(), imgAggregated.cols());
	
	long aggregatedCols = imgAggregated.cols();
	long aggregatedRows = imgAggregated.rows();
	
	long templateCols = imgTemplate.cols();
	long templateRows = imgTemplate.rows();
	
	IemTImageIter<short> paddedIter(imgPadded);
	IemTImageIter<short> templateIter(imgTemplate);

	for(long c = 0; c < aggregatedCols; c++) {
		for(long r = 0; r < aggregatedRows; r++) {

			short max = SHRT_MIN; 

			for(long rr = 0; rr < templateRows; rr++) {

				paddedIter.setPos(r + rr, c);
				templateIter.setPos(rr, 0);

				for(long cc = 0; cc < templateCols; cc++) {
					short v = paddedIter[0] + templateIter[0];
					if(v > max) max = v;
          			paddedIter.colInc(); templateIter.colInc();
				}
			
			}

			imgFiltered[0][r][c] = max;
	}}

	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}

 

IemTImage<short> MorphFilter::erosion(IemTImage<short>& img) {

	imgAggregated = iemAggregateDown(img, aggregation, 0);

	long tpad = imgTemplate.rows() / 2;
	long lpad = imgTemplate.cols() / 2;
	long rpad = imgTemplate.cols() / 2;
	long bpad = imgTemplate.rows() / 2;
		
	imgPadded = ImageUtil::pad(imgAggregated, tpad, lpad, rpad, bpad);
//	userAgent.writeDebugImage(imgPadded, "MorphFilter_ErosionPadded");

	IemTImage<short> imgFiltered(1, imgAggregated.rows(), imgAggregated.cols());
	
	long aggregatedCols = imgAggregated.cols();
	long aggregatedRows = imgAggregated.rows();
	
	long templateCols = imgTemplate.cols();
	long templateRows = imgTemplate.rows();
	
	IemTImageIter<short> paddedIter(imgPadded);
	IemTImageIter<short> templateIter(imgTemplate);

	for(long c = 0; c < aggregatedCols; c++) {
		for(long r = 0; r < aggregatedRows; r++) {

			short min = SHRT_MAX;

			for(long rr = 0; rr < templateRows; rr++) {

				paddedIter.setPos(r + rr, c);
				templateIter.setPos(rr, 0);

				for(long cc = 0; cc < templateCols; cc++) {
					short v = paddedIter[0] - templateIter[0];
					if(v < min) min = v;
          			paddedIter.colInc(); templateIter.colInc();
				}
			
			}

			imgFiltered[0][r][c] = min;
	}}

	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}


// Masked vesions

IemTImage<short> MorphFilter::filter(IemTImage<short>& img, short type, IemTImage<unsigned char>& imgMask) {

	IemTImage<short> imgFiltered;
	
	userAgent.writeDebugImage(img, "MorphFilter_Input");
	userAgent.writeDebugImage(imgTemplate, "MorphFilter_Template");

	if(type == DILATE) imgFiltered = basicFilter(img, DILATE, imgMask);
	else if(type == ERODE) imgFiltered = basicFilter(img, ERODE, imgMask);
	else if(type == OPEN) {
		IemTImage<short> imgTmp = basicFilter(img, ERODE, imgMask);
		userAgent.writeDebugImage(imgTmp, "MorphFilter_Eroded");
		imgFiltered = basicFilter(imgTmp, DILATE, imgMask);
		userAgent.writeDebugImage(imgFiltered, "MorphFilter_Opened");
	}
	else if(type == CLOSE) {
		IemTImage<short> imgTmp = basicFilter(img, DILATE, imgMask);
		userAgent.writeDebugImage(imgTmp, "MorphFilter_Eroded");
		imgFiltered = basicFilter(imgTmp, ERODE, imgMask);
		userAgent.writeDebugImage(imgFiltered, "MorphFilter_Closed");
	}

	return imgFiltered;
}


IemTImage<short> MorphFilter::basicFilter(IemTImage<short>& img, short type, IemTImage<unsigned char>& imgMask) {
	if(type == DILATE) return dilation(img, imgMask);
	else if(type == ERODE) return erosion(img, imgMask);
	else return img.copy();
}

IemTImage<short> MorphFilter::dilation(IemTImage<short>& img, IemTImage<unsigned char>& imgMask) {

	imgAggregated = iemAggregateDown(img, aggregation, 0);
	imgMaskAggregated = iemAggregateDown(imgMask, aggregation, 0);

//	"c:/tmp/mask.tif" << imgMask;
//	"c:/tmp/maskAgg.tif" << imgMaskAggregated; 

	long tpad = imgTemplate.rows() / 2;  
	long lpad = imgTemplate.cols() / 2;
	long rpad = imgTemplate.cols() / 2;
	long bpad = imgTemplate.rows() / 2;
		
	imgPadded = ImageUtil::pad(imgAggregated, tpad, lpad, rpad, bpad);
	imgMaskPadded = ImageUtil::pad(imgMaskAggregated, tpad, lpad, rpad, bpad);
	
//	"c:/tmp/imagePadded.tif" << imgPadded;
//	"c:/tmp/maskPadded.tif" << imgMaskPadded;

	IemTImage<short> imgFiltered(1, imgAggregated.rows(), imgAggregated.cols());
	imgFiltered = 0;
	
	long aggregatedCols = imgAggregated.cols();
	long aggregatedRows = imgAggregated.rows();   
	
	long templateCols = imgTemplate.cols();
	long templateRows = imgTemplate.rows();

	IemTImageIter<short> paddedIter(imgPadded);
	IemTImageIter<unsigned char> paddedMaskIter(imgMaskPadded);
	IemTImageIter<short> templateIter(imgTemplate);

	for(long c = 0; c < aggregatedCols; c++) {
		for(long r = 0; r < aggregatedRows; r++) {
		                                         	
		//     if(imgMaskAggregated[0][r][c] == 0) continue;

			short max = SHRT_MIN;

			for(long rr = 0; rr < templateRows; rr++) {

				paddedIter.setPos(r + rr, c);
				paddedMaskIter.setPos(r + rr, c);
				templateIter.setPos(rr, 0);

				for(long cc = 0; cc < templateCols; cc++) {
					short v = paddedIter[0] + templateIter[0];
					if(paddedMaskIter[0] != 0 && v > max) max = v;
          			paddedIter.colInc(); paddedMaskIter.colInc(); templateIter.colInc();
				}
			}

			imgFiltered[0][r][c] = max;
	}}

	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}



IemTImage<short> MorphFilter::erosion(IemTImage<short>& img, IemTImage<unsigned char>& imgMask) {

	imgAggregated = iemAggregateDown(img, aggregation, 0);
	imgMaskAggregated = iemAggregateDown(imgMask, aggregation, 0);

	long tpad = imgTemplate.rows() / 2;
	long lpad = imgTemplate.cols() / 2;
	long rpad = imgTemplate.cols() / 2;
	long bpad = imgTemplate.rows() / 2;
		
	imgPadded = ImageUtil::pad(imgAggregated, tpad, lpad, rpad, bpad);
	imgMaskPadded = ImageUtil::pad(imgMaskAggregated, tpad, lpad, rpad, bpad);

	IemTImage<short> imgFiltered(1, imgAggregated.rows(), imgAggregated.cols());
	imgFiltered = 0;
	
	long aggregatedCols = imgAggregated.cols();
	long aggregatedRows = imgAggregated.rows();
	
	long templateCols = imgTemplate.cols();
	long templateRows = imgTemplate.rows();
	
	IemTImageIter<short> paddedIter(imgPadded);
	IemTImageIter<unsigned char> paddedMaskIter(imgMaskPadded);
	IemTImageIter<short> templateIter(imgTemplate);

	for(long c = 0; c < aggregatedCols; c++) {
		for(long r = 0; r < aggregatedRows; r++) {
		                                         	
		 //    if(imgMaskAggregated[0][r][c] == 0) continue;

			short min = SHRT_MAX;

			for(long rr = 0; rr < templateRows; rr++) {

				paddedIter.setPos(r + rr, c);
				paddedMaskIter.setPos(r + rr, c);
				templateIter.setPos(rr, 0);

				for(long cc = 0; cc < templateCols; cc++) {
					short v = paddedIter[0] - templateIter[0];
					if(paddedMaskIter[0] != 0 && v < min) min = v;
					
					  // if(rr == templateRows/2 && cc == templateCols/2) min = paddedIter[0];
          			paddedIter.colInc(); paddedMaskIter.colInc(); templateIter.colInc();
				}
			}

			imgFiltered[0][r][c] = min;
	}}

	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}

/*
IemTImage<short> MorphFilter::erosion(IemTImage<short>& img, IemTImage<unsigned char>& imgMask) {

	imgAggregated = iemAggregateDown(img, aggregation, 0);
	imgMaskAggregated = iemAggregateDown(imgMask, aggregation, 0);
	
	long cols = imgAggregated.cols();
	long rows = imgAggregated.rows();
	
	long templateCols = imgTemplate.cols();
	long templateRows = imgTemplate.rows();

	IemTImage<short> imgFiltered(1, rows, cols);

	IemTImageIter<short> imgIter(imgAggregated);
	IemTImageIter<unsigned char> maskIter(imgMaskAggregated);
	IemTImageIter<short> templateIter(imgTemplate);

	for(long c = 0; c < aggregatedCols; c++) {
		for(long r = 0; r < aggregatedRows; r++) {
		                                         	
			short min = SHRT_MAX;
		                                         	
		     if(imgMaskAggregated[0][r][c] == min) continue;


			for(long rr = 0; rr < templateRows; rr++) {

				paddedIter.setPos(r + rr, c);
				paddedMaskIter.setPos(r + rr, c);
				templateIter.setPos(rr, 0);

				for(long cc = 0; cc < templateCols; cc++) {
					short v = imgIter[0] - templateIter[0];
					if(paddedMaskIter[0] != 0 && v < min) min = v;
          			imgIter.colInc(); imgMaskIter.colInc(); templateIter.colInc();
				}
			}

			imgFiltered[0][r][c] = min;
	}}

	return iemInterpolate(imgFiltered, img.rows(), img.cols());
}

*/


	







	






