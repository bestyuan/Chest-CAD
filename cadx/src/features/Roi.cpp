//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//	
	
		
	
#include "Roi.h" 
#include "Util.h"  
#include "ImageUtil.h"
#include "Boundary.h"
#include "Histogram.h" 
#include "ImageReader.h" 
#include "Iem/IemCrop.h"  
#include "Iem/IemPad.h" 
#include "Iem/IemImageIO.h"  


using namespace CADX_SEG;


Roi::Roi() {
//	imgBilateral = IemTImage<short>(1, 128, 128);
//	bigimgBilateral=IemTImage<short>(1,512,512);
//	smallROI=IemTImage<short>(1,128,128);

	initialize();
}

	
Roi::Roi(char* _name, IemTImage<short>& _img, IemTImage<unsigned char>& _imgAnatomicalMap,
 long candidateCol, long candidateRow, long _groundtruth) {

	initialize();
	strcpy(name, _name);
	setLocation(candidateCol, candidateRow);
	cropImage(_img, _imgAnatomicalMap);
	groundtruth = _groundtruth;
	
	imgNoduleMap = IemTImage<unsigned char>(1, height, width);
	imgNoduleMap = 0;
}


void Roi::free() {

}


void Roi::initialize() {
	strcpy(name, "");
	strcpy(source, ""); 
	valid = false;
	index = 0;
	width = 256;
	height = 256;    
	area = 0;
	originCol = 0;	
	originRow = 0;	 
	relCandidateCol = 0;	
	relCandidateRow = 0;
	probability = 0.0;
	groundtruth = Util::GROUNDTRUTH_UNKNOWN;
	overgrow = 1.0; 
	undergrow = 1.0;
	nGroups = 0;
	minCodeValue = 0;
	maxCodeValue = 4095;
	nScale = 0;
}


char* Roi::getName() {
	sprintf(name, "%s_roi%d_scale%d_%s", source, index, nScale, Util::getGtDescription(groundtruth));
	return name;
}


void Roi::mapToBoundary() {
	boundary.mapToBoundary(imgNoduleMap);
}

void Roi::mapToBoundaryribsup() {
	boundaryribsup.mapToBoundary(imgNoduleMapribsup);
}


void Roi::cropImage(IemTImage<short>& _img, IemTImage<unsigned char>& _imgAnatomicalMap) {

	imgCropped = IemTImage<short>(1, height, width);
	imgAnatomicalMap = IemTImage<unsigned char>(1, height, width);

	long cols = _img.cols();
	long rows = _img.rows();

	for(long y = 0; y < height; y++) {
		for(long x = 0; x < width; x++) {

			long xx = x + originCol;
			long yy = y + originRow;
			
			bool outsideImage = false;
			
			if(xx < 0) {xx = 0; outsideImage = true;}
			if(yy < 0) {yy = 0; outsideImage = true;}
			if(xx >= cols) {xx = cols - 1; outsideImage = true;}
			if(yy >= rows) {yy = rows - 1; outsideImage = true;}

			short v = _img[0][yy][xx];

			imgCropped[0][y][x] = v;

			if(!outsideImage) {imgAnatomicalMap[0][y][x] = _imgAnatomicalMap[0][yy][xx];}
			else {imgAnatomicalMap[0][y][x] = 0;}
	}}
}

void Roi::cropImageribsup(IemTImage<short>& _imgribsup, IemTImage<unsigned char>& _imgAnatomicalMapribsup) {

	imgCroppedribsup = IemTImage<short>(1, height, width);
	imgAnatomicalMapribsup = IemTImage<unsigned char>(1, height, width);

	long cols = _imgribsup.cols();
	long rows = _imgribsup.rows();

	for(long y = 0; y < height; y++) {
		for(long x = 0; x < width; x++) {

			long xx = x + originCol;
			long yy = y + originRow;
			
			bool outsideImage = false;
			
			if(xx < 0) {xx = 0; outsideImage = true;}
			if(yy < 0) {yy = 0; outsideImage = true;}
			if(xx >= cols) {xx = cols - 1; outsideImage = true;}
			if(yy >= rows) {yy = rows - 1; outsideImage = true;}

			short v = _imgribsup[0][yy][xx];

			imgCroppedribsup[0][y][x] = v;

			if(!outsideImage) {imgAnatomicalMapribsup[0][y][x] = _imgAnatomicalMapribsup[0][yy][xx];}
			else {imgAnatomicalMapribsup[0][y][x] = 0;}
	}}
}


IemTImage<short> Roi::cropRoiFromImage(IemTImage<short>& _img) {

	IemTImage<short> imgOut = IemTImage<short>(1, height, width);

	long cols = _img.cols();
	long rows = _img.rows();

	for(long y = 0; y < height; y++) {
		for(long x = 0; x < width; x++) {

			long xx = x + originCol;
			long yy = y + originRow;

			if(xx < 0) {xx = 0;}
			if(yy < 0) {yy = 0;}
			if(xx >= cols) {xx = cols - 1;}
			if(yy >= rows) {yy = rows - 1;}

			short v = _img[0][yy][xx];

			imgOut[0][y][x] = v;
	}}
	
	return imgOut;
}


void Roi::setLocation(long candidateCol, long candidateRow) {   
 	
	originCol = candidateCol - width / 2;	
	originRow = candidateRow - height / 2;	    

	relCandidateCol = width / 2;
	relCandidateRow = height / 2;	
}          


bool Roi::isInsideAbsBoundary(long _col, long _row) {
	long col = getRelCol(_col);
	long row = getRelRow(_row);
	return boundary.isInsideBoundary(col, row);
}


bool Roi::isInsideBoundary(long col, long row) {
	return boundary.isInsideBoundary(col, row);
}


IemTImage<unsigned char> Roi::getAnnotated(long* outlineCol, long* outlineRow, long nOutlinePoints) {

	IemTImage<unsigned char> imgAnnotated = getAnnotated();

	// Mark the boundary points.
	for(long k = 0; k < nOutlinePoints; k++) {     

		long bCol = getRelCol(outlineCol[k]);
		long bRow = getRelRow(outlineRow[k]);

		if(bCol < 0 || bCol >= imgAnnotated.cols()
		 || bRow < 0 || bRow >= imgAnnotated.rows()) continue;

		imgAnnotated[0][bRow][bCol] = 255;   
		imgAnnotated[1][bRow][bCol] = 0;
		imgAnnotated[2][bRow][bCol] = 0;
	}

	return imgAnnotated;
}


double Roi::getGroundtruthFit() {
	return 0.5 * (2.0 - overgrow - undergrow);
} 


IemTImage<unsigned char> Roi::getAnnotated() {   
	
	long d = 1, c0, r0, c, r, v;

	IemTImage<unsigned char> imgAnnotated(3, imgCropped.rows(), imgCropped.cols());
	
	Histogram histogram(minCodeValue, maxCodeValue);
	histogram.build(imgCropped);

	double lower = histogram.getLowerPenetration(0.02);
	double upper = histogram.getUpperPenetration(0.02);
	
	// Scale and invert image.

	for(r = 0; r < imgCropped.rows(); r++) {		                                        
		for(c = 0; c < imgCropped.cols(); c++) {
			
			double cv = (double)imgCropped[0][r][c];
			
			cv = (cv - lower) / (upper - lower);

			if(cv < 0) cv = 0.0;
			else if(cv > 1.0) cv = 1.0;
			
			cv = 255.0 * cv;
		//	cv = 255.0 * (1.0 - cv);

			if(imgAnatomicalMap[0][r][c] == 0) cv *= 0.7;
			
			imgAnnotated[0][r][c] = cv;
			imgAnnotated[1][r][c] = cv;	
			imgAnnotated[2][r][c] = cv;		
	}}
	
	ImageUtil::makeMark(imgAnnotated, relCandidateCol, relCandidateRow, 0, 255, 255, 3);

	// Mark the boundary points.
	list<Point>::iterator iter;

	for(iter = boundary.getList().begin(); iter != boundary.getList().end(); iter++) {
		long bCol = (*iter).x;
		long bRow = (*iter).y;

		ImageUtil::makeMark(imgAnnotated, bCol, bRow, 255, 255, 0, 2);
	}

	return imgAnnotated;
}


void Roi::calculateArea() {
	
	area = 0;
	
	for(long c = 0; c < width; c++) {
		for(long r = 0; r < height; r++) {
			if(imgNoduleMap[0][r][c] == Util::NODULE_LABEL) area++;
	}} 
}	

void Roi::calculateArearibsup() {
	
	area = 0;
	
	for(long c = 0; c < width; c++) {
		for(long r = 0; r < height; r++) {
			if(imgNoduleMapribsup[0][r][c] == Util::NODULE_LABEL) area++;
	}} 
}

double Roi::compareRegionToGroundtruth(Hit& hit) {   
	
	long col = getAbsCandidateCol();
	long row = getAbsCandidateRow();
	
	long noduleIndex = hit.getEnclosingNodule(col, row);
	             
	// Return -1 if the Roi is not within a nodule.	             
	if(noduleIndex == -1) return -1;
	
	long inCount = 0, boundaryArea = 0;	
	
	for(long c = 0; c < width; c++) {
		for(long r = 0; r < height; r++) {

			if(isInsideBoundary(c, r)) {
			
				boundaryArea++;

				long col = getAbsCol(c);
				long row = getAbsRow(r);
				
				if(hit.isInsideOutline(noduleIndex, col, row)) inCount++;	
			}
	}}      
	
	long noduleArea = (double)hit.getNoduleArea(noduleIndex);	  
	
	long outCount = boundaryArea - inCount;
		
	if(boundaryArea > 0) overgrow = (double)outCount / (double)boundaryArea;
	else overgrow = 0.0;

	undergrow = (double)(noduleArea - inCount) / (double)noduleArea;     

	return getGroundtruthFit();
}
		

Roi& Roi::operator=(const Roi& rhs) {  

	free();
	
	long k;
	
	if(rhs.imgCropped.isValid()) imgCropped = rhs.imgCropped.copy();   
	if(rhs.imgCropped.isValid()) imgAnatomicalMap = rhs.imgAnatomicalMap.copy();
	if(rhs.imgNoduleMap.isValid()) imgNoduleMap = rhs.imgNoduleMap.copy();
	if(rhs.imgPreprocessed.isValid()) imgPreprocessed = rhs.imgPreprocessed.copy();  
	if(rhs.imgNoduleRibDif.isValid()) imgNoduleRibDif = rhs.imgNoduleRibDif.copy();   

	strcpy(name, rhs.name); 
	strcpy(source, rhs.source); 		
	
	index = rhs.index;;  
	width = rhs.width;
	height = rhs.height;     
	originCol = rhs.originCol;	
	originRow = rhs.originRow;	
	relCandidateCol = rhs.relCandidateCol;	
	relCandidateRow = rhs.relCandidateRow;
	
	minCodeValue = rhs.minCodeValue;
	maxCodeValue = rhs.maxCodeValue;

	probability = rhs.probability;		
	groundtruth = rhs.groundtruth;
	
	overgrow = rhs.overgrow; 	
	undergrow = rhs.undergrow;
	
	boundary = rhs.boundary;
	area = rhs.area;

	featureSet = rhs.featureSet;   
	
	nScale = rhs.nScale;

	return *this;	
}


double Roi::getScore() {

	return getFeatureValue(2);	
}


Roi& Roi::getBest(Roi* roiArray, long size) {   
	
	double bestScore = -1.0;
	long bestK = 0;
	
	for(long k = 0; k < size; k++) {     
		
		double score = roiArray[k].getScore();
		
		if(roiArray[k].getScore() > bestScore) {
			bestScore = score;
			bestK = k;						
		}				
	}	
	
	return roiArray[bestK]; 		
}	


void Roi::setRegionToGroundtruth(Hit& hit) {

	if(!imgNoduleMap.isValid()) {imgNoduleMap = IemTImage<unsigned char>(1, height, width);}  

	long col = getAbsCandidateCol();
	long row = getAbsCandidateRow();                                                   	
	long noduleIndex = hit.getEnclosingNodule(col, row);
	
	for(long c = 0; c < width; c++) {
		for(long r = 0; r < height; r++) {

			col = getAbsCol(c);
			row = getAbsRow(r); 
				
			if(hit.isInsideOutline(noduleIndex, col, row)) imgNoduleMap[0][r][c] = 255;
			else imgNoduleMap[0][r][c] = 0; 
	}}
	
	calculateArea(); 
	mapToBoundary();
}


void Roi::write(ostream& s) {   

	s << "\nRoi: "
	 << "\nname= " << name << " index= " << index
	 << "\nsource= " << source << " nScale= " << nScale
	 << "\ngroundtruth= " << Util::getGtDescription(groundtruth)
	 << "\nprobability= " << probability
	 << "\nwidth= " << width << " height= " << height
	 << "\noriginCol= " << originCol << " originRow= " << originRow
	 << "\nrelCandidateCol= " << relCandidateCol << " relCandidateRow= " << relCandidateRow
	 << "\nabsCandidateCol= " << getAbsCandidateCol() << " absCandidateRow= " << getAbsCandidateRow()
	 << "\nabsMinBoundaryCol= " << getAbsMinBoundaryCol() << " absMaxBoundaryCol= " << getAbsMaxBoundaryCol()
	 << "\nabsMinBoundaryRow= " << getAbsMinBoundaryRow() << " absMaxBoundaryRow= " << getAbsMaxBoundaryRow()
	 << "\narea= " << area
	 << "\novergrow= " << overgrow << "  undergrow= " << undergrow << "  fit= " << getGroundtruthFit() << endl;
	
	featureSet.write(s);
}
	

ostream& operator << (ostream& s, Roi& roi) {  
	
	roi.write(s);  
	
	return s;
}

IemTImage<short> Roi::bilateralcropRoiFromImage(IemTImage<short>& _img) {

	IemTImage<short> imgOut = IemTImage<short>(1, 128, 128);

	long cols = _img.cols();
	long rows = _img.rows();

	for(long y = 0; y < 128; y++) {
		for(long x = 0; x < 128; x++) {

			long xx = x + originCol+192;
			long yy = y + originRow+192;

			if(xx < 0) {xx = 0;}
			if(yy < 0) {yy = 0;}
			if(xx >= cols) {xx = cols - 1;}
			if(yy >= rows) {yy = rows - 1;}

			short v = _img[0][yy][xx];

			imgOut[0][y][x] = v;
	}}
	
	return imgOut;
}

IemTImage<short> Roi::imgtobigbilateral(IemTImage<short>& _img) {

	IemTImage<short> imgOut = IemTImage<short>(1, 512, 512);


    short v;
	for(long y = 0; y < 512; y++) {
		for(long x = 0; x < 512; x++) {


			if((y>191)&(y<320)&(x>191)&(x<320))
			{
              v=_img[0][y-192][x-192];
			  imgOut[0][y][x]=v;
			}

			else
			{
				imgOut[0][y][x]=0;
			}

	
	}}
	
	return imgOut;
}
