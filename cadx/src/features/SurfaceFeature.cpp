//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "SurfaceFeature.h"
#include "Util.h"   
#include "CadxParm.h" 
#include "ConvexHull.h"


using namespace CADX_SEG;


SurfaceFeature::SurfaceFeature()
 : Feature() {
 	

}     

SurfaceFeature::SurfaceFeature(char* _labelRoot)
 : Feature(4, _labelRoot) {
 	
 	initialize(); 
}

SurfaceFeature::SurfaceFeature(char* _labelRoot, Roi& roi)
 : Feature(4, _labelRoot) {
 	
 	initialize(); 
 	 	
	imgMap = roi.getNoduleMap(); 
	
	colCenter = roi.getCandidateCol();
	rowCenter = roi.getCandidateRow();
}    


SurfaceFeature::SurfaceFeature(char* _labelRoot, IemTImage<short>& _img,
 IemTImage<unsigned char>& _imgMap, long _colCenter, long _rowCenter) 
 : Feature(4, _labelRoot) {

 	initialize();  
 	   
 	img = _img;
	imgMap = _imgMap; 
	
	colCenter = _colCenter;
	rowCenter = _rowCenter;
}  


double SurfaceFeature::getScore() {
	return 0.0;   
} 

/*
void SurfaceFeature::calculate() {
	
	PolyFit polyFit(4);
	polyFit.calcFit(img, imgMap, colCenter, rowCenter, 1);
	value[0] = polyFit.getFitError();
	 
	imgFit = polyFit.getMaskedFitImage();

	double a = polyFit.getPolynomial().getCoef(4);
	double b = polyFit.getPolynomial().getCoef(3);
	double c = polyFit.getPolynomial().getCoef(5);
	double d = polyFit.getPolynomial().getCoef(1);
	double e = polyFit.getPolynomial().getCoef(2);
	
	double g = d*d + e*e;
	
	if(g == 0.0) {
		value[1] = 0.0;
		return;
	}
	
	double curvature = 2.0 * (a*e*e - b*e*d + c*d*d) / pow(g, 1.5);

	value[1] = curvature;
}
*/

void SurfaceFeature::calculate() {
	
//	PolyFit polyFit(4);
	polyFit.initialize(4);
	polyFit.calcFit(img, imgMap, colCenter, rowCenter, 1);
	value[0] = polyFit.getFitError();
	 
	imgFit = polyFit.getMaskedFitImage();
	

	
	polyFit.getPolynomial().setPosition(colCenter, rowCenter);

	double dxx = polyFit.getPolynomial().getdXX(); 
	double dyy = polyFit.getPolynomial().getdYY();
	double dxy = polyFit.getPolynomial().getdXY();


	double b = -(dxx + dyy);
	double c = (dxx * dyy - dxy * dxy);

	double e = b * b - 4.0 * c;
	
	if(e < 0.0) {
		value[1] = 0.0;
		value[2] = 0.0;
		value[3] = 0.0;
		return;
	}

	lamda1 = (-b + sqrt(e)) / 2.0;
	lamda2 = (-b - sqrt(e)) / 2.0;
	lamda12 = lamda1 * lamda2;
	

	
//	regionStatistics();

	value[1] = lamda1; 
	value[2] = lamda2;
	value[3] = lamda12;
}


void SurfaceFeature::regionStatistics() {

	long area = 0; 
	
	double lamda1Sum = 0.0;
	double lamda2Sum = 0.0;
	double lamda12Sum = 0.0;

	for(long r = 0; r < img.rows(); r++) {
		for(long c = 0; c < img.cols(); c++) {
			if(imgMap[0][r][c] == Util::NODULE_LABEL) {

				polyFit.getPolynomial().setPosition(c, r);

				double dxx = polyFit.getPolynomial().getdXX();
				double dyy = polyFit.getPolynomial().getdYY();
				double dxy = polyFit.getPolynomial().getdXY();


				double b = -(dxx + dyy);
				double c = (dxx * dyy - dxy * dxy);

				double e = b * b - 4.0 * c;
	
				if(e < 0.0) {
					continue;
				}
				
				area++;

				lamda1 = (-b + sqrt(e)) / 2.0;
				lamda2 = (-b - sqrt(e)) / 2.0;
				lamda12 = lamda1 * lamda2;
				
				lamda1Sum += lamda1;
				lamda2Sum += lamda2;
				lamda12Sum += lamda12;
			}
		}
   		

	}

	if(area == 0) return;
	
	lamda1 = lamda1Sum / (double)area;
	lamda2 = lamda2Sum / (double)area;
	lamda12 = lamda12Sum / (double)area;
}
 

void SurfaceFeature::initialize() {

	colCenter = 0;  
	rowCenter = 0;

	lamda1 = 0.0;
	lamda2 = 0.0;
	lamda12 = 0.0;

	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Surface");

	sprintf(label[0], "%s.fitError", labelRoot);
	sprintf(label[1], "%s.lamda1", labelRoot);
	sprintf(label[2], "%s.lamda2", labelRoot);
	sprintf(label[3], "%s.lamda12", labelRoot);
}


