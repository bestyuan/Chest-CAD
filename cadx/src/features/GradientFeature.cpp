//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "GradientFeature.h" 
#include "Gradient.h"
#include "Util.h"   
#include "CadxParm.h" 
#include "Iem/IemImageIO.h" 


using namespace CADX_SEG; 


GradientFeature::GradientFeature() 
 : Feature() {      
 	

}  
 
GradientFeature::GradientFeature(char* _labelRoot)
 : Feature(3, _labelRoot) {

	initialize();
}


GradientFeature::GradientFeature(char* _labelRoot, Roi& roi, double _originCol, double _originRow)
 : Feature(3, _labelRoot) {

 	initialize();

	img = roi.getCroppedImage();
	imgMap = roi.getNoduleMap();

	originCol = _originCol;
	originRow = _originRow;

	init();
	
	calcGradient();
}    


GradientFeature::GradientFeature(char* _labelRoot, IemTImage<short>& _img, IemTImage<unsigned char>& _imgMap,
  double _originCol, double _originRow) : Feature(3, _labelRoot) {     
 	
 	initialize();  
 	
 	img = _img; 		
	imgMap = _imgMap; 
	
	originCol = _originCol;	
	originRow = _originRow;	
	
	init();
	
	calcGradient();
}  


GradientFeature::GradientFeature(char* _labelRoot, IemTImage<short>& _img, IemTImage<unsigned char>& _imgMap) 
 : Feature(3, _labelRoot) {     
 	
 	initialize();  
 	
 	img = _img; 		
	imgMap = _imgMap; 
	
	init();
	
	findBestOrigin();
	
	calcGradient();
}  


void GradientFeature::init() {
	imgCosine = IemTImage<unsigned char>(3, img.rows(), img.rows());
	imgCosine = 0;
}


void GradientFeature::calcGradient() {

	Gradient gradient;
	
	imgEdge = gradient.calcSobelGradient(img);
}


double GradientFeature::getScore() {
	
	return value[0] / value[1];		
} 


void GradientFeature::calculate() {     
 	
	regionStatistics();
	
	if(userAgent.getDebug()) {
		markCosineImgOrigin((long)originCol, (long)originRow);
	}
	
	value[0] = dirCoherence;  
	value[1] = nonuniformity;
	value[2] = dirCoherence / nonuniformity;
}    


void GradientFeature::initialize() {      
	
	dirCoherence = -1;
	nonuniformity = 1;

	minGradientMag = 40;
	maxGradientMag = 200;

//	minGradientMag = 4;
//	maxGradientMag = 400;
	
	originCol = 0;
	originRow = 0;
	
	regionLabel = Util::NODULE_LABEL;    
	
	for(long i = 0; i < 8; i++) {
		sectorCount[i] = 0; 
		sectorCosine[i] = 0;
		sectorHist[i] = 0;
	}

	if(strcmp(labelRoot, "") == 0) strcpy(labelRoot, "Gradient");
	
	sprintf(label[0], "%s.dirCoherence", labelRoot);
	sprintf(label[1], "%s.nonuniformity", labelRoot);
	sprintf(label[2], "%s.quality", labelRoot);
}


void GradientFeature::regionStatistics() {
	
	double sum = 0.0;   
	long area = 0;
	
	double q = 180.0 / Util::PI;
	double p = Util::PI / 180.0;
	
	userAgent.getLogFile() << "\n" << "GradientFeature::regionStatistics(): "
	 << "\norigin " << originCol << setw(12) << originRow;
				
	for(long r = 0; r < img.rows(); r++) {						                                        
		for(long c = 0; c < img.cols(); c++) {   
			
			if(imgMap[0][r][c] == regionLabel) {
			
				short mag = imgEdge[0][r][c];
			
				if(mag < minGradientMag || mag > maxGradientMag) continue;
				
				area++;
				
				double dx = (originCol - c);
				double dy = -(originRow - r);
				
				double theta0 =  q * atan2(dy, dx);
				
				if(theta0 < 0.0) theta0 += 360.0;  
				
				short theta = imgEdge[1][r][c];
				
				double delta = Util::abs(theta - theta0);
			
				double cosine = cos(p * delta);
				
				sum += cosine;
				
				long sectorNum;
				
				if(theta0 >= 180.0 && theta0 < 225.0) {sectorNum = 0;}
				else if(theta0 >= 225.0 && theta0 < 270.0) {sectorNum = 1;}
				else if(theta0 >= 270.0 && theta0 < 315.0) {sectorNum = 2;}
				else if(theta0 >= 315.0 && theta0 < 360.0) {sectorNum = 3;}
				else if(theta0 >= 0.0 && theta0 < 45.0) {sectorNum = 4;}
				else if(theta0 >= 45.0 && theta0 < 90.0) {sectorNum = 5;}
				else if(theta0 >= 90.0 && theta0 < 135.0) {sectorNum = 6;}
				else if(theta0 >= 135.0 && theta0 < 180.0) {sectorNum = 7;}
				
				sectorCount[sectorNum]++;
				sectorCosine[sectorNum] += cosine;
				
				if(userAgent.getDebug()) {
					markCosineImg(c, r, sectorNum, cosine);
				}

			} // region pixel

	}}
	
	//"h:/schildkr/gradient.tif" << imgTest;


	if(area == 0) return;
	
	userAgent.getLogFile() << "\nsectors";
	
	for(long i = 0; i < 8; i++) {
	
		sectorHist[i] = ((double)sectorCount[i]) / area;
		sectorCosine[i] /= (double)area;
						
		userAgent.getLogFile() << "\n" << setw(5) << i << setw(15) << sectorHist[i]
		 << setw(15) << sectorCosine[i];
	}
	
	dirCoherence = (double)sum / (double)area;
	
	calcNonuniformity();
}


void GradientFeature::calcNonuniformity() {   

//	double mean = dirCoherence / 8.0;
	double mean = 1.0 / 8.0;
	nonuniformity = 0.0;
	
	for(long i = 0; i < 8; i++) {
	
		nonuniformity += Util::square(sectorCosine[i] - mean);
	}
	
	nonuniformity = sqrt(nonuniformity / 8.0);
}


void GradientFeature::findBestOrigin() {

	short max = SHRT_MIN;
	long sumCol = 0, sumRow = 0, area = 0;
	long maxCol = 0, maxRow = 0, centerCol = 0, centerRow = 0;

	for(long r = 0; r < img.rows(); r++) {	                            
		for(long c = 0; c < img.cols(); c++) {

			if(imgMap[0][r][c] == regionLabel) {
			                                      	
				short v = img[0][r][c];
			       
				area++;
				sumCol += c;
				sumRow += r;
			
				if(v > max) {
					max = v;
					maxCol = c;
					maxRow = r;
				}
			}  
	}} 
	
	if(area == 0) return;

	centerCol = sumCol / area;
	centerRow = sumRow / area;
	
//	originCol = centerCol;
//	originRow = centerCol;

	originCol = maxCol;
	originRow = maxRow;
}

void GradientFeature::markCosineImgOrigin(long col, long row) {

	for(long i = -1; i <= 1; i++) {
		for(long k = -1; k <= 1; k++) {

			long c = col + k;
			long r = row + i;
			
			if(r < 0 || r >= imgCosine.rows() || c < 0 || c >= imgCosine.cols()) continue;

 			imgCosine[0][r][c] = 255;
			imgCosine[1][r][c] = 255;
			imgCosine[2][r][c] = 255;
	}}

}
 
void GradientFeature::markCosineImg(long c, long r, long sectorNum, double cosine) {

	if(cosine >= 0.0) {
		imgCosine[1][r][c] = 255 * cosine;
	}
	else {
		imgCosine[0][r][c] = -255 * cosine;
	}

	return; 

	long cosineView = (long)255 * cosine;
				
	if(sectorNum == 0) {
		imgCosine[0][r][c] = cosineView;
	}
	else if(sectorNum == 1) {
		imgCosine[1][r][c] = cosineView;
	}
	else if(sectorNum == 2) {
		imgCosine[2][r][c] = cosineView;
	}
	else if(sectorNum == 3) {
		imgCosine[0][r][c] = 0.50 * cosineView;
		imgCosine[1][r][c] = 0.50 * cosineView;
	}
	else if(sectorNum == 4) {
		imgCosine[0][r][c] = 0.50 * cosineView;
		imgCosine[2][r][c] = 0.50 * cosineView;
	}
	else if(sectorNum == 5) {
		imgCosine[1][r][c] = 0.50 * cosineView;
		imgCosine[2][r][c] = 0.50 * cosineView;
	}
	else if(sectorNum == 6) {
		imgCosine[0][r][c] = 0.25 * cosineView;
		imgCosine[1][r][c] = 0.25 * cosineView;
		imgCosine[2][r][c] = 0.50 * cosineView;
	}
	else if(sectorNum == 7) {
		imgCosine[0][r][c] = 0.25 * cosineView;
		imgCosine[1][r][c] = 0.50 * cosineView;
		imgCosine[2][r][c] = 0.25 * cosineView;
	}
				
				
	//	CadxParm::getInstance().getLogFile() << "\n" << setw(7) << c << setw(7) << r
	//	  << setw(12) << theta0 << setw(12) << theta << setw(12) << cosine;
}



