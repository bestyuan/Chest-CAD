//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


 
#include "Iem/IemCore.h"   
#include "Iem/IemImageIO.h" 
#include "Iem/IemCrop.h"   
#include "Iem/rasterOps.h" 
#include "Iem/IemInterpolate.h"
#include "Iem/IemMorphological.h"  
#include "Iem/IemAggregate.h"  


#include "MatchedFilter.h"
#include "Kespr.h"  
#include "EdgeDetector.h" 
#include "CurvatureFilter.h" 
#include "BlurFilter.h"  
#include "ScalingFilter.h"   
#include "MorphFilter.h"
#include "MultiMorphFilter.h"
#include "ImageUtil.h"
#include "CadxParm.h"


using namespace CADX_SEG;

IemTImage<short> makeNoduleTemplate(short size, double sigma);
IemTImage<short> makeHorRibTemplate(short size, short t, short amp);
IemTImage<short> makeVerRibTemplate(short size, short t, short amp);
IemTImage<short> makeDiagRibTemplate(short size, short t, short amp, short slant);

IemTImage<short> makeRibTemplate(short size, short t, short amp, double theta);


IemTImage<short> addNodule(IemTImage<short>& _imgTest, double sigma, 
 long c0, long r0, double amplitude, double tranparency);

IemTImage<short> addRib(IemTImage<short>& _imgTest, short c0, short r0, 
  double amplitude, double tranparency);

IemTImage<short> addHeart(IemTImage<short>& _imgTest, long c1, long c2, 
  double amplitude, double tranparency);


IemTImage<short> makeTestImage();

IemTImage<unsigned char> makeTestMask();




short maxCodeValue = 4095;
short minCodeValue = 0;


int main(int argc, char* argv[])
{                    
	short narg = 0, debug = 1, size = 128;   
	double sigma = 30.0, upperPenetration = 0.33;
	char outFileName[1024], parmFileName[1024];    
	
	
	// Read command line.
	while((++narg) < argc) {
		

		if(strcmp(argv[narg], "-o") == 0) {
			narg++; strcpy(outFileName, argv[narg]);
		}
		else if(strcmp(argv[narg], "-parm") == 0) {
			narg++; strcpy(parmFileName, argv[narg]);
		}  
		else if(strcmp(argv[narg], "-size") == 0) {
			narg++; size = atoi(argv[narg]);
		}
		else if(strcmp(argv[narg], "-sigma") == 0) {
			narg++; sigma = atof(argv[narg]);
		}

 
	} 




	
//	PolyFit::test(); return 0;

	strcpy(parmFileName, "../data/cadx2.parm");

	CadxParm::getInstance().readFile(parmFileName);   

	short decimation = 2;

//	short noduleSize = 17; double noduleSigma = 3; // small
	short noduleSize = 32; double noduleSigma = 8; // medium


	IemTImage<short> imgNoduleTemplate = makeNoduleTemplate(noduleSize, noduleSigma);



	short ribSize = 17, ribThickness = 0, ribAmp = 0.0, nRibTemplates = 8;

	IemTImage<short>* pRibTemplates = new IemTImage<short>[nRibTemplates];
/*
	pRibTemplates[0] = makeHorRibTemplate(ribSize, ribThickness, ribAmp);
	"h:/schildkr/imgRibTemplate0.tif" << pRibTemplates[0];
	pRibTemplates[1] = makeVerRibTemplate(ribSize, ribThickness, ribAmp);
	"h:/schildkr/imgRibTemplate1.tif" << pRibTemplates[1];
	pRibTemplates[2] = makeDiagRibTemplate(ribSize, ribThickness, ribAmp, 0);
	"h:/schildkr/imgRibTemplate2.tif" << pRibTemplates[2]; 
	pRibTemplates[3] = makeDiagRibTemplate(ribSize, ribThickness, ribAmp, -1);
	"h:/schildkr/imgRibTemplate3.tif" << pRibTemplates[3]; 
*/


	pRibTemplates[0]= makeRibTemplate(ribSize, ribThickness, ribAmp, 0);
	"h:/schildkr/imgRibTemplateTheta0.tif" << pRibTemplates[0]; 
	pRibTemplates[1]= makeRibTemplate(ribSize, ribThickness, ribAmp, 90);
	"h:/schildkr/imgRibTemplateTheta90.tif" << pRibTemplates[1]; 
	pRibTemplates[2]= makeRibTemplate(ribSize, ribThickness, ribAmp, 45);
	"h:/schildkr/imgRibTemplateTheta45.tif" << pRibTemplates[2]; 
	pRibTemplates[3]= makeRibTemplate(ribSize, ribThickness, ribAmp, -45);
	"h:/schildkr/imgRibTemplateThetaN45.tif" << pRibTemplates[3]; 
	pRibTemplates[4]= makeRibTemplate(ribSize, ribThickness, ribAmp, 22);
	"h:/schildkr/imgRibTemplateTheta22.tif" << pRibTemplates[4]; 
	pRibTemplates[5]= makeRibTemplate(ribSize, ribThickness, ribAmp, -22);
	"h:/schildkr/imgRibTemplateThetaN22.tif" << pRibTemplates[5]; 
	pRibTemplates[6]= makeRibTemplate(ribSize, ribThickness, ribAmp, 70);
	"h:/schildkr/imgRibTemplateTheta70.tif" << pRibTemplates[6]; 
	pRibTemplates[7]= makeRibTemplate(ribSize, ribThickness, ribAmp, -70);
	"h:/schildkr/imgRibTemplateThetaN70.tif" << pRibTemplates[7]; 



/*		
	EdgeDetector edgeDetector(imgNoduleTemplate);  
	edgeDetector.setUpperHistPen(1.0);
	edgeDetector.calcSobelGradient();       
	IemTImage<short> imgTemplateEdgeMag = edgeDetector.getEdgeMag(); 
	"h:/schildkr/imgTemplateEdgeMag.tif"	<< imgTemplateEdgeMag;  
	IemTImage<short> imgTemplateEdgeDir = edgeDetector.getEdgeDir(); 
	"h:/schildkr/imgTemplateEdgeDir.tif"	<< imgTemplateEdgeDir;  
*/            

  
	IemTImage<short> imgTest = makeTestImage(); 
	IemTImage<unsigned char> imgMask = makeTestMask();

//	imgTest = ImageUtil::invert(imgTest, 0, 4095);
	"Q:/schildkr/cadx/targets/target1.tif" << imgTest;
	"Q:/schildkr/cadx/targets/target1_mask.tif" << imgMask;

	imgNoduleTemplate = ImageUtil::invert(imgNoduleTemplate, 0, 4095);
	"h:/schildkr/imgNoduleTemplate.tif" << imgNoduleTemplate;

//	for(long i = 0; i < nRibTemplates; i++) pRibTemplates[i] = ImageUtil::invert(pRibTemplates[i], 0, 4095);


	"h:/schildkr/imgRibTemplateTheta0.tif" << pRibTemplates[0]; 
	"h:/schildkr/imgRibTemplateTheta90.tif" << pRibTemplates[1]; 
	"h:/schildkr/imgRibTemplateTheta45.tif" << pRibTemplates[2]; 
	"h:/schildkr/imgRibTemplateThetaN45.tif" << pRibTemplates[3]; 
	"h:/schildkr/imgRibTemplateTheta22.tif" << pRibTemplates[4]; 
	"h:/schildkr/imgRibTemplateThetaN22.tif" << pRibTemplates[5]; 
	"h:/schildkr/imgRibTemplateTheta68.tif" << pRibTemplates[6]; 
	"h:/schildkr/imgRibTemplateThetaN68.tif" << pRibTemplates[7]; 

	cout << "\nRibs done" << endl;



//	IemTImage<short> imgTestAggregated = iemAggregateDown(imgTest, 2, 0);
//	"h:/schildkr/imgTestAggregated.tif" << imgTestAggregated;


//	IemTImage<short> imgTestUnAggregated = iemInterpolate(imgTestAggregated, imgTest.rows(), imgTest.cols());
//	IemTImage<short> imgTestUnAggregated = iemInterpolate(imgTestAggregated, imgTest.rows(), imgTest.cols());
//	"h:/schildkr/imgTestUnAggregated.tif" << imgTestUnAggregated;





	
	

//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0002_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0003_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0023_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0028_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0035_0_PA_P_CR400_CO_can0_nodule_probable_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0015_0_PA_P_CR400_CO_can4_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0015_0_PA_P_CR400_CO_can3_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/test0/roi/EK0015_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/setA/roi/EK0032_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";
//	imgTest << "Q:/schildkr/cadx/setA/roi/EK0003_0_PA_P_CR400_CO_can0_nodule_definite_scaled.tif";



//	ScalingFilter scalingFilter;
//	IemTImage<short> imgTestScaled = scalingFilter.globalScaling(imgTest);
//	"h:/schildkr/imgTestScaled.tif" << imgTestScaled;

//	imgTest = imgTestScaled;



//	BlurFilter blurFilter(7, 7);
//	IemTImage<short> imgTestBlurred = blurFilter.filter(imgTest);
//	"h:/schildkr/imgTestBlurred.tif" << imgTestBlurred;


//	imgTest =  imgTestBlurred;

/*
	IemTImage<short>  imgPadded = ImageUtil::pad(imgTest, 128, 128, 128, 128);
	"h:/schildkr/imgTestPadded.tif"	<< imgPadded;  


	CurvatureFilter curvatureFilter(15 );
	IemTImage<short> imgTestCurvature = curvatureFilter.filter(imgTest);
	"h:/schildkr/imgTestCurvature.tif" << imgTestCurvature;

*/






	MultiMorphFilter multiMorphFilter(pRibTemplates, nRibTemplates, decimation, 1.0);
	IemTImage<short> imgTestRibOpened = multiMorphFilter.filter(imgTest, 0, MorphFilter::OPEN);
//	IemTImage<short> imgTestRibOpened = multiMorphFilter.filterDeltaOpen(imgTestInverted);
	"h:/schildkr/imgTestRibOpened.tif" << imgTestRibOpened;	


	MorphFilter morphFilter(imgNoduleTemplate, decimation, 1.0);
	IemTImage<short> imgTestNoduleOpened = morphFilter.filter(imgTest, MorphFilter::OPEN);
	"h:/schildkr/imgTestNoduleOpened.tif" << imgTestNoduleOpened;


	IemTImage<short> imgTestOpenedDif = ImageUtil::subtract(imgTestNoduleOpened, imgTestRibOpened);
	"h:/schildkr/imgTestOpenedDif.tif" << imgTestOpenedDif;

//	IemTImage<unsigned char> imgTestOpenedDifSet = ImageUtil::imageToSetUpperHist(imgTestOpenedDif, upperPenetration);
//	IemTImage<unsigned char> imgTestOpenedDifSet = ImageUtil::imageToSetUpperHist(imgTestNoduleOpened, upperPenetration);
//	"h:/schildkr/imgTestOpenedDifSet.tif" << imgTestOpenedDifSet;

 




	return 0;
}




IemTImage<short> makeNoduleTemplate(short size, double sigma) {
	
                         
	IemTImage<short> imgTemplate(1, size, size);  

	
	long c0 = imgTemplate.cols() / 2;
	long r0 = imgTemplate.rows() / 2;

	for(int c = 0; c < imgTemplate.cols(); ++c) {
		for(int r = 0; r < imgTemplate.rows(); ++r) {


			double d2 = 0.5 * (double)((c - c0) * (c - c0) + (r - r0) * (r - r0))
					/ (sigma * sigma);


			imgTemplate[0][r][c] = (short) maxCodeValue * (1.0 - exp(-d2));   
	}}

	return imgTemplate;
}


IemTImage<short> makeHorRibTemplate(short size, short t, short amp) {
	
                         
	IemTImage<short> imgTemplate(1, size, size);  
	imgTemplate = 0;
//	imgTemplate = maxCodeValue;

	
	long c0 = imgTemplate.cols() / 2;
	long r0 = imgTemplate.rows() / 2;

	for(long c = 0; c < imgTemplate.cols(); c++) {
		for(long r = r0 - t; r <= r0 + t; r++) {


			imgTemplate[0][r][c] = 4095;
	}}

	return imgTemplate;
}


IemTImage<short> makeVerRibTemplate(short size, short t, short amp) {
	
                         
	IemTImage<short> imgTemplate(1, size, size);
	imgTemplate = 0;
//	imgTemplate = maxCodeValue;

	
	long c0 = imgTemplate.cols() / 2;
	long r0 = imgTemplate.rows() / 2;

	for(long r = 0; r < imgTemplate.rows(); r++) {
		for(long c = c0 - t; c <= c0 + t; c++) {


			imgTemplate[0][r][c] = 4095;
	}}

	return imgTemplate;
}


IemTImage<short> makeDiagRibTemplate(short size, short t, short amp, short slant) {

                         
	IemTImage<short> imgTemplate(1, size, size);  
	imgTemplate = maxCodeValue;

	
	long c0 = imgTemplate.cols() / 2;
	long r0 = imgTemplate.rows() / 2;
	long rr;


	for(long r = 0; r < imgTemplate.rows(); r++) {
		for(long c = 0; c < imgTemplate.cols(); c++) {

			if(slant == -1) rr = r;
			else rr = size - r;

			if(abs(c - rr) < t) imgTemplate[0][r][c] *= amp;
	}}


	return imgTemplate;
}


IemTImage<short> makeRibTemplate(short size, short t, short amp, double theta) {

	IemTImage<short> imgTemplate(1, size, size);  
	imgTemplate = 0;
//	imgTemplate = maxCodeValue;

	if(theta == 90) return makeVerRibTemplate(size, t, amp);
	else if(theta == 0) return makeHorRibTemplate(size, t, amp);

	double rr;

	theta *= Util::PI / 180.0;

	double c0 = (double)imgTemplate.cols() / 2.0;
	double r0 = (double)imgTemplate.rows() / 2.0;

	double g = (double)t / cos(theta);

/*

	for(long r = 0; r < imgTemplate.rows(); r++) {
		for(long c = 0; c < imgTemplate.cols(); c++) {


		//	rr = -tan(theta) * ((double)c - c0) + r0;
			rr = -tan(theta) * ((double)c - c0) + r0;

			rr = (int)(rr + 0.5);
		

			if(rr == r) imgTemplate[0][r][c] *= amp;
	}}
*/

	cout << "\ntheta=" << theta; 



	for(double c = 0; c <= c0; c += 0.1) {


		//	rr = -tan(theta) * ((double)c - c0) + r0;
			long r = -tan(theta) * (double)c;

			long c1 = c0 - c;
			long c2 = c0 + c;
			long r1 = r0 + r;
			long r2 = r0 - r;

			if(!(c1 < 0 || r1 < 0 || c1 >= imgTemplate.cols() || r1 >= imgTemplate.rows())) imgTemplate[0][r1][c1] = 4095;
			if(!(c2 < 0 || r2 < 0 || c2 >= imgTemplate.cols() || r2 >= imgTemplate.rows())) imgTemplate[0][r2][c2] = 4095;


		//	cout << "\ncol=" << c << ", row=" << r << ", c1=" << c1 << ", r1=" << r1 << flush;

	}

	cout << endl << endl;


	return imgTemplate;
}



IemTImage<short> addNodule(IemTImage<short>& imgTest, double sigma, 
 long c0, long r0, double amplitude, double tranparency) {


	for(long r = 0; r < imgTest.rows(); r++) {
		for(long c = 0; c < imgTest.cols(); c++) {	
			
			double d2 = 0.5 * (double)((c - c0) * (c - c0) + (r - r0) * (r - r0)) / (sigma * sigma);

			double	nodule = amplitude * exp(-d2);   
			
			imgTest[0][r][c] = tranparency * imgTest[0][r][c] 
				+ (1.0 - tranparency) * (imgTest[0][r][c] - nodule);						
	}}
	
	return imgTest;
}


IemTImage<short> addRib(IemTImage<short>& imgTest, short c0, short r0, 
  double amplitude, double tranparency) {

	long rr;
	double t = 30;


	for(double theta = -60; theta < 60; theta += 30) {


		double thetaR = Util::PI / 180.0 * theta;

		for(long r = 0; r < imgTest.rows(); r++) {
			for(long c = 0; c < imgTest.cols(); c++) {


				rr = -tan(thetaR) * (c - c0) + r0;

				if(abs(rr - r) < t / cos(thetaR)) imgTest[0][r][c] = tranparency * imgTest[0][r][c] 
					+ (1.0 - tranparency) * (imgTest[0][r][c] - amplitude);
		}}

	}
	
	return imgTest;
}


IemTImage<short> addHeart(IemTImage<short>& imgTest, long c1, long c2,
  double amplitude, double tranparency) {

	for(long r = 0; r < imgTest.rows(); r++) {
		for(long c = c1; c < c2; c++) {	
			
			imgTest[0][r][c] = tranparency * imgTest[0][r][c] 
				+ (1.0 - tranparency) * (imgTest[0][r][c] - amplitude);				
	}}	
	
	return imgTest;
}



IemTImage<short> makeTestImage() {


	IemTImage<short> imgTest(1, 2500, 2048); 
	imgTest = 1800;

	Kespr kespr;
//	img = kespr.Read("J:/image/CADX/test0/Images/EK0003_0_PA_P_CR400_CO.img");  


	long c1 = 0;
	long c2 = imgTest.cols() / 4;
	imgTest = addHeart(imgTest, c1, c2, 500, 0.0);


	short cRib = imgTest.cols() / 3;
	short rRib = imgTest.rows() / 3;
	imgTest = addRib(imgTest, cRib, rRib, 100, 0.0);


	long r0 = imgTest.rows() / 2;
	long c0 = imgTest.cols() / 2;	
	double sigma = 32;
	imgTest = addNodule(imgTest, sigma, c0, r0, 100, 0.0);

	imgTest = addNodule(imgTest, sigma, 890, 2090, 100, 0.0);

	imgTest = addNodule(imgTest, sigma, 530, 1335, 100, 0.0);
	

	return imgTest;
}

IemTImage<unsigned char> makeTestMask() {

	IemTImage<unsigned char> imgMask(1, 2500, 2048);
	imgMask = 255;

	long c1 = 0;
	long c2 = imgMask.cols() / 4;

	for(long r = 0; r < imgMask.rows(); r++) {
		for(long c = c1; c < c2; c++) {	
			
			imgMask[0][r][c] = 0;				
	}}	
	
	return imgMask;
}





