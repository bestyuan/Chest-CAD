//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//


#ifndef GMLCLASSIFIER_H
#define GMLCLASSIFIER_H


#include "FeatureSet.h"  
#include "GmlData.h"
#include "Util.h"
#include "Roi.h"



namespace CADX_SEG {


class GmlClassifier  {

	protected:

	GmlProb* probs;


	long nClasses;

	FeatureSet featureSet;

	IemMatrix featureVector;

	public:
	GmlClassifier();

	// Constructor to create a Can for reading.	
	GmlClassifier(GmlData& gmlData);

	~GmlClassifier();
	
	void classify(Roi& roi);
	
	void write(ostream& s);



	protected:
	
	void initialize();  





	
	
};



} // End namespace


#endif
