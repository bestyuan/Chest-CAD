//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "ImageUtil.h"
#include "Iem/IemImageIO.h"
#include "Iem/IemMorphological.h"
#include "Iem/IemTImageIter.h"
#include "Histogram.h"
#include "CadxParm.h"


  
using namespace CADX_SEG;

   
IemTImage<short> ImageUtil::invert(IemTImage<short>& img, short min, short max) {

	IemTImage<short> imgInverted(img.chans(), img.rows(), img.cols());
	
   	for(long b = 0; b < img.chans(); b++) {
		for(long c = 0; c < img.cols(); c++) {
			for(long r = 0; r < img.rows(); r++) {

				short v = max - img[b][r][c];
	                
				if(v >= min) imgInverted[b][r][c] = v;
				else imgInverted[b][r][c] = min;
	}}}  
	
	return imgInverted;     
}


IemTImage<short> ImageUtil::subtract(IemTImage<short>& img1, IemTImage<short>& img2) {

	IemTImage<short> img(img1.chans(), img1.rows(), img1.cols());
	
   	for(long b = 0; b < img.chans(); b++) {
		for(long c = 0; c < img.cols(); c++) {
			for(long r = 0; r < img.rows(); r++) {

				img[b][r][c] = img1[b][r][c] - img2[b][r][c];
	}}}  
	
	return img;
}


IemTImage<unsigned char> ImageUtil::imageToSet(IemTImage<short>& img, short threshold) {

	IemTImage<unsigned char> imgSet(img.chans(), img.rows(), img.cols());
	imgSet = 0;

	for(long b = 0; b < img.chans(); b++) {
		for(long r = 0; r < img.rows(); r++) {		                                        
			for(long c = 0; c < img.cols(); c++) {  
			
				if(img[b][r][c] >= threshold) imgSet[b][r][c] = 255;
	}}}	

	return imgSet;
}


IemTImage<unsigned char> ImageUtil::imageToSetUpperHist(IemTImage<short>& img, double upperPenetration) {

	Histogram histogram(CadxParm::getInstance().getMinCodeValue(),
	 CadxParm::getInstance().getMaxCodeValue());  

	histogram.build(img);
	 
	short threshold = histogram.getUpperPenetration(upperPenetration);   

	return imageToSet(img, threshold);
}


IemTImage<short> ImageUtil::min(IemTImage<short>& img1, IemTImage<short>& img2, long b) {

	IemTImage<short> imgMin(1, img1.rows(), img1.cols());

	IemTImageIter<short> iter1(img1);
	IemTImageIter<short> iter2(img2);
	IemTImageIter<short> iter3(imgMin);
	
	while(!iter1.rowDone()) {
		while(!iter1.colDone()) {
		                        	
			short v1 = iter1[b];
			short v2 = iter2[b];

			if(v1 < v2) iter3[0] = v1;
			else iter3[0] = v2;

			iter1.colInc(); iter2.colInc(); iter3.colInc();
		}

		iter1.colStart(); iter2.colStart(); iter3.colStart();
		iter1.rowInc(); iter2.rowInc(); iter3.rowInc();
	}

	return imgMin;
}


IemTImage<short> ImageUtil::max(IemTImage<short>& img1, IemTImage<short>& img2, long b) {

	IemTImage<short> imgMax(1, img1.rows(), img1.cols());

	IemTImageIter<short> iter1(img1);
	IemTImageIter<short> iter2(img2);
	IemTImageIter<short> iter3(imgMax);
	
	while(!iter1.rowDone()) {
		while(!iter1.colDone()) {
		                        	
			short v1 = iter1[b];
			short v2 = iter2[b];

			if(v1 > v2) iter3[0] = v1; 
			else iter3[0] = v2;

			iter1.colInc(); iter2.colInc(); iter3.colInc();
		}

		iter1.colStart(); iter2.colStart(); iter3.colStart();
		iter1.rowInc(); iter2.rowInc(); iter3.rowInc();
	}

	return imgMax;
}


IemTImage<short> ImageUtil::pad(IemTImage<short>& img, short tpad, short lpad, short rpad, short bpad) {

	long v, r, c, rr, cc;

	IemTImage<short> imgPadded(img.chans(), img.rows() + tpad + bpad, img.cols() + lpad + rpad);
	imgPadded = 0;
	
	for(long b = 0; b < imgPadded.chans(); b++) {
	
		for(r = 0, rr = tpad; r < img.rows(); r++, rr++) {
			for(c = 0, cc = lpad; c < img.cols(); c++, cc++) {
				imgPadded[b][rr][cc] = img[b][r][c];
		}}

		for(r = tpad; r < tpad + img.rows(); r++) {
			v = imgPadded[b][r][lpad];
			for(c = 0; c < lpad; c++) {
				imgPadded[b][r][c] = v;
			}
			v = imgPadded[b][r][lpad + img.cols() - 1];
			for(c = lpad + img.cols(); c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = v;
			}
		}

		for(r = tpad; r < tpad + img.rows(); r++) {
			v = imgPadded[b][r][lpad];
			for(c = 0; c < lpad; c++) {
				imgPadded[b][r][c] = v;
			}
			v = imgPadded[b][r][lpad + img.cols() - 1];
			for(c = lpad + img.cols(); c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = v;
			}
		}

		for(r = 0; r < tpad; r++) {
			for(c = 0; c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = imgPadded[b][tpad][c];
		}}

		rr = tpad + img.rows() - 1;
		for(r = rr + 1; r < imgPadded.rows(); r++) {
			for(c = 0; c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = imgPadded[b][rr][c];
		}}
	}

	return imgPadded;
}


IemTImage<unsigned char> ImageUtil::mapToBoundary(IemTImage<unsigned char>& img, 
	long* &xPoints, long* &yPoints, long& nPoints) {
	
	nPoints = 0;

	IemTImage<unsigned char> imgBoundary(1, img.rows(), img.cols());
	imgBoundary = 0;
	
	long rows = img.rows() - 1;
	long cols = img.cols() - 1;

	for(long r = 0; r <= rows; r++) {
		for(long c = 0; c <= cols; c++) {
			
			if(img[0][r][c] != 0) {

			   short isBoundary = 0;
			   
			   // Region boundary limited by ROI size.
			   if(r == 0 || c == 0 || r == rows || c == cols) {isBoundary = 1;}
			
				// Real region boundary.
				else if((img[0][r-1][c] == 0) || (img[0][r+1][c] == 0)
				 || (img[0][r][c-1] == 0) || (img[0][r][c+1] == 0)) {
			 
			 		isBoundary = 1;
				}

				if(isBoundary) {
			 		imgBoundary[0][r][c] = 255;
					nPoints++;
				}
			}
	}}	
	
	xPoints = new long[nPoints];
	yPoints = new long[nPoints];	
	long n = 0;

	for(r = 0; r <= rows; r++) {
		for(long c = 0; c <= cols; c++) {
			
			if(imgBoundary[0][r][c] == 255) {
				xPoints[n] = c;
				yPoints[n] = r;	
				n++;
			}
	}}		

	return imgBoundary;
}


IemTImage<unsigned char> ImageUtil::dilateMap(IemTImage<unsigned char>& _imgMap, short d,
 short aggregation) {

	long cols = _imgMap.cols();
	long rows = _imgMap.rows();

	IemTImage<unsigned char> imgMap = iemAggregateDown(_imgMap, aggregation, 0);
	imgMap = iemDilate(imgMap, d, d);
	imgMap = iemInterpolate(imgMap, rows, cols);

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
		//	if(imgMap[0][y][x] < 255) imgMap[0][y][x] = 0;
			if(imgMap[0][y][x] >= 128) imgMap[0][y][x] = 255;
			else imgMap[0][y][x] = 0;
	}}

	return imgMap;
}


IemTImage<unsigned char> ImageUtil::erodeMap(IemTImage<unsigned char>& _imgMap, short d,
 short aggregation) {

	long cols = _imgMap.cols();
	long rows = _imgMap.rows();

	IemTImage<unsigned char> imgMap = iemAggregateDown(_imgMap, aggregation, 0);
	imgMap = iemErode(imgMap, d, d);
	imgMap = iemInterpolate(imgMap, rows, cols);

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
		//	if(imgMap[0][y][x] < 255) imgMap[0][y][x] = 0;
			if(imgMap[0][y][x] >= 128) imgMap[0][y][x] = 255;
			else imgMap[0][y][x] = 0;
	}}

	return imgMap;
}


IemTImage<short> ImageUtil::applyMask(IemTImage<short>& img, long band,
 IemTImage<unsigned char>& imgMask, unsigned char maskValue, short setValue) {
                                                            	
	IemTImage<short> imgOut = img.copy();
                                                            	
	long cols = img.cols();
	long rows = img.rows();

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(imgMask[0][y][x] == maskValue) imgOut[band][y][x] = setValue;
	}}

	return imgOut;
}


IemTImage<unsigned char> ImageUtil::pad(IemTImage<unsigned char>& img, short tpad, short lpad, short rpad, short bpad) {

	long v, r, c, rr, cc;

	IemTImage<unsigned char> imgPadded(img.chans(), img.rows() + tpad + bpad, img.cols() + lpad + rpad);
	imgPadded = 0;
	
	for(long b = 0; b < imgPadded.chans(); b++) {
		for(r = 0, rr = tpad; r < img.rows(); r++, rr++) {
			for(c = 0, cc = lpad; c < img.cols(); c++, cc++) {
				imgPadded[b][rr][cc] = img[b][r][c];
		}}

		for(r = tpad; r < tpad + img.rows(); r++) {
			v = imgPadded[b][r][lpad];
			for(c = 0; c < lpad; c++) {
				imgPadded[b][r][c] = v;
			}
			v = imgPadded[b][r][lpad + img.cols() - 1];
			for(c = lpad + img.cols(); c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = v;
			}
		}

		for(r = tpad; r < tpad + img.rows(); r++) {
			v = imgPadded[b][r][lpad];
			for(c = 0; c < lpad; c++) {
				imgPadded[b][r][c] = v;
			}
			v = imgPadded[b][r][lpad + img.cols() - 1];
			for(c = lpad + img.cols(); c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = v;
			}
		}

		for(r = 0; r < tpad; r++) {
			for(c = 0; c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = imgPadded[b][tpad][c];
		}}

		rr = tpad + img.rows() - 1;
		for(r = rr + 1; r < imgPadded.rows(); r++) {
			for(c = 0; c < imgPadded.cols(); c++) {
				imgPadded[b][r][c] = imgPadded[b][rr][c];
		}}
	}

	return imgPadded;
}


void ImageUtil::threshold(IemTImage<unsigned char>& img, long b,
 short threshold, short lowCv, short highCv) {

	long cols = img.cols();
	long rows = img.rows();

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(img[b][y][x] >= threshold) img[b][y][x] = highCv;
			else img[b][y][x] = lowCv;
	}}
}


IemTImage<unsigned char> ImageUtil::codevalueToMask(IemTImage<unsigned char>& img,
 short codeValue, short maskCodeValue, double& meanX, double& meanY) {
                            	
     long sumX = 0, sumY = 0, nPixels = 0;

	IemTImage<unsigned char> imgOut(1, img.rows(), img.cols());
                                                            	
	long cols = img.cols();
	long rows = img.rows();

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(img[0][y][x] == codeValue) {
				nPixels++; sumX += x; sumY += y;
			     imgOut[0][y][x] = maskCodeValue;
			}
			else imgOut[0][y][x] = 0;
	}}

	if(nPixels > 0) {
		meanX = (double)sumX / (double)nPixels;
		meanY = (double)sumY / (double)nPixels;
	}
	else {meanX = meanX = 0.0;}
	
	return imgOut;
}


void ImageUtil::makeMark(IemTImage<short>& img, long col, long row, short r, short g, short b, short size) {

	long rows = img.rows();
	long cols = img.cols();

	for(long cc = col - size; cc <= col + size; cc++) {
		for(long rr = row - size; rr <= row + size; rr++) {

			if(rr < 0 || rr >= rows) continue;
			if(cc < 0 || cc >= cols) continue;
			
			img[0][rr][cc] = r; 
			img[1][rr][cc] = g;
			img[2][rr][cc] = b;
   }}

}


void ImageUtil::makeMark(IemTImage<unsigned char>& img, long col, long row, short r, short g, short b, short size) {

	long rows = img.rows();
	long cols = img.cols();

	for(long cc = col - size; cc <= col + size; cc++) {
		for(long rr = row - size; rr <= row + size; rr++) {

			if(rr < 0 || rr >= rows) continue;
			if(cc < 0 || cc >= cols) continue;
			
			img[0][rr][cc] = r; 
			img[1][rr][cc] = g;
			img[2][rr][cc] = b;
   }}

}


IemTImage<unsigned char> ImageUtil::mapResize(IemTImage<unsigned char>& imgMap, long cols, long rows) {

	IemTImage<unsigned char> imgMapResized = iemInterpolate(imgMap, rows, cols);

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
			if(imgMapResized[0][y][x] >= 128) imgMapResized[0][y][x] = 255;
			else imgMapResized[0][y][x] = 0;
	}}

	return imgMapResized;
}


IemTImage<short> ImageUtil::applyDoubleThreshold(IemTImage<short>& img, long b,
 short min, short max) {
                                                            	
	IemTImage<short> imgOut = img.copy();
                                                            	
	long cols = img.cols();
	long rows = img.rows();

	for(long x = 0; x < cols; x++) {
		for(long y = 0; y < rows; y++) {
		     short v = img[b][y][x];
			if(v < min || v > max) imgOut[b][y][x] = 0;
	}}

	return imgOut;
}






