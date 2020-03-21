//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "MultiMorphFilter.h"
#include "MorphFilter.h"
#include "ImageUtil.h"
#include "CadxParm.h" 
#include "Histogram.h" 
#include "Iem/IemImageIO.h"
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  


using namespace CADX_SEG; 




MultiMorphFilter::MultiMorphFilter(IemTImage<short>* _pImgTemplate, short _nTemplates, 
 short _aggregation, double _templateHeightScale) {

	initialize();
	
	nTemplates = _nTemplates;
	
	pImgTemplate = new IemTImage<short>[nTemplates];

	aggregation = _aggregation;
	templateHeightScale = _templateHeightScale;	

	for(long i = 0; i < nTemplates; i++) {
		pImgTemplate[i] = _pImgTemplate[i].copy();
	}
}


MultiMorphFilter::~MultiMorphFilter() {
	if(pImgTemplate != NULL) delete[] pImgTemplate;
}


void MultiMorphFilter::scaleTemplates(double scale) {

	if(scale == 1.0) return;

	for(long i = 0; i < nTemplates; i++) {
	     long rows = scale * pImgTemplate[i].rows();
	     long cols = scale * pImgTemplate[i].cols();
		pImgTemplate[i] = iemInterpolate(pImgTemplate[i], rows, cols);
	}
}


void MultiMorphFilter::initialize() {
	nTemplates = 0;
	pImgTemplate = NULL;
	aggregation = 0;
	templateHeightScale = 1.0;
	debug = 0;
	strcpy(outputDir, "");
	strcpy(name, "");
}


IemTImage<short> MultiMorphFilter::filter(IemTImage<short>& img, long band, short type) {

	IemTImage<short> imgFiltered(img.chans(), img.rows(), img.cols());
	
	userAgent.writeDebugImage(img, "MultiMorphFilter_Input");

	if(type == MorphFilter::OPEN || type == MorphFilter::DILATE) imgFiltered = SHRT_MIN;
	else if(type == MorphFilter::CLOSE || type == MorphFilter::ERODE) imgFiltered = SHRT_MAX;
	
	char tag[1024], oldTag[1024];
	strcpy(oldTag, userAgent.getTag());

	for(long i = 0; i < nTemplates; i++) {
	     sprintf(tag, "%s_%d", oldTag, i);
	     userAgent.setTag(tag);

		MorphFilter morphFilter(pImgTemplate[i], aggregation, templateHeightScale);
          morphFilter.setUserAgent(userAgent);
		
		IemTImage<short> imgTmp = morphFilter.filter(img, type);

		if(type == MorphFilter::OPEN || type == MorphFilter::DILATE) {
			imgFiltered = ImageUtil::max(imgFiltered, imgTmp, 0);
		}
		else if(type == MorphFilter::CLOSE || type == MorphFilter::ERODE) {
			imgFiltered = ImageUtil::min(imgFiltered, imgTmp, 0);
		}
	}

	return imgFiltered;
}



IemTImage<short> MultiMorphFilter::filter(IemTImage<short>& img, long band, short type,
 IemTImage<unsigned char>& imgMask) {

	IemTImage<short> imgFiltered(img.chans(), img.rows(), img.cols());
	
	userAgent.writeDebugImage(img, "MultiMorphFilter_Input");
	userAgent.writeDebugImage(imgMask, "MultiMorphFilter_Mask");
	
	if(type == MorphFilter::OPEN || type == MorphFilter::DILATE) imgFiltered = SHRT_MIN;
	else if(type == MorphFilter::CLOSE || type == MorphFilter::ERODE) imgFiltered = SHRT_MAX;
	
	char tag[1024], oldTag[1024];
	strcpy(oldTag, userAgent.getTag());

	for(long i = 0; i < nTemplates; i++) {
	     sprintf(tag, "%s_%d", oldTag, i);
	     userAgent.setTag(tag);
	
		MorphFilter morphFilter(pImgTemplate[i], aggregation, templateHeightScale);
		morphFilter.setUserAgent(userAgent);

		IemTImage<short> imgTmp = morphFilter.filter(img, type, imgMask);

		if(type == MorphFilter::OPEN || type == MorphFilter::DILATE) {
			imgFiltered = ImageUtil::max(imgFiltered, imgTmp, 0);
		}
		else if(type == MorphFilter::CLOSE || type == MorphFilter::ERODE) {
			imgFiltered = ImageUtil::min(imgFiltered, imgTmp, 0);
		}
	}

	return imgFiltered;
}



IemTImage<short> MultiMorphFilter::filterDeltaOpen(IemTImage<short>& img) { 

	char outFileName[1024];

	IemTImage<short> imgMax(img.chans(), img.rows(), img.cols());
	imgMax = SHRT_MIN;
	
	IemTImage<short> imgMin(img.chans(), img.rows(), img.cols());
	imgMin = SHRT_MAX;

	for(long i = 0; i < nTemplates; i++) {
	
		MorphFilter morphFilter(pImgTemplate[i], aggregation, templateHeightScale);

		IemTImage<short> imgTmp = morphFilter.filter(img, MorphFilter::OPEN);

		if(debug) {
			sprintf(outFileName, "%s/%s_multiMorphFilter_%d.tif", outputDir, name, i);
			iemWrite(imgTmp, outFileName);
			sprintf(outFileName, "%s/%s_multiMorphFilter_template_%d.tif", outputDir, name, i);
			iemWrite(morphFilter.getTemplate(), outFileName);
		}

		imgMax = ImageUtil::max(imgMax, imgTmp, 0);
		imgMin = ImageUtil::min(imgMin, imgTmp, 0);
	}

	return imgMax - imgMin;
}
