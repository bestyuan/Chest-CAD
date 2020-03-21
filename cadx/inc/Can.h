//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CAN_H
#define CAN_H   


#include "FeatureSet.h"  
#include "Roi.h"      
#include "util.h"   



namespace CADX_SEG {


class Can  {

	private:
	char imageSource[1024];   
	long width;
	long height;     
	long nCandidates; 
	long candidateIndex;
	char typeRegion[256];
	long nFeatures;     
	char** label;  
	
	long nGroups;
	long groups[16];

	short valid; 
	
	fstream file; 

	long nCandidatesPos;
	long numFeaturesPos;
	long flabelsPos;
	long startCandidatesPos;


	public:
	Can();

	// Constructor to create a Can for reading.	
	Can(char* fileName);
	       
	// Constructor to create a Can for writting.
	Can(char* fileName, long width, long height, char* imageSource);      
	

	
	~Can();	
	

	void open(char* fileName);

	void open(char* fileName, long width, long height, char* imageSource);

	void close() {file.close();}
	
	void reset() {file.seekg(0, ios_base::beg); file.clear();}
	
 	void seekgToFirstCandidate() {file.seekg(startCandidatesPos, ios_base::beg); file.clear();}
	
	short isValid() {return valid;}   
	
	void setValid(short v) {valid = v;}                
	
	long getNCandidates() {return nCandidates;}     
	long getNFeatures() {return nFeatures;}    
	char** getLabels() {return label;}    
	
	char* getImageSource() {return imageSource;}
	
	long getWidth() {return width;}
	long getHeight() {return height;}
	
	long getNGroups() {return nGroups;}
	long getGroup(long i) {return groups[i];}

	
	void setImageSource(char *_imageSource) {
		strcpy(imageSource, _imageSource); 
		Util::stripDirectory(imageSource);
	}
	
	void writeCandidate(Roi& roi, double resize = 1.0);
	
	void readNextCandidate(Roi& roi);   
   

	private:    
	
	void initialize();  
	
	void writeHeader();    
	
	void readHeader();  
	
	void setFeatures(Roi& roi);  
	


	
	
	
};



} // End namespace


#endif
