//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef CALCFEATURES_H
#define CALCFEATURES_H


#include "Iem/Iem.h"   
#include "UserAgent.h"
#include "Hit.h"
#include "Can.h"
#include "Cnl.h"


namespace CADX_SEG {

  

class CalcFeatures  { 

	private:
	IemTImage<short> img;
	IemTImage<unsigned char> imgAnatomicalMap;

	long nCandidates;
	long processedCanList[1024];

	char imgFileName[1024];
	char maskFileName[1024];
	char baseName[1024];     
	char outCanFileName[1024];
	
	double runtime;
	
	UserAgent userAgent;
	
	double minProbability;
	
	Hit hit; 
	Can inCan; 
	Can outCan; 	
	Cnl cnl;   
	
	Roi *roiArray;
	

	void initialize();	
	
	void process(Roi& roi);		

	public: 

	CalcFeatures();

	~CalcFeatures();	
	
	double getRuntime() {return runtime;}
	
	void run(char* imgFileName, char* maskFileName, char* inCanFileName,
 	 char* outCanFileName, char* hitFileName = NULL, 
	 short candidatesFromHitFile = 0, short hitsFirst = 0, 
	 short onlyHits = 0, long onlyCandidate = -1);

	void setMinProbability(double _minProbability) {minProbability = _minProbability;}


	void processHit();

	void processCan(long hitsOnly = 0, long onlyCandidate = -1); 
	
	// return 1 if candidate i was processed in a previous call to processCan.
	long wasProcessed(long i);

	void setUserAgent(UserAgent& agent) {userAgent = agent;}

};



} // Namespace CADX_SEG



#endif
