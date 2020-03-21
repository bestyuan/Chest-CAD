//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef GMLPROB_H
#define GMLPROB_H


#include "FeatureSet.h"
#include "util.h"
#include "Iem/IemMatrix.h"



namespace CADX_SEG {


class GmlProb  {

	private:
	char label[1024];

	IemMatrix meanVector;
	IemMatrix invCovarMatrix;

	// Preallocated vector to hold (featureVector -  meanVector).
	IemMatrix tempVector1;
	
	// Preallocated vector to hold invCovarMatrix * tempVector1.
	IemMatrix tempVector2;

	double prior;

	// Save log of prior so no need to recalculate.
	double logPrior;

	double logCovarDet;

	long nFeatures;
	
	long nProbs;

	public:
	GmlProb();

	// Constructor to create a Can for reading.
	GmlProb(long nFeatures);

	~GmlProb(){;}

	void setPrior(double v) {prior = v; logPrior = log(prior);}
	void setLogCovarDet(double v) {logCovarDet = v;}
	
	void setLabel(char* s) {strcpy(label, s);}
	
	void setMean(long i, double v) {meanVector[i][0] = v;}
	double getMean(long i) {return meanVector[i][0];}

	void setInvCovar(long c, long r, double v) {invCovarMatrix[r][c] = v;}
	double getInvCovar(long c, long r) {return invCovarMatrix[r][c];}
	
	double getProbability(IemMatrix& featureVector);
	
	void write(ostream& s);

	void allocate(long nFeatures);

	protected:
	
	void initialize();

	void allocate();
	
	
};



} // End namespace


#endif
