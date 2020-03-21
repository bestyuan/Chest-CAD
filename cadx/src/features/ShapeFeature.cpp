//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "ShapeFeature.h"   
#include "Util.h"   
#include "CadxParm.h" 
#include "ConvexHull.h" 


using namespace CADX_SEG;    
 

ShapeFeature::ShapeFeature() 
 : Feature() {
 	

}     

ShapeFeature::ShapeFeature(char* _labelRoot)
 : Feature(4, _labelRoot) {
 	
 	initialize(); 
}

ShapeFeature::ShapeFeature(char* _labelRoot, Roi& roi)
 : Feature(4, _labelRoot) {
 	
 	initialize(); 
 	 	
	imgMap = roi.getNoduleMap(); 
	
	colAim = roi.getCandidateCol();
	rowAim = roi.getCandidateRow();
}    


ShapeFeature::ShapeFeature(char* _labelRoot, IemTImage<unsigned char>& _imgMap) 
 : Feature(4, _labelRoot) {
 	
 	initialize();  
 	
	imgMap = _imgMap; 
	
	colAim = imgMap.cols() / 2;
	rowAim = imgMap.rows() / 2;
}  


double ShapeFeature::getScore() {
		
	return value[2] * value[3] * value[4];		
} 


void ShapeFeature::calculate() {     
 	
	regionStatistics();

	if(area == 0) {

		imgConvexHull = imgMap.copy();

		value[0] = 0;	
		value[1] = 0;
		value[2] = 0; 
		value[3] = 0;

		return;
	}
	
	calculateShape(); 
	
	calculateConvexHull();
	
	value[0] = (double)area / (double)imageArea;	
	value[1] = aspectRatio;
	value[2] = position;
	value[3] = (double)area / (double)convexHullArea; 
}    


void ShapeFeature::initialize() {      
	
	area = 0;	  
	imageArea = 0;
	convexHullArea = 0;
	colCenter = 0;
	rowCenter = 0;
	momentRR = 0;
	momentCC = 0;
	momentRC = 0;  
	fit = 0; 
	aspectRatio = 0;
	regionLabel = Util::NODULE_LABEL;  
	position = 0.0;
	colAim = 0;
	rowAim = 0;
	convexHullArea = 0;
	
	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Shape");
	
	sprintf(label[0], "%s.areaFraction", labelRoot);
	sprintf(label[1], "%s.aspectRatio", labelRoot);
	sprintf(label[2], "%s.position", labelRoot);
	sprintf(label[3], "%s.areaToHullRatio", labelRoot);
}


void ShapeFeature::regionStatistics() {  
	
	double colSum = 0, rowSum = 0, col2Sum = 0, row2Sum = 0, colRowSum = 0; 
	
	area = 0;      
	
	imageArea = imgMap.rows() *  imgMap.cols();
	
	
	for(long r = 0; r < imgMap.rows(); r++) {	
		
		long lineColSum = 0, lineRowSum = 0, lineCol2Sum = 0, lineRow2Sum = 0, lineColRowSum = 0;  
					                                        
		for(long c = 0; c < imgMap.cols(); c++) {   
			
			if(imgMap[0][r][c] == regionLabel) {   
				
				area++;
				
				lineColSum += c;
   				lineRowSum += r;       				
   				lineCol2Sum += c * c;
   				lineRow2Sum += r * r;
   				lineColRowSum += c * r;	
			}  
		}

		colSum += (double)lineColSum;
   		rowSum += (double)lineRowSum;       				
   		col2Sum += (double)lineCol2Sum;
   		row2Sum += (double)lineRow2Sum;
   		colRowSum += (double)lineColRowSum;
	}     


	if(area == 0) return;
	
	colCenter = colSum / (double)area;
	rowCenter = rowSum / (double)area;
 
   	double col2Center = col2Sum / (double)area;
   	double row2Center = row2Sum / (double)area;
	double colRowCenter = colRowSum / (double)area;

	momentRR = row2Center - rowCenter * rowCenter;
	momentCC = col2Center - colCenter * colCenter;
	momentRC = colRowCenter - colCenter * rowCenter;   
}


void ShapeFeature::calculateShape() {     
	
	if(area == 0) return;
	
	double temp = sqrt(Util::square(momentCC - momentRR) + 4.0 * Util::square(momentRC));

	double C1 = momentCC + momentRR + temp;
	double C2 = momentCC + momentRR - temp;

	if(C2 == 0) aspectRatio = 0.0;
	else aspectRatio = sqrt(C1 / C2);

	double u = momentRR * momentCC - Util::square(momentRC);
	temp = 4.0 * u;

	if(temp == 0.0){fit = 0.0; return;}

	double d = momentCC / temp;
	double e = -momentRC / temp;
	double f = momentRR / temp;

	double ellipseArea = 4.0 * Util::PI * sqrt(u);
/*
	long inCount = 0, outCount = 0; 

	for(long r = 0; r < imgMap.rows(); r++) {		                                        
		for(long c = 0; c < imgMap.cols(); c++) { 
			
			if(imgMap[0][r][c] == regionLabel) {   
		
     			double cc = c - colCenter;  
     			double rr = r - rowCenter;

     			double r = d * Util::square(rr) + 2.0 * e * cc * rr + f * Util::square(cc);

				if(r <= 1.0) inCount++; 
				else outCount++;      
			}
	}}      

	double inPixels = Util::min(inCount, ellipseArea);

	if(ellipseArea != 0) fit = 0.5 * (2.0 - (double)outCount / 
	 (double)area - (ellipseArea - inPixels) / ellipseArea);
	else fit = 0.0;
*/
	double d1 = sqrt(Util::square(colCenter - colAim) + Util::square(rowCenter - rowAim));
//	double d2 = sqrt(Util::square(imgMap.cols()) + Util::square(imgMap.rows()));
	
	double d2 = sqrt(area / Util::PI);
	
	position = 1.0 - d1 / d2;
}


void ShapeFeature::calculateConvexHull() {

	ConvexHull convexHull(40);
	
	imgConvexHull = convexHull.calculate(imgMap);	
	
	convexHullArea = convexHull.getArea();

//	CadxParm::getInstance().getLogFile() << "\n\n" << convexHull << endl;
}





