//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "MultiMatchedFilter.h"
#include "MatchedFilter.h"
#include "ImageUtil.h"
#include "CadxParm.h" 
#include "Histogram.h" 
#include "Iem/IemImageIO.h"
#include "Iem/IemPad.h"   
#include "Iem/IemCrop.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  




MultiMatchedFilter::MultiMatchedFilter(IemTImage<short>* _pImgTemplate, 
 short _nTemplates, short _decimation) {

	initialize();
	
	nTemplates = _nTemplates;
	
	decimation = _decimation;
	
	pImgTemplate = new IemTImage<short>[nTemplates];	

	for(long i = 0; i < nTemplates; i++) {
		pImgTemplate[i] = _pImgTemplate[i].copy();
	}
}


MultiMatchedFilter::~MultiMatchedFilter() {
	if(pImgTemplate != NULL) delete[] pImgTemplate;
}


void MultiMatchedFilter::initialize() {

	nTemplates = 0;
	pImgTemplate = NULL;
	decimation = 0;
}


IemTImage<short> MultiMatchedFilter::filter(IemTImage<short>& img) { 

	IemTImage<short> imgFiltered(img.chans(), img.rows(), img.cols());
	
	imgFiltered = SHRT_MIN;

	for(long i = 0; i < nTemplates; i++) {
	
		MatchedFilter matchedFilter(pImgTemplate[i], decimation); 
		
		IemTImage<short> imgTmp = matchedFilter.filter(img);

		imgFiltered = ImageUtil::max(imgFiltered, imgTmp);
	}

	return imgFiltered;
}




