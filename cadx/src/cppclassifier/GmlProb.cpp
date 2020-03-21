//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "GmlProb.h"





using namespace CADX_SEG;


GmlProb::GmlProb() {
	initialize();
}


GmlProb::GmlProb(long _nFeatures) {
	initialize();
	nFeatures = _nFeatures;
	allocate();
}


void GmlProb::initialize() {
	prior = 0.0;
	logCovarDet = 0.0;
	nFeatures = 0.0;
	logPrior = 0.0;
}


void GmlProb::allocate(long _nFeatures) {
	initialize();
	nFeatures = _nFeatures;
	allocate();
}


void GmlProb::allocate() {
	invCovarMatrix = IemMatrix(nFeatures, nFeatures);
	meanVector = IemMatrix(nFeatures, 1);

	tempVector1 = IemMatrix(nFeatures, 1);
	tempVector2 = IemMatrix(nFeatures, 1);
}


double GmlProb::getProbability(IemMatrix& featureVector) {
	long k;

	// Feature vector is wrong size.
	if(featureVector.rows() != nFeatures) return 0.0;

	// (f - mean)
	for(k = 0; k < nFeatures; k++) { 
		tempVector1[k][0] =  featureVector[k][0] - meanVector[k][0];
	}

	tempVector2 = 0.0;

	// iCov * (f - mean)
	for(k = 0; k < nFeatures; k++) {
		for(long l = 0; l < nFeatures; l++) {
			tempVector2[k][0] +=  invCovarMatrix[k][l] * tempVector1[l][0];
		}
	}

	// (f - mean)t * iCov * (f - mean)
	double distance = 0.0;
	for(k = 0; k < nFeatures; k++) {
		distance += tempVector1[k][0] * tempVector2[k][0];
	}

	double p1 = 0.5 * (distance + logCovarDet) - logPrior;
	
	return exp(-p1);
}


void GmlProb::write(ostream& s) {
	s << "\n\nlabel= " << label;
	s << "\n\nprior= " << prior;
	s << "\n\nlogCovarDet= " << logCovarDet;

	s << "\n\nmean=\n";

	for(long k = 0; k < nFeatures; k++) {
		s << "  " << meanVector[k][0];
	}
	
	s << "\n\ninvCovar=\n";

	for(long r = 0; r < nFeatures; r++) {
	     for(long c = 0; c < nFeatures; c++) {
			s << "  " << invCovarMatrix[r][c];
		}
		s << "\n";
	}

}










