//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef ROI_H
#define ROI_H


#include "Iem/Iem.h"   

#include "Hit.h"   
#include "FeatureSet.h"  
#include <list>
#include "Point.h"
#include "Boundary.h"
 


namespace CADX_SEG {
  

class Roi { 

    public:
	long originCol;
	long originRow;	
	long relCandidateCol;	
	long relCandidateRow;
	IemTImage<short> imgBilateral;
	IemTImage<short> bigimgBilateral;
	IemTImage<short> smallROI;
	private:
	IemTImage<short> imgCropped; 
	IemTImage<short> imgCroppedribsup;
	IemTImage<short> imgPreprocessed;
	IemTImage<short> imgPreprocessedribsup;
	
	IemTImage<short> imgNoduleRibDif;
	IemTImage<short> imgNoduleRibDifribsup;

	IemTImage<unsigned char> imgNoduleMap;
	IemTImage<unsigned char> imgNoduleMapribsup;

	IemTImage<unsigned char> imgAnatomicalMap;
	IemTImage<unsigned char> imgAnatomicalMapribsup;

	char name[1024];
	char source[1024];

	bool valid;
	long index;  
	long width;
	long height;     

	
	short minCodeValue;
	short maxCodeValue;

	Boundary boundary;
	Boundary boundaryribsup;

	long area;
	
	long nScale;			// The scale that the Roi belongs to.
	
	double probability;		// The calculated probability that candidate is a nodule.
	long groundtruth;		// The ground-truth associated with the candidate.   

	FeatureSet featureSet;

	double overgrow; 		// Region overgrow with respect to groundtruth.
	double undergrow;		// Region undergrow with respect to groundtruth.

	long nGroups;			// Number of groups the ROI belongs to.
	long groups[16];		// Array of group numbers.

	public:            
	Roi();   
	     
	// Creates a ROI with specified center.	
	Roi(char* name, IemTImage<short>& img, IemTImage<unsigned char>& imgAnatomicalMap,
	 long candidateCol, long candidateRow, long groundtruth);
	                  	 
	~Roi() {free();}   
	
	void initialize();
	void free();
	
	// Returns an 8-bit version of the image with the boundary of the region map marked.
	IemTImage<unsigned char> getAnnotated();
	
	// Returns an 8-bit version of the image with both the boundary of the region map
	// and ground-truth marked.
	IemTImage<unsigned char> getAnnotated(long* outlineCol, long* outlineRow, long nOutlinePoints);

	IemTImage<short> getCroppedImage() {return imgCropped.copy();}  
	IemTImage<short> getPreprocessedImage() {return imgPreprocessed.copy();}
	IemTImage<short> getPreprocessedImageribsup() {return imgPreprocessedribsup.copy();}
	IemTImage<short> getNoduleRibDifImage() {return imgNoduleRibDif.copy();}
	IemTImage<short> getNoduleRibDifImageribsup() {return imgNoduleRibDifribsup.copy();}
	IemTImage<unsigned char> getAnatomicalMap() {return imgAnatomicalMap.copy();}
	IemTImage<unsigned char> getNoduleMap() {return imgNoduleMap.copy();}  	

	void setValid(bool v) {valid = v;}
	bool isValid() {return valid;}

	char* getName();	         
	long getIndex() {return index;}	
	long getCandidateRow() {return relCandidateRow;}	
	long getCandidateCol() {return relCandidateCol;}
	long getAbsCandidateRow() {return getAbsRow(relCandidateRow);}	
	long getAbsCandidateCol() {return getAbsCol(relCandidateCol);}   
   
	long getArea() {return area;}  
	
	long getNFeatures() {return featureSet.getNFeatures();}     
	double getFeatureValue(long k) {return featureSet.getValue(k);}       
	char* getFeatureLabel(long k) {return featureSet.getLabel(k);}   
	

	long getGroundtruth() {return groundtruth;}	
	double getProbability() {return probability;}

	list<Point>& getBoundaryList() {return boundary.getList();}
	Boundary& getBoundary() {return boundary;}

	long getNBoundaryPoints() {return boundary.getList().size();}
/*
	long getAbsBoundaryCol(long k) {return getAbsCol(boundaryCol[k]);}   
	long getAbsBoundaryRow(long k) {return getAbsRow(boundaryRow[k]);}
*/
	double getGroundtruthFit();
	double getGroundtruthUndergrow() {return undergrow;}	
	double getGroundtruthOvergrow() {return overgrow;}	
	
	double getScore(); 
	
	long getNGroups() {return nGroups;}
	long getGroup(long i) {return groups[i];}
	
	void setScaleNum(long n) {nScale = n;}
	long getScaleNum() {return nScale;}

	void addToGroup(long g) {if(nGroups >= 16) return; groups[nGroups++] = g;}

	double getFeatureValue(char* name) {return featureSet.getValue(name);}
	
	short hasFeature(char* name) {return featureSet.hasFeature(name);}

	void setArea(long _area) {area = _area;}
	
	void setBoundingBox() {boundary.setBoundingBox();}

	void setIndex(long _index) {index = _index;}   	                                                      
	void setName(char* _name) {strcpy(name, _name);}   	                                                      
	void setGroundtruth(long _groundtruth) {groundtruth = _groundtruth;}
	void setProbability(double _probability) {probability = _probability;}
	void setMinCodeValue(short _minCodeValue) {minCodeValue = _minCodeValue;}
	void setMaxCodeValue(short _maxCodeValue) {maxCodeValue = _maxCodeValue;}

	void setCroppedImage(IemTImage<short>& _img) {imgCropped = _img.copy();}
	void setPreprocessedImage(IemTImage<short>& _img) {imgPreprocessed = _img.copy();}
	void setPreprocessedImageribsup(IemTImage<short>& _imgribsup) {imgPreprocessedribsup = _imgribsup.copy();}
	void setNoduleRibDifImage(IemTImage<short>& _img) {imgNoduleRibDif = _img.copy();}
	void setNoduleRibDifImageribsup(IemTImage<short>& _imgribsup) {imgNoduleRibDifribsup = _imgribsup.copy();}

	// Returns an image cropped from the input image that corresponds to the Roi.
	IemTImage<short> cropRoiFromImage(IemTImage<short>& _img);

	IemTImage<short> bilateralcropRoiFromImage(IemTImage<short>& _img);
	IemTImage<short> imgtobigbilateral(IemTImage<short>& _img);
	
	// Hand the ROI the source image.
//	void setSourceImage(IemTImage<short>& _img) {cropImage(_img);}
	
	void setSourceImage(IemTImage<short>& _img, IemTImage<unsigned char>& _imgMask) {cropImage(_img, _imgMask);}
	
	// Have the ROI read the source image using the sourceImageName field.
//	void setSourceImage(); 
	    
	void setNoduleMap(IemTImage<unsigned char>& _imgNoduleMap) 
		{imgNoduleMap = _imgNoduleMap.copy(); calculateArea(); mapToBoundary();} 

	void setNoduleMapribsup(IemTImage<unsigned char>& _imgNoduleMapribsup) 
		{imgNoduleMapribsup = _imgNoduleMapribsup.copy(); calculateArearibsup(); mapToBoundaryribsup();} 

	void setLocation(long candidateCol, long candidateRow);

	// Creates boundary points from the map.
	void mapToBoundary();
	void mapToBoundaryribsup();

	void setSource(char* name) {strcpy(source, name);}
	char* getSource() {return source;}
	
	void setAbsBoundaryPoint(Point& _point) {
		Point point(getRelCol(_point.x), getRelRow(_point.y));
		boundary.addPoint(point);
	}
	
	long getAbsCol(long col) {return col + originCol;}  
	long getAbsRow(long row) {return row + originRow;}
	long getRelCol(long col) {return col - originCol;}
	long getRelRow(long row) {return row - originRow;}
	
	long getAbsMinBoundaryCol() {return getAbsCol(boundary.getMinCol());}
	long getAbsMaxBoundaryCol() {return getAbsCol(boundary.getMaxCol());}
	long getAbsMinBoundaryRow() {return getAbsRow(boundary.getMinRow());}
	long getAbsMaxBoundaryRow() {return getAbsRow(boundary.getMaxRow());}
	  

	void addFeature(char* label, double value) {featureSet.addFeature(label, value);}     
      

	double compareRegionToGroundtruth(Hit& hit);
	
	void setRegionToGroundtruth(Hit& hit);
	                                   
	// Tests if a pixel in Roi is inside the boundary using relative coordinates.
	bool isInsideBoundary(long col, long row);

	// Tests if a pixel in Roi is inside the boundary using absolute coordinates.
	bool isInsideAbsBoundary(long col, long row);

	Roi& operator=(const Roi& rhs);     

	static Roi& getBest(Roi* roiArray, long size);	  
	
	void cropImage(IemTImage<short>& img, IemTImage<unsigned char>& _imgAnatomicalMap);
	void cropImageribsup(IemTImage<short>& imgribsup, IemTImage<unsigned char>& _imgAnatomicalMapribsup);
//	void cropImage(IemTImage<short>& img);
	
	void write(ostream& s);  

	
	private:
	
	void calculateArea();
	void calculateArearibsup();
	void calculateMemberArrays();  

	

	
};

ostream& operator << (ostream& s, Roi& roi);  


} // End namespace


#endif
