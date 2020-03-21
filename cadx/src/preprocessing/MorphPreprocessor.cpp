//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//

#include "MorphPreprocessor.h"
#include "Iem/maximumFxns.h"
#include "Iem/minimumFxns.h"
#include "Iem/IemImageIO.h"
#include "MorphFilter.h"
#include "MultiMorphFilter.h"
#include "ImageUtil.h"
#include "CadxParm.h"



using namespace CADX_SEG;


MorphPreprocessor::MorphPreprocessor() {
	initialize();
}



MorphPreprocessor::MorphPreprocessor(IemTImage<short>& _imgNoduleTemplate, short _nNoduleAggregations, double _noduleTemplateMultiplier,
 IemTImage<short>* _imgRibTemplate, short _nRibTemplates, short _nRibAggregations, double _ribTemplateMultiplier) {

	initialize();

	imgNoduleTemplate = _imgNoduleTemplate.copy();
	nNoduleAggregations = _nNoduleAggregations;
	noduleTemplateMultiplier = _noduleTemplateMultiplier;

	imgRibTemplate = _imgRibTemplate;
	nRibTemplates = _nRibTemplates;
	
	nRibAggregations = _nRibAggregations; 
	ribTemplateMultiplier = _ribTemplateMultiplier;
}



void MorphPreprocessor::initialize() {
	noduleTemplateMultiplier = 0.5;
	nNoduleAggregations = 0;

	ribTemplateMultiplier = 0.5;
	ribTemplateScale = 1.0;
	nRibAggregations = 0;
	nRibTemplates = 0;
}  


void MorphPreprocessor::apply() {  
	makeRibImage();

	makeNoduleImage();
 
	imgPreprocessed = imgNodule - imgRib;

//	IemTImage<unsigned char> imgLungMaskEroded = ImageUtil::erodeMap(imgLungMask, 5);
//	imgPreprocessed = ImageUtil::applyMask(imgPreprocessed, 0, imgLungMask, 0, 0);
//	userAgent.writeDebugImage(imgPreprocessed, "MorphPreprocessor_NoduleRib");
}       


IemTImage<short> MorphPreprocessor::apply(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask) {
	img = _img;
	imgLungMask = _imgMask;
	
	makeClearLungFieldImage();
	userAgent.writeDebugImage(imgClearLung, "MorphPreprocessor_ClearLung");

	apply();

	return imgPreprocessed;
} 


IemTImage<short> MorphPreprocessor::apply(IemTImage<short>& _img) { 
	img = _img;
	
	imgClearLung = img;

	apply();

	return imgPreprocessed;
}


IemTImage<short> MorphPreprocessor::apply(IemTImage<short>& _img, IemTImage<short>& _imgRib) {
	img = _img;
	imgRib = _imgRib;

	imgClearLung = img;

//////////////////////////////////////////////////
		IemImage img8bit(IemByte, 1, imgRib.rows(), imgRib.cols());

	long rows = imgRib.rows();
	long cols = imgRib.cols();

	short chen=0;
	for(long m=0;m<rows;m++){
		for(long n=0;n<cols;n++){
			if(chen<=imgRib[0][m][n])
				chen=imgRib[0][m][n];
	}}

	double k = (255.0 / double(chen));

	for(long r = 0; r < rows; r++) {
		for(long c = 0; c < cols; c++) {

			short v = k * imgRib[0][r][c];

			if(v > 255) v = 255;
			if(v < 0) v = 0;
		                        	
			img8bit.put(0, r, c, v);		                               	
	 }}
    IemTImage<unsigned char> imagex=img8bit;
//////////////////////////////////////////////////
	userAgent.writeDebugImage(imagex, "MorphPreprocessor_Rib");

	makeNoduleImage();
	//////////////////////////////////////////////////
	IemImage img8bit1(IemByte, 1, imgNodule.rows(), imgNodule.cols());

	long rows1 = imgNodule.rows();
	long cols1 = imgNodule.cols();

	for(long r1 = 0; r1 < rows1; r1++) {
		for(long c1 = 0; c1 < cols1; c1++) {

			short v1 = k * imgNodule[0][r1][c1];

			if(v1 > 255) v1= 255;
			if(v1 < 0) v1 = 0;
		                        	
			img8bit1.put(0, r1, c1, v1);		                               	
	 }}
    IemTImage<unsigned char> imagex1=img8bit1;
//////////////////////////////////////////////////
	userAgent.writeDebugImage(imagex1, "MorphPreprocessor_Nodule");

	imgPreprocessed = imgNodule - imgRib;
	//////////////////////////////////////////////////
	IemImage img8bit2(IemByte, 1, imgPreprocessed.rows(), imgPreprocessed.cols());

	long rows2 = imgPreprocessed.rows();
	long cols2 = imgPreprocessed.cols();

	for(long r2 = 0; r2 < rows2; r2++) {
		for(long c2 = 0; c2 < cols2; c2++) {

			short v2 = k * imgPreprocessed[0][r2][c2];

			if(v2 > 255) v2 = 255;
			if(v2 < 0) v2 = 0;
		                        	
			img8bit2.put(0, r2, c2, v2);		                               	
	 }}
    IemTImage<unsigned char> imagex2=img8bit2;
//////////////////////////////////////////////////
	userAgent.writeDebugImage(imagex2, "MorphPreprocessor_NoduleRib");

	return imgPreprocessed;
}


void MorphPreprocessor::makeRibImage() {
	MultiMorphFilter multiMorphFilter(imgRibTemplate, nRibTemplates, nRibAggregations, ribTemplateMultiplier);
	UserAgent newUserAgent(userAgent, "_Rib");
//	multiMorphFilter.setUserAgent(newUserAgent);
	multiMorphFilter.scaleTemplates(ribTemplateScale);
	imgRib = multiMorphFilter.filter(imgClearLung, 0, MorphFilter::OPEN);
}


void MorphPreprocessor::makeNoduleImage() {
	MorphFilter morphFilter(imgNoduleTemplate, nNoduleAggregations, noduleTemplateMultiplier);
	UserAgent newUserAgent(userAgent, "_Nodule");
//	morphFilter.setUserAgent(newUserAgent);
	imgNodule = morphFilter.filter(imgClearLung, MorphFilter::OPEN);
}


void MorphPreprocessor::makeClearLungFieldImage() {

	imgClearLung = img.copy();
	
	IemTImage<unsigned char> imgLungMaskDilated = ImageUtil::dilateMap(imgLungMask, 13);

//	short v = iemMinimum(img);
//	short v = getClearLungAvgCV();
	short v = 0;

	long rows = img.rows();
	long cols = img.cols(); 

	for(long r = 0; r < rows; r++) {
		for(long c = 0; c < cols; c++) {
			if(imgLungMaskDilated[0][r][c] == 0) {imgClearLung[0][r][c] = v;}
			//if(imgLungMask[0][r][c] == 0) {imgClearLung[0][r][c] = v;}
	}} 
	
}


short MorphPreprocessor::getClearLungAvgCV() {

	double sum = 0.0;
	long n = 0;

	long rows = img.rows();
	long cols = img.cols(); 

	for(long r = 0; r < rows; r++) {
		for(long c = 0; c < cols; c++) {
			if(imgLungMask[0][r][c] != 0) {
			     sum += img[0][r][c];
			     n++;
			}
	}} 

	return sum / (double)n;
}

