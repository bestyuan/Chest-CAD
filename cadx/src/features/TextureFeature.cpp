//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "TextureFeature.h"
#include "EdgeDetector.h"
#include "ScalingFilter.h"
#include "Util.h"
#include "CadxParm.h"
#include "Iem/IemImageIO.h"
#include "Iem/minimumFxns.h"
#include "Iem/maximumFxns.h"


using namespace CADX_SEG;


TextureFeature::TextureFeature()
 : Feature() {     
 	

}  


TextureFeature::TextureFeature(char* _labelRoot)
 : Feature(6, _labelRoot) {

	initialize();
}


TextureFeature::TextureFeature(char* _labelRoot, IemTImage<short>& _img, IemTImage<unsigned char>& _imgMap)
 : Feature(6, _labelRoot) {
 	
 	initialize();  
 	
 	img = _img; 		
	imgMap = _imgMap;
}


double TextureFeature::getScore() {
	
	return value[0];
}


void TextureFeature::calculate() {

	short max;

	imgAgg = iemAggregateDown(img, aggregation, 0);
	imgMapAgg = iemAggregateDown(imgMap, aggregation, 0);

	EdgeDetector edgeDetector;
	imgEdge = edgeDetector.calcSobelGradient(imgAgg);

	max = iemMaximum(imgAgg);
	Cooccurrence cooccurrence1;
	cooccurrence1.calculate(imgAgg, 0, 0, max, imgMapAgg);
	value[0] = cooccurrence1.getEnergy();
	value[1] = cooccurrence1.getConstrast();
	
	max = iemMaximum(imgEdge[0]);
	Cooccurrence cooccurrence2;
	cooccurrence2.calculate(imgEdge, 0, 0, max, imgMapAgg);
	value[2] = cooccurrence2.getEnergy();
	value[3] = cooccurrence2.getConstrast();
	
	Cooccurrence cooccurrence3;
	cooccurrence3.calculate(imgEdge, 1, 0, 360, imgMapAgg);
	value[4] = cooccurrence3.getEnergy();
	value[5] = cooccurrence3.getConstrast();
} 
 

void TextureFeature::initialize() { 

	aggregation = 2;

	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Texture");
	
	sprintf(label[0], "%s.energy", labelRoot);
	sprintf(label[1], "%s.contrast", labelRoot);
	sprintf(label[2], "%s.energyEdgeMag", labelRoot);
	sprintf(label[3], "%s.contrastEdgeMag", labelRoot);
	sprintf(label[4], "%s.energyEdgeDir", labelRoot);
	sprintf(label[5], "%s.contrastEdgeDir", labelRoot);
}


/*
void TextureFeature::scaleImage() {

	double meanAim = 30.0;
	double sigmaAim =  30.0 / 5.0;

	ScalingFilter scalingFilter(meanAim, sigmaAim, 0, 100);

	imgQuantized = scalingFilter.globalScaling(img, 0, imgMap);
}
*/

 


 

