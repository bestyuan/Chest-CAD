//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef MULTIMORPHFILTER_H
#define MULTIMORPHFILTER_H

 
#include "Iem/Iem.h"  
#include "UserAgent.h"


namespace CADX_SEG {



class MultiMorphFilter {


	protected:
	IemTImage<short>* pImgTemplate;
	short nTemplates;
	short aggregation;
	double templateHeightScale;
	short debug;
	char outputDir[1024];
	char name[1024];
	
	UserAgent userAgent;


	public:    
	MultiMorphFilter(IemTImage<short>* pImgTemplate, short nTemplates,
	 short aggregation = 0, double templateHeightScale = 1.0);

	~MultiMorphFilter();
	

	IemTImage<short> filter(IemTImage<short>& img, long band, short type);
	
	IemTImage<short> filter(IemTImage<short>& img, long band, short type, IemTImage<unsigned char>& imgMask);

	IemTImage<short> filterDeltaOpen(IemTImage<short>& img);
	
	void setDebug(short _debug) {debug = _debug;}
	void setOutputDir(char* _outputDir) {strcpy(outputDir, _outputDir);}
	void setName(char* _name) {strcpy(name, _name);}
	void setUserAgent(UserAgent& agent) {userAgent = agent;}

	void scaleTemplates(double scale);

	protected:
	
	void initialize();
};


} // End namespace


#endif
