//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "Iem/Iem.h"
#include "Iem/IemImageIO.h" 



#include "MatchedFilter.h"
#include "Kespr.h"
#include "EdgeDetector.h" 
#include "CurvatureFilter.h" 
#include "BlurFilter.h"  
#include "ScalingFilter.h"   
#include "MorphFilter.h"
#include "MultiMorphFilter.h"
#include "MultiMatchedFilter.h"
#include "ImageUtil.h"
#include "CadxParm.h"
#include "ImageReader.h"



IemTImage<short> ribFilter(IemTImage<short>& img);



int main(int argc, char* argv[])
{                    
	short narg = 0, debug = 1, size = 128, noduleIndex = 0;   
	char inFileName[1024], outFileName[1024], parmFileName[1024];    
	
	
	// Read command line.
	while((++narg) < argc) {
		
		if(strcmp(argv[narg], "-i") == 0) {
			narg++; strcpy(inFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-o") == 0) {
			narg++; strcpy(outFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}  
		else if(strcmp(argv[narg], "-noduleIndex") == 0) {
			narg++; noduleIndex = atoi(argv[narg]);
		}  
	}


	CadxParm::getInstance().readFile(parmFileName);   

	cout << "\nReading image file" << flush;
	ImageReader reader;
	IemTImage<short> img = reader.read(inFileName);
	
	cout << "\nInvert image" << flush;
	IemTImage<short> imgInverted = ImageUtil::invert(img, CadxParm::getInstance().getMinCodeValue(), CadxParm::getInstance().getMaxCodeValue());

	cout << "\nRib opening" << flush;
	IemTImage<short> imgRibs = ribFilter(imgInverted);


	IemTImage<short> imgTemplate = CadxParm::getInstance().getNoduleTemplate(noduleIndex);
	long nNoduleAggregations = CadxParm::getInstance().getNNoduleAggregations(noduleIndex);

	cout << "\nNodule opening" << flush;
	MorphFilter morphFilter(imgTemplate, nNoduleAggregations);
	IemTImage<short> imgNodule = morphFilter.filter(imgInverted, MorphFilter::OPEN);

	cout << "\nDif image" << flush;
	IemTImage<short> imgDif = imgNodule - imgRibs;

	cout << "\nScaling image" << flush;
	ScalingFilter scalingFilter(2000, 400, CadxParm::getInstance().getMinCodeValue(),
    CadxParm::getInstance().getMaxCodeValue());

	IemTImage<short> imgDifScaled = scalingFilter.globalScaling(imgDif);

	outFileName << imgDifScaled;

	return 0;
}


IemTImage<short> ribFilter(IemTImage<short>& img) {
	short nTemplates = CadxParm::getInstance().getNRibTemplates();
	IemTImage<short>* pRibTemplates = new IemTImage<short>[nTemplates];
	for(short j = 0; j < nTemplates; j++) pRibTemplates[j] = CadxParm::getInstance().getRibTemplate(j);
	long nRibAggregations = CadxParm::getInstance().getNRibAggregations();
	MultiMorphFilter multiMorphFilter(pRibTemplates, nTemplates, nRibAggregations, 1.0);
	return multiMorphFilter.filter(img, MorphFilter::OPEN);
}



