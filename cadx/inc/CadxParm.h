//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CADXPARM_H
#define CADXPARM_H


#include "Util.h"  
#include "Iem/Iem.h" 


namespace CADX_SEG {


class CadxParm {  
 
	private:
	static CadxParm instance;
	
	// Image max code value. 
	long maxCodeValue; 
	
	// Image min code value. 	
	long minCodeValue; 	
	
	// The algorithm expects that high density regions in the
	// object correspond to low code values in the input image.
	// For example, if the input image is displayed as is bones
	// appear dark. In this case set invert to 1.
	// If the image has been rendered in the standard way so
	// that high density regions of the object appear bright
	// set invert equal to 0.
	bool invert;
	
	// Size of Roi in pixels ROI. 
	long roiResolution;
	
	// Aim spacing between pixels in mm/pixel.
	double aimPixelSpacing;
	
	// Maximum number of candidates to process per image
	long maxCandidates;
	
	// Ranges between 0 and 1. 
	// Region size increases as this value decreases.
	short regionSupportThreshold;
	
	// The number of nodule templates to use.
	// Holds up to 16 nodule template file names. 	
	long nNoduleTemplates;				
	char noduleTemplateFile[16][1024];
	IemTImage<short>* pNoduleTemplates;
	
	// nNoduleAggregations[i] is the number of times the
	// image undergoes 2x2 pixel aggregation before
	// the i'th nodule template is applied.
	long nNoduleAggregations[16];
	
	// The number of rib templates to use.
	// Holds up to 16 rib template file names.
	long nRibTemplates;				
	char ribTemplateFile[16][1024];  
	IemTImage<short>* pRibTemplates;
	
	// nRibAggregations is the number of times the
	// image undergoes 2x2 pixel aggregation before
	// the rib templates are applied.
	long nRibAggregations;

	// Use lung mask.
	bool lungMask;
	
	// Goup overlapping regions.
	bool group;
	double maxOverlap;
	
	// The maximum number of annotations in an image.
	int maxAnnotations;
	      
	// Private contructor.
	CadxParm();   
	
	public:
	// Destructor. 
	~CadxParm();	
	

	
	static CadxParm& getInstance();
	
	void readFile(const char *parmFileName);
	
	long getMaxCodeValue() {return maxCodeValue;}
	void setMaxCodeValue(long cv) {maxCodeValue = cv;}

	long getMinCodeValue() {return minCodeValue;}     
	long getRoiResolution() {return roiResolution;}

	long getNTemplates() {return nNoduleTemplates;} 
	long getNRibTemplates() {return nRibTemplates;} 
	IemTImage<short>* getRibTemplatesPtr() {return pRibTemplates;}

	long getMaxCandidates() {return maxCandidates;}     
	short getRegionSupportThreshold() {return regionSupportThreshold;}
	
	char* getNoduleTemplateFileName(short i) {return noduleTemplateFile[i];}  
	char* getRibTemplateFileName(short i) {return ribTemplateFile[i];}  
	
	IemTImage<short> getNoduleTemplate(short i) {return pNoduleTemplates[i];}
	IemTImage<short> getRibTemplate(short i) {return pRibTemplates[i];} 
	
	long getNNoduleAggregations(short i) {return nNoduleAggregations[i];}  
	long getNRibAggregations() {return nRibAggregations;} 
	
	bool doGrouping() {return group;}
	
	bool doInversion() {return invert;}

	double getMaxOverlap() {return maxOverlap;}
	
	double getAimPixelSpacing() {return aimPixelSpacing;}

	bool useLungMask() {return lungMask;}
	
	int getMaxAnnotations() {return maxAnnotations;}

	void write(ostream& s);	
	
	private:
	
	void readTemplates();


};


} // End namespace

#endif
