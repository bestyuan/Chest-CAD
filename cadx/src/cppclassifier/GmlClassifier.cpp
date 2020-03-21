//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#include "GmlClassifier.h"



using namespace CADX_SEG;



GmlClassifier::GmlClassifier() {  
	initialize(); 
}   


GmlClassifier::GmlClassifier(GmlData& gmlData) { 
	initialize();

	nClasses = gmlData.getNClasses();
	
	featureSet = gmlData.getFeatureSet();

	probs = new GmlProb[nClasses];
	
	for(long k = 0; k < nClasses; k++) {
		gmlData.readNext(probs[k]);
	} 
	
	long nFeatures = featureSet.getNFeatures();
	
	featureVector = IemMatrix(nFeatures, 1);
}


GmlClassifier::~GmlClassifier() { 
	if(probs != NULL) delete[] probs;
}


void GmlClassifier::initialize() {
	nClasses = 0;
	probs = NULL;
}


void GmlClassifier::classify(Roi& roi) {

	// Segmentation failed
	if(roi.getFeatureValue("Shape.areaFraction") == 0) {
		roi.setProbability(0.0);
		return;
	}

	long nFeatures = featureSet.getNFeatures();

	// Populate the feature vector.
	for(long k = 0; k < nFeatures; k++) {  
		double v = roi.getFeatureValue(featureSet.getLabel(k));
		featureVector[k][0] = v;

//		cout << "\ncandidate= " << roi.getIndex() << ", label= " << featureSet.getLabel(k)
//		 << ", value= " << v;
	}

	double prob0 = probs[0].getProbability(featureVector);
	double prob1 = probs[1].getProbability(featureVector);

	// Both probabilities are too low.
	if(prob0 < DBL_MIN && prob1 < DBL_MIN) {
		roi.setProbability(0.0);
		return;
	}

	roi.setProbability(prob0 / (prob0 + prob1));
}


void GmlClassifier::write(ostream& s) {  

	featureSet.write(s);

	for(long k = 0; k < nClasses; k++) {
		s << "\n\n\n";
		probs[k].write(s);
	}
}




