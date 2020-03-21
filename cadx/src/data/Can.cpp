//
//  (c) Copyright Eastman Kodak Company
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "Can.h"    
#include "Tokenizer.h" 
#include "Point.h"
#include <list>



using namespace CADX_SEG;



Can::Can() {
	initialize();
}   


Can::Can(char* fileName) {
	open(fileName);
}   


void Can::open(char* fileName) {
	initialize();
		
	file.open(fileName, ios_base::in);

	if(!file.fail()) valid = 1;

	if(!isValid()) return;

	file.clear();
	
	readHeader();
}        
                
 
Can::Can(char* fileName, long _width, long _height, char* _imageSource) {
	open(fileName, _width, _height, _imageSource);
}


void Can::open(char* fileName, long _width, long _height, char* _imageSource) {
	initialize();
	
	width = _width;
	height = _height;
	
	setImageSource(_imageSource);  
	
	file.open(fileName,ios_base::out);
	
	if(!file.fail()) valid = 1;

	writeHeader();
}    


void Can::setFeatures(Roi& roi) {
	nFeatures = roi.getNFeatures();

	label = new char*[nFeatures];	  
	
	for(long k = 0; k < nFeatures; k++) {
		label[k] = new char[1024];
		strcpy(label[k], roi.getFeatureLabel(k)); 		
	}	
}


Can::~Can() {   	
	if(label != NULL) {
		for(long k = 0; k < nFeatures; k++) delete[] label[k];
		delete [] label;
	}			
}  


void Can::initialize() {
	strcpy(imageSource, "unknown");
	width = 0;
	height = 0; 
	nCandidates = 0; 
	candidateIndex = 0;
	strcpy(typeRegion, "b"); 
	nFeatures = 0;     
	label = NULL; 		
	valid = 0;	
	nCandidatesPos = 0;
	numFeaturesPos = 0;
	flabelsPos = 0;
	nGroups = 0;
}


void Can::writeHeader() {    
	
	file.seekp(0, ios_base::beg);
	
	file << "#^CAN V1.2"  << "\nxraySourceID: " << imageSource
	 << "\nimageSize: " << width << " " << height     
	 << "\nnumCandidates: ";
        nCandidatesPos = file.tellp();
        file << setw(12) << nCandidates;

	file << "\ntypeRegion: " << typeRegion;

	file << "\nnumFeatures: " << nFeatures;
	numFeaturesPos = file.tellp();

	file << "\nflabels: "; 
        flabelsPos = file.tellp();
	
	for(long k = 0; k < nFeatures; k++) file << label[k] << " ";

	file << "\nendHeader:" << endl;
}


void Can::readHeader() {    
	
	char type[1024], version[1024], token[1024];	
	
	file.seekg(0, ios_base::beg);  
	
	file >> type >> version;
	if(strcmp(type, "#^CAN") != 0) {setValid(0); return;}   
	
	Tokenizer tokenizer(file);
	
	while(!tokenizer.EndOfFile()) {    
				
		strcpy(token, tokenizer.NextToken());   
		
		if(strcmp(token, "xraySourceID:") == 0) {
			strcpy(imageSource, tokenizer.NextToken());
		}
		else if(strcmp(token, "imageSize:") == 0) {
			width = atoi(tokenizer.NextToken());     
			height = atoi(tokenizer.NextToken());
		}    
		else if(strcmp(token, "typeRegion:") == 0) {
			strcpy(typeRegion, tokenizer.NextToken());
		}
		else if(strcmp(token, "numCandidates:") == 0) {
			nCandidates = atoi(tokenizer.NextToken()); 	 
		} 		    
		else if(strcmp(token, "numFeatures:") == 0) {
			nFeatures = atoi(tokenizer.NextToken()); 
			label = new char*[nFeatures];	
			for(long k = 0; k < nFeatures; k++) label[k] = new char[256];   
		}     
		else if(strcmp(token, "flabels:") == 0) {
			for(long k = 0; k < nFeatures; k++) strcpy(label[k], tokenizer.NextToken());
		}    
		else if(strcmp(token, "endHeader:") == 0) {
         startCandidatesPos = file.tellg();
			break;
		} 
	}
	
}          
        

void Can::writeCandidate(Roi& roi, double resize) {
	 
	long k;
	char buffer[256];
	
	if(label == NULL) {
	   setFeatures(roi);
	   writeHeader();
	}
	
	file << "\ncandidateIndex: " <<  roi.getIndex();
	
	nCandidates++;
	
	file << "\nnGroups: " << roi.getNGroups();  

	file << "\ngroups: ";
	
	for(k = 0; k < roi.getNGroups(); k++) file << roi.getGroup(k) << " ";
	
	if(roi.getGroundtruth() == Util::GROUNDTRUTH_UNKNOWN) strcpy(buffer, "unknown");
	else if(roi.getGroundtruth() == Util::GROUNDTRUTH_NEGATIVE) strcpy(buffer, "negative");
	else if(roi.getGroundtruth() == Util::GROUNDTRUTH_POSSIBLE) strcpy(buffer, "possible");
	else if(roi.getGroundtruth() == Util::GROUNDTRUTH_PROBABLE) strcpy(buffer, "probable");
	else if(roi.getGroundtruth() == Util::GROUNDTRUTH_DEFINITE) strcpy(buffer, "definite");
			
	file << "\ngroundtruth: " << buffer;  
	
	file << "\nprobability: " << roi.getProbability();  
	
	file << "\nftrVector: ";
	
	if(roi.getNFeatures() > 0) { 
		for(k = 0; k < nFeatures; k++) file << roi.getFeatureValue(k) << " " ;
	}
	
	file << "\ncentroid: " <<  Util::round(roi.getAbsCandidateCol() / resize) << " "
	 << Util::round(roi.getAbsCandidateRow() / resize);
	

	list<Point> boundaryList = roi.getBoundaryList();
	file << "\nnPoints: " << boundaryList.size(); 

	for(list<Point>::iterator iter = boundaryList.begin(); iter != boundaryList.end(); iter++) {
		long col = Util::round(roi.getAbsCol((*iter).x) / resize);
		long row = Util::round(roi.getAbsRow((*iter).y) / resize);
		file << "\n" << col << " " << row;
	}
/*	 
	file << "\nnPoints: " << roi.getNBoundaryPoints();  
	 
	 for(k = 0; k < roi.getNBoundaryPoints(); k++) {
	 	file << "\n" << roi.getAbsBoundaryCol(k) << " " << roi.getAbsBoundaryRow(k); 
	 }
*/	

	file << "\nendCandidate:" << flush;		
	
	// Update the number of candidates field.
	long currentPos = file.tellp();
	file.seekp(nCandidatesPos, ios_base::beg);
	file << setw(12) << nCandidates;
	file.seekp(currentPos, ios_base::beg);
}      


void Can::readNextCandidate(Roi& roi) {
		
	char token[1024];	 
		
	
  //  file.clear();
//	file.seekg(0, ios_base::beg); 

	roi.setSource(imageSource);
	
	Tokenizer tokenizer(file);     
	
	
	while(!tokenizer.EndOfFile()) {
		
		strcpy(token, tokenizer.NextToken());  
		
		if(strcmp(token, "candidateIndex:") == 0) {  
			candidateIndex = atoi(tokenizer.NextToken()); 
			roi.setIndex(candidateIndex);
		} 		
		else if(strcmp(token, "nGroups:") == 0) {
			nGroups = atoi(tokenizer.NextToken());
		}
		else if(strcmp(token, "groups:") == 0) {
		   for(int k = 0; k < nGroups; k++) {
				groups[k] = atoi(tokenizer.NextToken());
				roi.addToGroup(groups[k]);
			}
		}
		else if(strcmp(token, "groundtruth:") == 0) {
			strcpy(token, tokenizer.NextToken());  
			
			if(strcmp(token, "unknown") == 0) roi.setGroundtruth(Util::GROUNDTRUTH_UNKNOWN);
			else if(strcmp(token, "negative") == 0) roi.setGroundtruth(Util::GROUNDTRUTH_NEGATIVE);      
			else if(strcmp(token, "possible") == 0) roi.setGroundtruth(Util::GROUNDTRUTH_POSSIBLE); 
			else if(strcmp(token, "probable") == 0) roi.setGroundtruth(Util::GROUNDTRUTH_PROBABLE);     
			else if(strcmp(token, "definite") == 0) roi.setGroundtruth(Util::GROUNDTRUTH_DEFINITE);      	
		}    
		else if(strcmp(token, "probability:") == 0) {
			roi.setProbability(atof(tokenizer.NextToken()));
		}
		else if(strcmp(token, "ftrVector:") == 0) {  
			
			for(long k = 0; k < nFeatures; k++) {      
				roi.addFeature(label[k], atof(tokenizer.NextToken()));
			}  
		}     
		else if(strcmp(token, "centroid:") == 0) {    
			double xcen = atof(tokenizer.NextToken()); 
			double ycen = atof(tokenizer.NextToken());  
			
			roi.setLocation(xcen, ycen);	 		
		}	
		else if(strcmp(token, "nPoints:") == 0) {
			long nPoints = atoi(tokenizer.NextToken());	  
			
			if(strcmp(typeRegion, "b") == 0) {			

				for(long k = 0; k < nPoints; k++) {
					long col = atoi(tokenizer.NextToken());
					long row = atoi(tokenizer.NextToken());	

					Point point(col, row);
					    
					roi.setAbsBoundaryPoint(point);		  					  
				}  


				roi.setBoundingBox();
			}
		}	
		else if(strcmp(token, "endCandidate:") == 0) {
			break;
		}	

	} 
		

}

















