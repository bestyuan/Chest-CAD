//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef FEATURESTATS_H
#define FEATURESTATS_H



#include "Util.h"  
#include "Roi.h" 
  

namespace CADX_SEG {


class FeatureStats {              
	private:	
	
	long nFeatures; 
	char** label;

	long nPosVectors;
	long nNegVectors;

	long nPosCandidates;
	long nNegCandidates;
	
	double* posMean;
	double* negMean;   
	 
	double* posSigma;
	double* negSigma; 
	
	double* posSum;    
	double* negSum; 
	
	double* posSum2;    
	double* negSum2;  
	
	double* posMin;    
	double* negMin;   
	
	double* posMax;    
	double* negMax;     
	
	double** posCov;
	double** negCov;
	
	double groundtruthUndergrow;	    
	double groundtruthOvergrow;	
	double groundtruthFit;	

	char negFileName[1024], posFileName[1024];
	
	fstream negFile, posFile;	
	ofstream plotFile;	
	ofstream jmpFile;
	
	
	public:    
	FeatureStats(char* plotFileName);    
	
	~FeatureStats(); 
	
	void addRoi(Roi& roi, Hit& hit);  
	
	void calculate();     
	
	void histograms();
	
	void write(ostream& s);

		  	
	private:
	void initialize();   
	
	void allocate(long nFeatures);	  
	
	double getDiscriminant(long k);

};


	ostream& operator << (ostream& s, FeatureStats& stats);



} // End namespace

#endif
