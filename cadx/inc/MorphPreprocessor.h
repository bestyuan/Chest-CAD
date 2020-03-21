//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MORPHPREPROCESSOR_H
#define MORPHPPREPROCESSOR_H


#include "Iem/Iem.h"
#include "Roi.h" 
#include "Hit.h"
#include "UserAgent.h"


namespace CADX_SEG {


class MorphPreprocessor {

	protected:  

	// Input image.
	IemTImage<short> img;
	
	// Preprocessed image.
	IemTImage<short> imgPreprocessed;
	
	// Image with enhanced ribs.
	IemTImage<short> imgRib;

	// Image with enhanced nodules.
	IemTImage<short> imgNodule;

	// Input image with pixels outside clear lung field
	// set to min value.
	IemTImage<short> imgClearLung;

	IemTImage<unsigned char> imgLungMask;
	
	// Nodule template.
	IemTImage<short> imgNoduleTemplate;
	
	// Number of image aggregations before nodule template is applied.
	short nNoduleAggregations;
	
	// Nodule template height is multiplied by this factor before it is applied.
	double noduleTemplateMultiplier;


     // Pointer to array of rib templates.
	IemTImage<short>* imgRibTemplate;
	
	// Number of rib templates.
	short nRibTemplates;
	
	// Number of image aggregations before rib templates are applied.
	short nRibAggregations;
	
	// Rib templates height are multiplied by this factor before they are applied.
	double ribTemplateMultiplier;

	double ribTemplateScale;

	UserAgent userAgent;


	public:
	
	MorphPreprocessor();

	MorphPreprocessor(IemTImage<short>& imgNoduleTemplate, short nNoduleAggregations, double noduleTemplateMultiplier,
	 IemTImage<short>* imgRibTemplate, short nRibTemplates, short nRibAggregations, double ribTemplateMultiplier);

	IemTImage<short> apply(IemTImage<short>& img, IemTImage<unsigned char>& imgMask);

	IemTImage<short> apply(IemTImage<short>& img);

	// Use when rib image has already been produced.	
	IemTImage<short> apply(IemTImage<short>& img, IemTImage<short>& imgRib);
	
	IemTImage<short> getRibImage() {return imgRib;}
	

	void setUserAgent(UserAgent& agent) {userAgent = agent;}
	
	void setRibTemplateScale(double s) {ribTemplateScale = s;}

		
	protected:

	void initialize();

	void apply();
	
	void makeRibImage();
	
	void makeNoduleImage();
	
	void makeClearLungFieldImage();
	
	short getClearLungAvgCV();
	


};   


} // Namespace CADX_SEG


#endif
